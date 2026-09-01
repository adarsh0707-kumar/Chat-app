# API Reference — meta-micro

Base URL: `/api` (proxied by the frontend dev server / nginx to the
FastAPI backend). Interactive, always-current docs are also available at
`GET /docs` (Swagger UI) and `GET /redoc` whenever the backend is
running, generated from the same Pydantic schemas as this document.

## Conventions

- **Auth**: every endpoint except `POST /auth/signup`, `POST /auth/login`,
  and `GET /health` requires `Authorization: Bearer <JWT>`.
- **Tenancy**: every resource is implicitly scoped to the authenticated
  user's institute — there is no `institute_id` parameter on any
  request; it's derived from the token.
- **Roles**: routes marked **admin only** additionally require the
  authenticated user's `role == admin` (403 otherwise). Everything else
  just requires a valid, active user (admin or teacher).
- **Errors**: standard FastAPI/Pydantic error shape,
  `{"detail": "..."}` for 4xx, or a list of validation errors for 422.

---

## Auth — `/api/auth`

| Method | Path | Auth | Description |
|---|---|---|---|
| POST | `/signup` | none | Creates an institute + its first admin user in one call. Returns a JWT. |
| POST | `/login` | none | Authenticates an existing user by email/password. Returns a JWT. |
| GET | `/me` | any user | Returns the current user and their institute. |
| POST | `/teachers/invite` | **admin only** | Creates a teacher account under the admin's institute with a temp password. |
| GET | `/teachers` | **admin only** | Lists all teachers in the institute. |

**`POST /signup`**
```json
// request
{
  "institute_name": "Bright Future Coaching",
  "city": "Patna",
  "admin_name": "Adarsh Kumar",
  "email": "admin@brightfuturecoaching.co.in",
  "password": "supersecret123",
  "default_language": "hi"
}
// response 201
{ "access_token": "eyJ...", "token_type": "bearer" }
```

**`GET /me`**
```json
{
  "user": { "id": 1, "institute_id": 1, "name": "...", "email": "...", "phone": null, "role": "admin", "is_active": true },
  "institute": { "id": 1, "name": "...", "city": "Patna", "default_language": "hi" }
}
```

---

## Batches — `/api/batches`

| Method | Path | Description |
|---|---|---|
| GET | `/` | List batches. Query: `search` (name substring match). |
| POST | `/` | Create a batch. |
| GET | `/{batch_id}` | Get one batch (includes `student_count`). |
| PUT | `/{batch_id}` | Replace a batch's editable fields. |
| DELETE | `/{batch_id}` | Delete a batch; its students are unassigned (`batch_id → NULL`), not deleted. |

Request/response body (`BatchCreate` / `BatchOut`):
```json
{
  "name": "Class 10 Science",
  "subject": "Science",
  "schedule_days": "Mon-Fri",
  "timing": "4-6 PM",
  "monthly_fee": 1500
}
```

---

## Students — `/api/students`

| Method | Path | Description |
|---|---|---|
| GET | `/` | List students. Query: `search` (name), `batch_id`. |
| POST | `/` | Create a student, optionally assigned to a batch. |
| GET | `/{student_id}` | Get one student. |
| PUT | `/{student_id}` | Update a student (also toggles `is_active`). |
| DELETE | `/{student_id}` | Delete a student and cascade-delete their fees/attendance/scores. |

```json
{
  "name": "Rahul Kumar",
  "batch_id": 1,
  "parent_name": "Suresh Kumar",
  "parent_phone": "+919000000001",
  "phone": null,
  "admission_date": "2026-04-01",
  "monthly_fee": 1500
}
```

---

## Fees — `/api/fees`

| Method | Path | Description |
|---|---|---|
| GET | `/` | List fee records. Query: `status_filter` (`due`/`paid`/`overdue`), `batch_id`, `student_id`. |
| GET | `/summary` | Aggregate counts: total outstanding amount, overdue/due/paid counts. |
| POST | `/` | Create a fee record for a student for a billing `period`. |
| POST | `/{fee_id}/mark-paid` | Marks a fee paid with the given `amount_paid`; sets `paid_at`. |

```json
// POST /
{ "student_id": 1, "period": "2026-08", "amount_due": 1500, "due_date": "2026-08-10" }

// POST /{id}/mark-paid
{ "amount_paid": 1500 }
```

