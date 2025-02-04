# ft_irc Project Overview

## Project Description
The ft_irc project involves building an Internet Relay Chat (IRC) server in C++98. The server must handle multiple clients using non-blocking I/O operations through `poll()` or equivalent systems. Key functionalities include authentication, channel management, and user interaction handling.

## Key Features
* **TCP/IP Server**: Accept client connections on a specified port
* **Authentication**: Process user nickname, username, and password
* **Channel Management**: Support channel creation, joining, and operator actions (kick, invite, mode settings)
* **User Roles**: Implement operator (@) and regular user distinctions
* **Command Handling**: Process standard IRC commands (JOIN, PRIVMSG, KICK, TOPIC, etc.)
* **Non-Blocking I/O**: Utilize poll() for efficient multiple client handling

## Development Plan and Task Breakdown

### Phase 1: Setup & Foundations (2-3 Days)

#### Project Setup (1 Day)
* Initialize GitHub repository
* Create Makefile with -Wall -Wextra -Werror flags
* Implement basic TCP socket setup (bind, listen, accept)

#### Basic Connection Handling (1-2 Days)
* Accept multiple client connections using poll()
* Implement basic client authentication (password handling)

**Task Distribution:**
* Person A: Makefile, GitHub setup, server initialization
* Person B: Client connection handling with poll()

### Phase 2: Client-Server Communication (4-5 Days)

#### IRC Message Parsing (2 Days)
* Parse user input and extract commands (JOIN, PRIVMSG, etc.)
* Implement message buffering for partial packet handling

#### User & Channel Management (2-3 Days)
* Implement nickname setting and channel joining functionality
* Create channel objects with member and operator storage

**Task Distribution:**
* Person A: Message parsing and basic command handling
* Person B: User management system

### Phase 3: Core IRC Commands (5-6 Days)

#### Basic Commands (2-3 Days)
* NICK - Nickname setting
* USER - User details configuration
* JOIN - Channel creation/joining
* PRIVMSG - User/channel message sending

#### Operator Commands (3 Days)
* KICK - Channel user removal
* INVITE - Private channel invitation
* TOPIC - Channel topic management
* MODE - Channel settings modification (i, t, k, o, l)

**Task Distribution:**
* Person A: NICK, USER, PRIVMSG, JOIN implementation
* Person B: KICK, INVITE, TOPIC, MODE implementation

### Phase 4: Stability & Testing (4-5 Days)

#### Error Handling & Edge Cases (2 Days)
* Handle invalid commands and malformed input
* Implement nickname conflict resolution
* Ensure proper client disconnection handling

#### Testing & Debugging (2-3 Days)
* Test with IRC clients (HexChat, WeeChat)
* Conduct multiple client testing
* Test slow connections and packet fragmentation

**Task Distribution:**
* Person A: Error and edge case handling
* Person B: Comprehensive testing with IRC clients

### Bonus Features (Optional)
* File transfer support
* IRC Bot implementation (automated messages, moderation)

## Project Timeline (2-Person Team)

| Phase | Task | Duration | Person A | Person B |
|-------|------|----------|----------|----------|
| 1 | Setup & Connection | 2-3 Days | Setup & Server | Client Connection |
| 2 | Communication | 4-5 Days | Message Parser | User Management |
| 3 | IRC Commands | 5-6 Days | Basic Commands | Operator Commands |
| 4 | Testing | 4-5 Days | Error Handling | System Testing |
| Bonus | Advanced Features | Optional | File Transfer | IRC Bot |

## Development Guidelines
* Use GitHub Issues for task tracking
* Begin testing early with IRC clients
* Maintain C++98 compliance (avoid std::string, use cstring)
* Document code thoroughly
* Implement robust error handling
* Follow consistent coding style

## Testing Strategy
* Unit testing for command parsing
* Integration testing with IRC clients
* Load testing with multiple simultaneous connections
* Edge case testing for error handling
* Network condition simulation testing

## Final Checklist
- [ ] All core features implemented
- [ ] C++98 compliance verified
- [ ] Memory leaks checked
- [ ] Error handling tested
- [ ] Documentation completed
- [ ] Successfully tested with IRC clients
- [ ] Code review completed