# TCP server

- TCP
- TCP Server
- IPv4
- IPv6
- .NET
- C#
- Visual Studio 2022
- Windows
- Linux

---

Can be tested with [TCP Client](../TcpHexClient/), see additional information there.

Generic TCP server class `Server` is exported from TcpServer class library. 

Events:

- `ClientConnection` - called when client is connected/disconnected.
- `DataReceived` - called when data is received from connected client.

Methods:

- `Start`
- `Stop`
- `Send` - sends data to specific client.
- `Broadcast` - sends data to all connected clients.

Using: Create the Server instance, subscribe to events, call `Start`. Implement your own functionality by analyzing an input packets in `DataReceived` event, and using `Send`/`Broadcast` methods.

EchoServer is demo Console application project, which uses the `Server` class, implementing a basic Echo server. It just sends back to client any received data.

---
Not for production code. Don't expect exceptional performance or 100% stability.

When to use? During development process:
- Testing various TCP clients, including embedded hardware devices. 
- Writing hardware server emulators. 
- Writing stress tests for different TCP clients.

---
## Running TCP server in Linux

Microsoft .NET program may run of different OS. Let's try to run EchoServer on Ununtu 22.04 x64.

Open TcpServer solution in Visual Studio 2022, right-click on EchoServer project - Publish. Select "Publish to directory" and continue. Finally, it should create `publish` directory in `bin\Release\net8.0`. To run the server, we can execute `EchoServer.exe` or `dotnet EchoServer.dll`:

```
C:\tmp\TcpServer\EchoServer\bin\Release\net8.0\publish>dir
 
 4-Aug-24  03:06 PM               704 EchoServer.deps.json
14-Aug-24  03:06 PM             7,168 EchoServer.dll
14-Aug-24  03:06 PM           142,848 EchoServer.exe
14-Aug-24  03:06 PM            11,348 EchoServer.pdb
14-Aug-24  03:04 PM               340 EchoServer.runtimeconfig.json
14-Aug-24  03:04 PM            15,872 TcpServer.dll
14-Aug-24  03:04 PM            13,848 TcpServer.pdb

C:\tmp\TcpServer\EchoServer\bin\Release\net8.0\publish>EchoServer.exe
EchoServer port 41000, Address family InterNetwork
Server: Socket is created
Server: Socket is bound
Echo server is running. Press any other key to stop
Server: Socket is listening
Server: EndAccept: SocketException The I/O operation has been aborted because of either a thread exit or an application request.
Server: Listening socket is closed
Server: ServerTask ended

C:\tmp\TcpServer\EchoServer\bin\Release\net8.0\publish>dotnet EchoServer.dll
EchoServer port 41000, Address family InterNetwork
Server: Socket is created
Server: Socket is bound
Echo server is running. Press any other key to stop
Server: Socket is listening
Server: EndAccept: SocketException The I/O operation has been aborted because of either a thread exit or an application request.
Server: Listening socket is closed
Server: ServerTask ended
```

Copy `publish` directory to Ubuntu. Now we need to install .NET 8.0 SDK or runtime, as described here: 

[Install .NET SDK or .NET Runtime on Ubuntu](https://learn.microsoft.com/en-us/dotnet/core/install/linux-ubuntu-install?tabs=dotnet8&pivots=os-linux-ubuntu-2204)


```
alex@alex-22:~$ sudo apt-get install -y dotnet-runtime-8.0
[sudo] password for alex: 
...
Processing triggers for libc-bin (2.35-0ubuntu3.8) ...

alex@alex-22:~$ dotnet --list-runtimes
Microsoft.NETCore.App 8.0.8 [/usr/lib/dotnet/shared/Microsoft.NETCore.App]
```

Finally, run the EchoServer:

```
alex@alex-22:~/tmp/publish$ ls
EchoServer.deps.json  EchoServer.exe  EchoServer.runtimeconfig.json  TcpServer.pdb
EchoServer.dll        EchoServer.pdb  TcpServer.dll

alex@alex-22:~/tmp/publish$ dotnet EchoServer.dll
EchoServer port 41000, Address family InterNetwork
Server: Socket is created
Echo server is running. Press any other key to stop
Server: Socket is bound
Server: Socket is listening
Client 0 Connected
Server: ClientSocket 0. ReadCallback, EndReceive: bytesRead = 0, closing the socket
Client 0 disconnected
Server: EndAccept: SocketException Operation canceled
Server: ServerTask ended
Server: Listening socket is closed
```

