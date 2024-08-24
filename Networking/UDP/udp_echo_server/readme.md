# UDP echo server

- UDP
- UDP server
- IPv4
- IPv6
- C++
- Linux
- Embedded Linux
- Raspberry Pi

## udp_echo_server program

This program works like [C++ UDP echo server](../sv_client/udp_server), with some differences:

- It is written in C.
- Program is not cross-platform, runs only on Linux (including Embedded).
- Contains only minimal necessary code.
- Critical errors are reported and `exit` is called (no clean exit).
- Finished with endless loop waiting for incoming messages, to stop it in the terminal window `Ctrl+C` is used.
- This directory doesn't contain any IDE project or makefile, we will build the program from command line.
- Otherwise, it works exactly like C++ UDP echo server, see [documentation](../sv_client/readme.md).

## Testing on Desktop Linux computer

Since the program doesn't contain any embedded or board-specific code, we can test it first on PC computer. In my case, this is Ubuntu 22.04 x64.
We assume that `samples` repository is cloned to `~/tmp` directory.

Let's build and run the program:

```
alex@u23:~/tmp/samples/Networking/UDP/udp_echo_server$ gcc udp_echo_server.c -o udp_echo_server
alex@u23:~/tmp/samples/Networking/UDP/udp_echo_server$ ./udp_echo_server
UDP echo server started
port 41000 IPv4
socket is created
bound to port 41000
```

Now open [UDP client](../sv_client/udp_client) project in Qt Creator and build it in Release configuration. Start `udp_client`:

```
alex@u23:~/tmp/samples/Networking/UDP/sv_client/udp_client/Release$ ./udp_client
```

![UDP client](../../../images/udp_cl_linux_local_4.png)

Press `Ctrt+C` to stop `udp_echo_server` and run it again in IPv6:

```
alex@u23:~/tmp/samples/Networking/UDP/udp_echo_server$ ./udp_echo_server 41000 6
UDP echo server started
port 41000 IPv6
socket is created
bound to port 41000
```

Run `udp_client`:

![UDP client](../../../images/udp_cl_linux_local_6.png)


## Prepare Raspberry Pi board for testing

This description is board-specific: I am using Raspberry Pi 4 Model B. Generally, any Embedded Linux board with Ethernet connector is OK for this test.

We need SD card with operating system, as described in the [Raspberry Pi](https://www.raspberrypi.com/documentation/computers/getting-started.html) article. By default, user name is `pi`, password `raspberry`.

Before running the board, we need to set static IP address. Connect SD card to Linux PC. Open file `/media/user/rootfs/etc/dhcpcd.conf` in text editor. Doesn't work? Try to do this with `sudo`.

Add the following lines to the end of `dhcpcd.conf`:

```
interface eth0
static ip_address=192.168.3.14/24
static ip6_address=fc00::2/64
static routers=192.168.3.1
static domain_name_servers=192.168.3.1 8.8.8.8 fd51:42f8:caae:d92e::1
```
Save file, remove SD card from PC and attach it to the board.

Set PC static IPv4 address `192.168.3.1/24` and IPv6 address `fc00::1/64". Connect PC and board with an Ethernet cable, power up the board.

```
alex@u23:~$ ping 192.168.3.14
PING 192.168.3.14 (192.168.3.14) 56(84) bytes of data.
64 bytes from 192.168.3.14: icmp_seq=1 ttl=64 time=0.348 ms
64 bytes from 192.168.3.14: icmp_seq=2 ttl=64 time=0.212 ms
PING fc00::2(fc00::2) 56 data bytes
64 bytes from fc00::2: icmp_seq=1 ttl=64 time=0.496 ms
64 bytes from fc00::2: icmp_seq=2 ttl=64 time=0.217 ms
```

## Testing on Raspberry Pi

We need to copy file `udp_echo_server.c` to the board. Then we login using SSH, complile and run the program:

```
alex@u23:~/tmp/samples/Networking/UDP/udp_echo_server$ scp udp_echo_server.c pi@192.168.3.14:/home/pi
pi@192.168.3.14's password: 
udp_echo_server.c                                                                             100% 4132     3.6MB/s   00:00    
alex@u23:~/tmp/samples/Networking/UDP/udp_echo_server$ ssh pi@192.168.3.14
pi@192.168.3.14's password: 
Linux raspberrypi 5.10.63-v7l+ #1459 SMP Wed Oct 6 16:41:57 BST 2021 armv7l

The programs included with the Debian GNU/Linux system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Debian GNU/Linux comes with ABSOLUTELY NO WARRANTY, to the extent
permitted by applicable law.
Last login: Sat Dec  4 19:17:12 2021
pi@raspberrypi:~ $ ls
Bookshelf  Documents  label.txt  Music     Public     test               Videos
Desktop    Downloads  Pictures  Templates  udp_echo_server.c
pi@raspberrypi:~ $ gcc udp_echo_server.c -o udp_echo_server
pi@raspberrypi:~ $ file udp_echo_server
udp_echo_server: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-armhf.so.3, BuildID[sha1]=b1e788df605d1b5318621e9783f88aa1be35785b, for GNU/Linux 3.2.0, not stripped
pi@raspberrypi:~ $ ./udp_echo_server
UDP echo server started
port 41000 IPv4
socket is created
bound to port 41000

```

Run `udp_client`:

![UDP client](../../../images/udp_cl_linux_rpi_4.png)

Stop `udp_echo_server` by pressing `Ctrl+C`and run it again as IPv6:

```
pi@raspberrypi:~ $ ./udp_echo_server
UDP echo server started
port 41000 IPv4
socket is created
bound to port 41000
^C
pi@raspberrypi:~ $ ./udp_echo_server 41000 6
UDP echo server started
port 41000 IPv6
socket is created
bound to port 41000
```

Run `udp_client`:

![UDP client](../../../images/udp_cl_linux_rpi_6.png)

