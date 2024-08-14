# TCP server

- TCP
- TCP Server
- IPv4
- IPv6
- .NET
- C#
- Visual Studio 2022

---

Can be tested with [TCP Client](../TcpHexClient/), see additional information there.

Generic TCP server class `Server` is exported from TcpServer class library. 

Events:

- ClientConnection - called when client is connected/disconnected.
- DataReceived - called when data is received from connected client.

Methods:

- Start.
- Stop.
- Send - send data to specific client.
- Broadcast - send data to all connected clients.

Using: Create the Server instance, subscribe to events, call Start. Implement your own functionality by analyzing an input packets in DataReceived event, and using Send/Broadcast methods.

EchoServer is demo Console application project, which uses the Server class, implementing a basic Echo server. It just sends back to client any received data.

---
Not for production code. Don't expect exceptional performance or 100% stability.

When to use? During development process:
- Testing various TCP clients, including embedded hardware devices. 
- Writing hardware server emulators. 
- Writing stress tests for different TCP clients.