---

## Attendance — `/api/attendance`

| Method | Path | Description |
|---|---|---|
| GET | `/` | Get attendance for a batch on a date. Query (required): `batch_id`, `on_date`. |
| POST | `/` | Upsert attendance for a batch/date — one entry per student. |

```json
// POST /
{
  "batch_id": 1,
  "date": "2026-08-24",
  "entries": [
    { "student_id": 1, "status": "present" },
    { "student_id": 2, "status": "absent" }
  ]
}
```

Resubmitting for the same `(batch_id, date)` updates existing records
rather than creating duplicates.

---

## Test Scores — `/api/test-scores`

| Method | Path | Description |
|---|---|---|
| GET | `/` | List score entries. Query: `batch_id`, `student_id`. |
| POST | `/` | Record one student's score for one test. |
| GET | `/report` | Per-student aggregate (tests taken, total marks, percentage) for a batch, sorted descending by percentage. Query (required): `batch_id`. |

```json
// POST /
{
  "student_id": 1,
  "batch_id": 1,
  "test_name": "Unit Test 1",
  "subject": "Physics",
  "max_marks": 50,
  "marks_obtained": 42,
  "test_date": "2026-08-20",
  "remarks": null
}
```

---

## Templates — `/api/templates`

| Method | Path | Description |
|---|---|---|
| GET | `/` | List templates. Query: `category` (`fee_reminder`/`parent_update`). |
| POST | `/` | Create a template. |
| PUT | `/{template_id}` | Replace a template's fields. |
| DELETE | `/{template_id}` | Delete a template. |

```json
{
  "name": "Fee Reminder Hindi",
  "category": "fee_reminder",
  "language": "hi",
  "body": "Namaste {parent_name}, {student_name} ki fees {monthly_fee} abhi baaki hai."
}
```

**Supported placeholders** (substituted at send time):
`{student_name}`, `{parent_name}`, `{monthly_fee}`, `{institute_name}`,
plus `{period}` for automation-triggered fee reminders.

---

## Messaging — `/api/messaging`

| Method | Path | Description |
|---|---|---|
| POST | `/fee-reminders` | Renders `template_id` for each of `student_ids` and sends via WhatsApp. |
| POST | `/parent-updates` | Same, for the parent-update category. |
| GET | `/logs` | Last 200 sent messages for the institute, newest first. |

```json
// request (both endpoints)
{ "template_id": 3, "student_ids": [1, 2, 5] }

// response — one MessageLog per student with a resolvable phone number
[
  {
    "id": 12, "student_id": 1, "recipient_phone": "+919000000001",
    "category": "fee_reminder", "body": "Namaste Suresh Kumar, ...",
    "status": "sent", "sent_at": "2026-08-24T18:19:43Z"
  }
]
```

A student with no `parent_phone` and no `phone` is silently skipped
(no log row, no error) — the response only contains students that
actually had a deliverable number.

---

## Automations — `/api/automations`

**Admin only.**

| Method | Path | Description |
|---|---|---|
| GET | `/` | List the institute's automation settings (one per category). |
| PUT | `/` | Create or update the automation for a category. |

```json
{ "category": "fee_reminder", "enabled": true, "day_of_month": 5, "template_id": 3 }
```

When `enabled`, a daily background job (see `docs/ARCHITECTURE.md` §8)
checks whether today matches `day_of_month` and, if so, sends the
configured template to every relevant student and logs the sends.

---

## Setup — `/api/setup`

| Method | Path | Description |
|---|---|---|
| GET | `/status` | Onboarding checklist state: whether the institute has any batches/students/templates yet, plus counts. |
| POST | `/import-students` | Bulk-create students from a JSON array (same shape as `POST /students`, repeated). No frontend UI yet — callable directly. |

```json
// POST /import-students
[
  { "name": "Student A", "batch_id": 1, "parent_phone": "+91..." },
  { "name": "Student B", "batch_id": 1, "parent_phone": "+91..." }
]
```

---

## Health

| Method | Path | Auth | Description |
|---|---|---|---|
| GET | `/api/health` | none | Liveness check — `{"status": "ok", "app": "meta-micro"}`. |
