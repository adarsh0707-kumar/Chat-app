# Product Requirements Document — meta-micro

| | |
|---|---|
| **Product** | meta-micro |
| **Type** | Micro SaaS |
| **Status** | MVP implemented |
| **Owner** | Adarsh Kumar |
| **Last updated** | 2026-08-24 |

## 1. Summary

meta-micro is a micro SaaS that automates the day-to-day administrative
work of small, local coaching centres (tuition institutes) in Patna. It
replaces registers, spreadsheets, and manual phone calls with a single
bilingual (Hindi/English) web app that manages students and batches,
tracks monthly fee dues, records attendance, captures test scores, and
sends fee reminders and parent updates directly to parents' WhatsApp.

## 2. Problem statement

Coaching-centre admins in tier-2/tier-3 cities run their institutes on
paper registers and ad-hoc WhatsApp messages. This creates four
recurring pain points:

1. **Fee collection is manual and inconsistent.** Admins track dues in
   notebooks or Excel and call/message parents individually, so
   reminders are late, inconsistent, or skipped entirely.
2. **Attendance and test scores live in paper registers**, making them
   slow to record, easy to lose, and impossible to analyze.
3. **Parent communication has no structure.** Every admin writes fee and
   progress messages from scratch, in whichever language they're
   comfortable with, with no record of what was sent to whom.
4. **Existing SaaS tools are built for large institutions** — English-only,
   feature-heavy, and priced for scale — and don't fit a 50–150 student
   neighbourhood coaching centre.

## 3. Goals

| Goal | How meta-micro addresses it |
|---|---|
| Cut the time admins spend on fee follow-ups | One-click bulk WhatsApp fee reminders from a live dues dashboard |
| Make attendance and scores digital without new hardware | Web-based attendance sheet and score entry, usable on any phone/laptop |
| Make communication consistent and bilingual | Reusable Hindi/English message templates with placeholders |
| Fit a solo-admin or small-staff operation | Two roles only (admin, teacher); setup takes minutes, not days |

### Non-goals (out of scope for MVP)

- Online fee payment / payment gateway integration (fees are marked paid
  manually once collected offline).
- Student-facing or parent-facing login/portal (parents are reached only
  via WhatsApp, not a shared account).
- Multi-branch / franchise management across institutes.
- SMS or email as delivery channels (WhatsApp only).
- Native mobile apps (the web app is responsive but not packaged for
  app stores).

## 4. Target users & personas

### Coaching-centre administrator (primary)
Owns or manages the institute. Responsible for enrolling students,
organizing batches, tracking fees, and communicating with parents. Not
necessarily technical — needs an interface that's simple enough to use
with minimal training, in Hindi or English.

**Needs:** a dues dashboard, one-click bulk WhatsApp sends, bilingual
templates, and a setup flow that doesn't require a developer.

### Teacher (secondary)
Assigned to one or more batches. Marks attendance and enters test scores
for their students. Does not manage fees, templates, or automations.

**Needs:** a fast way to mark a whole batch present/absent and enter
marks for a test in one sitting.

## 5. Functional requirements

Numbered `FR-*` requirements map directly to what's implemented; see
`docs/API_REFERENCE.md` for the corresponding endpoints.

| ID | Requirement | Persona |
|---|---|---|
| FR-1 | Admin can sign up, which creates an institute and an admin account in one step | Admin |
| FR-2 | Admin can invite teachers, who log in with institute-scoped accounts | Admin |
| FR-3 | Admin can create, view, edit, and delete batches (class groups) | Admin |
| FR-4 | Admin can create, view, edit, and delete students, and assign them to a batch | Admin |
| FR-5 | Admin can view a dashboard of outstanding/overdue/paid fees, filterable by batch or student | Admin |
| FR-6 | Admin can create a fee record for a student for a billing period and mark it paid | Admin |
| FR-7 | Teacher can mark attendance (present/absent/late) for a batch on a given date | Teacher |
| FR-8 | Teacher can enter test scores (subject, max marks, marks obtained) for students in a batch | Teacher |
| FR-9 | Admin/teacher can view a per-batch test report ranked by percentage | Both |
| FR-10 | Admin can create, edit, and delete bilingual (Hindi/English) message templates for two categories: fee reminders and parent updates | Admin |
| FR-11 | Admin can select a template and a set of student recipients and send a WhatsApp broadcast; every send is logged | Admin |
| FR-12 | Admin can enable a monthly automation per category (fee reminder / parent update) that fires on a configured day of the month using a chosen template | Admin |
| FR-13 | Admin can walk through a setup checklist (batches → students → templates) after signup | Admin |
| FR-14 | UI text and message templates support both Hindi and English, switchable at any time | Both |

