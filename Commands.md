# IRC 42 Command Reference

## Mandatory Commands

### Authentication Sequence
`PASS` → `NICK` → `USER`

#### `PASS <password>`
- *First command after connection*
- Sets connection password
- Server rejects unauthenticated clients
- **Parameters:**
  1. `password`: Server authentication secret
- **Validation:**
  - Must be first command after connection
  - Must match server configuration
- **Error Codes:**
  - 464: Password incorrect
  - 462: Already registered
**Scenario:** Client connects and authenticates
```
Client: PASS secret123
Server: (If correct) Proceed to NICK/USER
       (If wrong) ERROR :Bad password
```
**Pseudocode:**
```python
def handle_PASS(client, password):
    if password == server_password:
        client.authenticated = True
    else:
        send_error(client, 'Bad password')
```

#### `NICK <nickname>`
- Sets/updates user nickname
- Nicknames must be unique
- **Parameters:**
  1. `nickname`: Public identifier (1-9 chars)
- **Rules:**
  - Valid chars: [a-zA-Z][\w-]+
  - Case-insensitive uniqueness
- **Error Codes:**
  - 433: Nickname in use
  - 432: Invalid nickname
**Scenario:** User sets initial nickname
```
Client: NICK john_doe
Server: :john_doe NICK :john_doe
```
**Pseudocode:**
```python
def handle_NICK(client, new_nick):
    if not is_valid_nick(new_nick):
        return ERR_ERRONEUSNICKNAME
    client.nickname = new_nick
    broadcast_nick_change(client)
```

#### `USER <username> 0 * <realname>`
- Registers client with server
- **Parameters:**
  1. `username`: Account identifier
  2. `0`: Legacy hostname (ignored)
  3. `*`: Servername placeholder
  4. `realname`: Display name
- **Validation:**
  - Must follow NICK command
**Scenario:** Complete registration
```
Client: USER jdoe 0 * John Doe
Server: :irc.server.com 001 jdoe :Welcome to IRC 42
```
**Pseudocode:**
```python
def handle_USER(client, username, hostname, servername, realname):
    # hostname/servername parameters ignored
    client.username = username
    client.realname = realname
    if client.authenticated:
        send_welcome(client)
```

### Channel Operations

#### `JOIN <channel>[ <password>]`
- Joins/Creates channel
- Optional password for +k mode channels
- **Parameters:**
  1. `channel`: #-prefixed name
  2. `password`: Required for +k channels
- **Modes Affected:**
  - +k: Password required
    - *Example:* `MODE #room +k secret` (sets password)
    - *Usage:* `JOIN #room secret` (joins with password)
  - +i: Invite required
    - *Example:* `MODE #private +i` (sets invite-only)
    - *Usage:* `INVITE user #private` (invites user)
- **Error Codes:**
  - 475: Bad channel key
  - 473: Invite-only
- Example: `JOIN #general`
**Scenario:** Joining a protected channel
```
Client: JOIN #private pass123
Server: :john_doe JOIN #private
```
**Pseudocode:**
```python
def handle_JOIN(client, channel, password=None):
    if channel.locked and password != channel.password:
        send_error(client, 'Bad channel key')
    else:
        channel.add_member(client)
```

#### `PRIVMSG <target> <message>`
- Target can be channel or user
- Message forwarding to all channel members
- **Parameters:**
  1. `target`: Channel or nickname
  2. `message`: Text to send
- **Delivery Rules:**
  - Channel: Broadcast to all members
  - User: Direct message
- **Error Codes:**
  - 401: No such nick/channel
  - 404: Cannot send to channel
- Example: `PRIVMSG #general Hello everyone!`
**Scenario:** Channel message broadcast
```
Client: PRIVMSG #general Hello everyone!
Server: Forward to all #general members
```
**Pseudocode:**
```python
def handle_PRIVMSG(sender, target, message):
    if target.startswith('#'):
        channel = get_channel(target)
        channel.broadcast(sender, message)
    else:
        user = get_user(target)
        user.send_message(message)
```

### Operator Commands

#### `MODE <channel> <flags> [<args>]`
- **Parameters:**
  1. `channel`: Target channel
  2. `flags`: Mode changes (+/-)
  3. `args`: Mode parameters
