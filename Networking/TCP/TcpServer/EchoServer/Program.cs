using System.Net.Sockets;

/// Using TcpServer class.
/// Project contains typical server implementation: echo server.
/// Command line Arguments:
/// port family
///
/// port - valid TCP port, maximal value in unsigned short range
/// family - 4 or 6
///
/// Default values: 50000 4


namespace EchoServer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            int port = 41000;
            AddressFamily family = AddressFamily.InterNetwork;                // InterNetworkV6

            if (args.Length >= 1)
            {
                int n;

                if (int.TryParse(args[0], out n))
                {
                    port = n;
                }
            }

            if (args.Length >= 2)
            {
                if (args[1] == "6")
                {
                    family = AddressFamily.InterNetworkV6;
                }
            }

            new EchoServer().Run(port, family);
        }
    }
}