## 6. Non-functional requirements

| ID | Requirement | Notes |
|---|---|---|
| NFR-1 | Institute data must be isolated per tenant | Every query is scoped by `institute_id`; enforced at the router layer via the authenticated user's institute |
| NFR-2 | Authentication must be secure | JWT bearer tokens, bcrypt-hashed passwords, role-gated endpoints (admin-only vs. any authenticated user) |
| NFR-3 | The app must degrade gracefully without WhatsApp credentials | A "log" provider records messages instead of sending them, so the app is fully usable in dev/demo without a Meta Business account |
| NFR-4 | The system should support many institutes concurrently | Stateless FastAPI app + PostgreSQL, horizontally scalable behind a load balancer; Docker/Kubernetes-ready |
| NFR-5 | UI must be usable by non-technical admins | Large touch targets, plain-language copy, a guided setup checklist, bilingual throughout |

## 7. Core user flows

1. **Admin enrollment & setup** — sign up → land on Setup checklist → add
   batches → add students → add templates → ready to send.
2. **Teacher attendance** — teacher logs in → Attendance page → picks
   batch + date → marks each student → submits.
3. **Fee reminder automation** — admin writes a fee-reminder template →
   enables the Fee Reminders automation with a day-of-month → the
   scheduler sends it automatically to every student with a due/overdue
   fee on that day each month.
4. **Test score entry & reporting** — teacher enters scores per test →
   admin/teacher views the auto-generated percentage report per batch.

## 8. Success metrics (indicative, for a live deployment)

- % of fee reminders sent via automation vs. manual bulk-send (adoption
  of automation over manual work).
- Median days-late on fee collection, before vs. after adoption.
- % of institutes that complete the Setup checklist within 24 hours of
  signup (onboarding friction).
- WhatsApp delivery success rate (`message_logs.status`).

## 9. Assumptions & constraints

- The product targets local coaching institutes in Patna, but nothing in
  the implementation is Patna-specific beyond the default city value —
  it generalizes to any city.
- Initial data setup (batches, students, templates) is done manually
  through the UI; there is a basic bulk student-import endpoint
  (`POST /api/setup/import-students`) but no bulk UI for it yet.
- WhatsApp delivery requires a parent (or student) phone number and,
  for real sends, a configured Meta WhatsApp Business Cloud API
  integration — this is a prerequisite, not something meta-micro
  provisions.

## 10. Glossary

| Term | Definition |
|---|---|
| Institute | A coaching centre / tenant. All data is scoped to one institute. |
| Batch | A class/section grouping of students (e.g. "Class 10 Science"). |
| Coaching-centre administrator | The institute's primary user; manages everything. |
| Teacher | A secondary user scoped to attendance and test scores. |
| Template | A reusable, bilingual WhatsApp message with placeholders like `{student_name}`. |
| Automation | A scheduled, recurring WhatsApp send (fee reminder or parent update) fired monthly on a configured day. |

## 11. Future enhancements (not in MVP)

- Online/UPI fee payment collection with automatic fee-status updates.
- SMS fallback for parents without WhatsApp.
- Bulk CSV import UI for students (backend endpoint already exists).
- Analytics dashboard (attendance trends, fee collection trends).
- Per-teacher batch assignment/permissions (currently any teacher can
  access any batch in their institute).
