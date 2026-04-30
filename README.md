# Chat Application

A real-time multi-client chat application built with C++ using sockets. Features include private messaging, timestamps, and user management.

## Features

- **Real-time messaging**: Broadcast messages to all connected clients
- **Private messaging**: Send direct messages using `/msg <username> <message>`
- **Timestamps**: All messages include timestamps in HH:MM format
- **User management**: Unique usernames, join/leave notifications
- **Commands**: `/help` to see available commands
- **Input validation**: Prevents empty or duplicate usernames
- **Error handling**: Robust handling of network errors and disconnections

## Project Structure

```
chat-app/
├── server/
│   ├── src/
│   │   └── server.cpp
│   ├── include/          # (for future headers)
│   └── Makefile
├── client/
│   ├── client.cpp
│   └── Makefile
└── README.md
```

## Building and Running

### Prerequisites

- C++11 compatible compiler (g++)
- Linux/Unix environment with socket support

### Build Server

```bash
cd server
make
```

### Build Client

```bash
cd client
make
```

### Run

1. Start the server:

   ```bash
   ./server/server
   ```

2. Start clients in separate terminals:

   ```bash
   ./client/client
   ```

3. Enter a unique username when prompted

4. Start chatting! Use `/msg username message` for private messages.

### Commands

- `/msg <username> <message>` - Send private message
- `/help` - Show available commands
- `exit` - Quit the client

## Technical Details

- **Server**: Multi-threaded server handling multiple clients concurrently
- **Client**: Single-threaded client with separate thread for receiving messages
- **Protocol**: Simple text-based protocol over TCP
- **Threading**: Uses C++11 threads and mutexes for synchronization
- **Networking**: POSIX sockets for cross-platform compatibility

## Future Enhancements

- User authentication
- Message history
- File sharing
- GUI interface
- Encryption
- Admin commands

## License

This project is open source and available under the MIT License.
