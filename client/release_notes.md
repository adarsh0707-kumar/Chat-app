# Chat Application v1.0.0 Release

## 🎉 First Stable Release

This is the initial release of our real-time multi-client chat application built with C++ using POSIX sockets.

## ✨ Features

### Core Functionality
- **Real-time messaging**: Broadcast messages to all connected clients
- **Multi-client support**: Handle multiple simultaneous connections
- **Thread-safe operations**: Mutex-protected client management

### Advanced Features
- **Private messaging**: Send direct messages using `/msg <username> <message>`
- **Timestamps**: All messages include HH:MM timestamps
- **User management**: Unique usernames with join/leave notifications
- **Commands**: `/help` to see available commands

### Robustness
- **Input validation**: Prevents empty or duplicate usernames
- **Error handling**: Comprehensive error checking for network operations
- **Graceful disconnects**: Proper client removal and notifications

## 🏗️ Architecture

- **Server**: Multi-threaded server using C++17
- **Client**: Single-threaded client with background message receiver
- **Protocol**: Simple TCP-based text protocol
- **Threading**: std::thread with atomic flags for synchronization

## 📁 Project Structure

```
chat-app/
├── server/
│   ├── src/server.cpp
│   ├── include/server.h
│   └── Makefile
├── client/
│   ├── client.cpp
│   ├── include/client.h
│   └── Makefile
└── README.md
```

## 🚀 Getting Started

### Prerequisites
- C++17 compatible compiler (g++)
- Linux/Unix environment

### Build & Run
```bash
# Build server
cd server && make

# Build client  
cd client && make

# Run server
./server/server

# Run clients (multiple terminals)
./client/client
```

## 🔧 Commands

- `/msg <username> <message>` - Private message
- `/help` - Show commands
- `exit` - Quit client

## 📈 Roadmap

Future enhancements planned:
- User authentication
- Message history
- File sharing
- GUI interface
- Encryption support

## 🤝 Contributing

This project demonstrates advanced C++ concepts including:
- Socket programming
- Multi-threading
- Memory management
- Error handling
- Build systems

## 📄 License

MIT License - see README.md for details

---

**Release Date:** Thursday 30 April 2026 03:25:03 PM IST
**Tag:** v1.0.0
**Commit:** 8e797b6796d743c53ff6662463b8d9cad1483d85
