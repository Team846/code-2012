using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace KeyTrackerConsole
{
    /// <summary>
    /// 
    /// </summary>
    class Program
    {
        static Socket socket;
        static byte[] buffer;
        static IPEndPoint iep;
        static EndPoint ep;

        static int lastpacketid = -1;
        static int missedpackets = 0;
        static int packets = 0;

        static void Main(string[] args)
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

            iep = new IPEndPoint(IPAddress.Any, 8000);
            ep = (EndPoint)iep;

            socket.Bind(iep);

            socket.BeginReceiveFrom(buffer = new byte[7], 0, buffer.Length, SocketFlags.None, ref ep, new AsyncCallback(onReceive), 0);

            while (true)
            {
                if (Console.KeyAvailable)
                {
                    Console.ReadKey();
                    break;
                }
            }

            Console.WriteLine("Packet Data Analysis: {0} packets received, {1} total bytes received, {2} total packets lost", packets, packets*7, missedpackets);

            Console.WriteLine("Press any key to continue...");

            Console.ReadKey();
        }

        static void onReceive(IAsyncResult ar)
        {
            int iReceived = socket.EndReceiveFrom(ar, ref ep);

            if (buffer[0] == 1)
            {
                int pid = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | (buffer[4] << 0);

                Console.WriteLine("{0}: R{1}, B{2}, Max:{3}", pid, buffer[5], buffer[6], Math.Max(buffer[5], buffer[6]));

                if (pid - 1 != lastpacketid && lastpacketid != -1)
                {
                    missedpackets += pid - lastpacketid - 1;
                }

                packets++;
                lastpacketid = pid;
            }

            socket.BeginReceiveFrom(buffer, 0, buffer.Length, SocketFlags.None, ref ep, new AsyncCallback(onReceive), null);
        }
    }
}
