1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

There are several techniques to handle this:
- Length prefixing - Send message length before actual data
- Delimiter-based framing - Use special character sequences to mark message boundaries
- Fixed-size messages - Pad messages to predetermined size
- Protocol buffers - Use structured message formats with built-in length information
- Chunked transfer - Break large messages into smaller chunks with length indicators

Common approaches include:
- Adding EOF markers at the end of messages
- Implementing timeout mechanisms
- Using buffered reads with size checks
- Maintaining state to track message completeness
- Implementing acknowledgment protocols
---
2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol over TCP requires careful handling of command boundaries. The protocol can implement message framing techniques like length prefixes, delimiter characters (such as newlines), or explicit start/end markers. The protocol design should include clear message formats, command IDs or sequence numbers, and consistent encoding schemes for reliable transmission. Without proper boundary handling, several issues can arise: command fragments might be interpreted as complete commands, multiple commands could be processed as one, buffer overflows may occur from incomplete size information, deadlocks can happen while waiting for more data, messages may become corrupted due to boundary confusion, and inefficient parsing can lead to performance problems.

---
3. Describe the general differences between stateful and stateless protocols.

Key differences between stateful and stateless protocols include:

Stateful Protocols:
- Maintain session information between requests
- Remember previous interactions
- Require server-side storage
- Higher overhead but better for complex transactions
- Examples: FTP, TCP

Stateless Protocols:
- Each request is independent
- No session tracking
- Lower server overhead
- Simpler to implement and scale
- Examples: HTTP, UDP

Trade-offs:
- Stateful protocols provide better consistency
- Stateless protocols offer better scalability
- Stateful requires connection management
- Stateless may need to resend context data

---
4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is preferred in scenarios where speed and low latency are more critical than guaranteed delivery. It's commonly used in real-time applications like video streaming, online gaming, and VoIP calls where dropping occasional packets is preferable to waiting for retransmissions. UDP's lightweight nature (no handshaking, connection management, or delivery confirmation) means less overhead and faster transmission compared to TCP. Additionally, applications that handle their own reliability mechanisms or those that send frequent updates where newer data supersedes older data (like game state updates) benefit from UDP's simplicity and efficiency.

---
5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Operating systems provide network communications through sockets, which serve as the primary abstraction for network programming. Sockets act as endpoints for communication between processes, offering both stream-oriented (TCP) and datagram-oriented (UDP) protocols. The socket API provides standard functions like socket(), bind(), listen(), accept(), connect(), send(), and recv() that applications can use to establish and manage network connections. This abstraction layer hides the complexity of low-level network protocols and hardware interactions, allowing developers to focus on application logic while the OS handles the details of packet transmission, routing, and network interface management.