- **Mode Reference:**
| Flag | Argument    | Description               |
|------|-------------|---------------------------|
| +i   | None        | Invite-only channel       |
| +t   | None        | Topic operator-only       |
| +k   | Password    | Set channel password      |
| +o   | Nickname    | Grant operator status     |
| +l   | Number      | Set user limit            |
- **Validation:**
  - Requires operator status
**Scenario:** Setting channel limit
```
Operator: MODE #chat +l 50
Server: Mode [+l 50] set by operator
```
**Pseudocode:**
```python
def handle_MODE(client, channel, mode, args):
    if not client.is_operator:
        return error('Permission denied')
    
    if '+l' in mode:
        channel.user_limit = args[0]
```

#### `INVITE <nickname> <channel>`
- Invites user to invite-only channel
- **Parameters:**
  1. `nickname`: User to invite
  2. `channel`: Target channel
- **Requirements:**
  - Inviter must be channel operator
  - Channel must be +i mode
- **Error Codes:**
  - 482: Not channel operator
  - 443: User already in channel
**Scenario:** Inviting to invite-only channel
```
Operator: INVITE mary #secret
Mary: Received INVITE message
```
**Pseudocode:**
```python
def handle_INVITE(inviter, target_user, channel):
    if inviter in channel.operators:
        target_user.invites.add(channel)
        send_invite_notice(target_user, channel)
```

#### `TOPIC <channel> [<topic>]`
- Views/channel topic
- **Parameters:**
  1. `channel`: Target channel
  2. `topic`: New topic (optional)
- **Mode Dependency:**
  - +t mode requires operator status
- **Error Codes:**
  - 482: Insufficient privileges
  - 331: No topic set
**Scenario:** Changing protected topic
```
Operator: TOPIC #staff New meeting schedule
Channel: Topic updated
```
**Pseudocode:**
```python
def handle_TOPIC(client, channel, new_topic=None):
    if new_topic:
        if channel.topic_protected and not client.is_operator:
            return error('Permission denied')
        channel.topic = new_topic
```

#### `KICK <channel> <user> [<reason>]`
- Removes user from channel
- **Parameters:**
  1. `channel`: Target channel
  2. `user`: Nickname to remove
  3. `reason`: Optional message
- **Validation:**
  - Requester must be operator
  - Target must be in channel
- **Error Codes:**
  - 482: Not operator
  - 441: User not in channel
- Example: `KICK #general spammer "No spamming"`
**Scenario:** Removing disruptive user
```
Operator: KICK #lounge bob 'Spamming'
Bob: Removed from channel
```
**Pseudocode:**
```python
def handle_KICK(operator, channel, target, reason):
    if operator not in channel.operators:
        return error('Not channel operator')
    channel.remove_user(target)
    log_action(f"{target} kicked by {operator}: {reason}")
```

## Bonus Features

### File Transfer
`SENDFILE <user> <filename>`
- **Parameters:**
  1. `user`: Recipient nickname
  2. `filename`: File to transfer
- **Protocol:**
  - Uses DCC SEND protocol
  - Requires port negotiation
- **Reference**
  - DCC: [https://en.wikipedia.org/wiki/Direct_Client-to-Client](https://en.wikipedia.org/wiki/Direct_Client-to-Client)
**Scenario:** Secure file transfer
```
Client: SENDFILE mary report.pdf
Server: Initiate DCC SEND protocol
```

### Chat Bot
`BOT <command>`
- **Parameters:**
  1. `command`: Bot instruction
- **Supported Commands:**
  - `weather <city>`: Get weather
  - `translate <text> <lang>`: Translation
  - `calc <expression>`: Math evaluation
- **Reference**
  - API: [https://openweathermap.org/api](https://openweathermap.org/api)
**Scenario:** Weather lookup
```
Client: BOT weather Paris
Bot: Current Paris weather: 22°C, sunny
```

## Implementation Notes
- All commands support partial input aggregation
- Server uses single poll() for non-blocking I/O
- Message format: RFC 1459 compliant

## References

- RFC 1459: [https://datatracker.ietf.org/doc/html/rfc1459](https://datatracker.ietf.org/doc/html/rfc1459)
