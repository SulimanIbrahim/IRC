# Understanding Sockets in Network Programming

## Introduction
A socket is a fundamental building block for network communication, serving as an endpoint for process-to-process communication across a computer network. This guide explains the core concepts and implementation details of socket programming.

## 🌟 Real-World Analogy
Think of a socket like a phone system:
- Socket ≈ Phone Line
- IP Address ≈ Phone Number
- Port ≈ Extension Number
- Connection ≈ Phone Call between two points

## 🔧 Technical Overview
A socket provides an interface for network communication enabling:
- Process-to-process communication
- Data exchange between different computers
- Communication using various protocols (TCP/IP, UDP)

## 📚 Types of Sockets

### 1. Stream Sockets (SOCK_STREAM)
- Uses TCP (Transmission Control Protocol)
- Provides reliable, ordered data delivery
- Connection-oriented (like a phone call)
- Common applications:
  - Web browsers
  - SSH
  - FTP

### 2. Datagram Sockets (SOCK_DGRAM)
- Uses UDP (User Datagram Protocol)
- No guaranteed delivery
- Connectionless (like sending letters)
- Common applications:
  - DNS
  - Streaming media
  - Online gaming

## 🔄 Socket Creation Process

### 1. Create a Socket
```cpp
int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
```
- `AF_INET`: Uses IPv4
- `SOCK_STREAM`: TCP protocol
- Returns a file descriptor for the socket

### 2. Bind the Socket (Server-side)
```cpp
bind(socket_fd, address, address_length);
```
- Assigns an address to the socket
- Similar to registering a phone number

### 3. Listen for Connections (Server-side)
```cpp
listen(socket_fd, backlog);
```
- Marks socket as passive
- Prepares to accept connections
- `backlog` defines maximum connection queue length

### 4. Accept Connections (Server-side)
```cpp
accept(socket_fd, client_address, address_length);
```
- Waits for incoming connections
- Creates new socket for each connected client

### 5. Connect to Server (Client-side)
```cpp
connect(socket_fd, server_address, address_length);
```
- Initiates connection to server
- Used by clients to connect to listening servers

## 📊 Socket States

| State | Description | Triggered By |
|-------|-------------|-------------|
| Created | Initial socket state | `socket()` |
| Bound | Address assigned | `bind()` |
| Listening | Ready for connections | `listen()` |
| Connected | Active connection | `connect()`/`accept()` |
| Closed | Socket termin
## Why Choose TCP Instead of UDP?
IRC (Internet Relay Chat) requires reliable communication. Let's break down the differences between TCP and UDP, and why TCP is the right choice.

## 🔹 TCP (Transmission Control Protocol)
* ✅ Reliable: Ensures messages arrive in the correct order and without loss.
* ✅ Connection-Oriented: Establishes a connection before exchanging data.
* ✅ Error-Checked: Retransmits lost packets.
* ✅ Stream-Based: Data is sent as a continuous stream.

📌 Best for: Chat applications, HTTP, FTP, and anything requiring ordered and complete data transmission.

## 🔸 UDP (User Datagram Protocol)
* ❌ Unreliable: No guarantee that data will arrive.
* ❌ Connectionless: No session tracking, just sends packets without checking if they arrive.
* ❌ Out-of-Order Data: Packets may arrive in any order.
* ✅ Low Latency: Faster than TCP because it skips reliability checks.

📌 Best for: Gaming, VoIP, streaming, and real-time applications where occasional data loss is acceptable.

## 📌 Why TCP for IRC?
* IRC requires messages to be received in order and without loss. If messages arrived out of order (or were lost), chat conversations would be chaotic. TCP guarantees:

* Reliable delivery of messages.
* Ordered message sequences (e.g., ensuring JOIN happens before PRIVMSG).
* Error handling if packets are lost.

## 🔹 Step 2: Set Socket Options (setsockopt())
In this step, we'll configure the socket to allow reuse of the address/port. This is important because when you restart your server or it crashes, you want to avoid the error "Address already in use" when binding the socket.

## 📖 What Happens Here?
* We use setsockopt() to configure socket options.
Specifically, we set the SO_REUSEADDR option to 1. This allows the socket to reuse an address/port that is in a TIME_WAIT state (from previous server runs).
* If setsockopt() fails, we'll output an error message and exit.
📝 What to Add in setupSocket()
📌 Write this after the socket() creation (in Step 2):

## // Step 2: Set socket options
int opt = 1;
if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    std::cerr << "Error: Failed to set socket options." << std::endl;
    exit(EXIT_FAILURE);
}
SOL_SOCKET: The level at which the option is defined.
SO_REUSEADDR: The option to allow address/port reuse.
&opt: Pointer to the value to set for the option (in this case, 1 means "allow reuse").
sizeof(opt): The size of the value being passed.