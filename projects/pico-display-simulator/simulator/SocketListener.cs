using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Linq;

namespace Simulator
{
    public class SocketListener
    {
        // Port number to listen on
        public const int m_portNumber = 5010;

        // Socket Handle
        public Socket m_socketHandle = null;

        // Commands
        List<ICommand> m_commandList;

        // Framer
        Framer m_framer;

        public int m_dd;
        public int m_hh;
        public int m_mm;
        public int m_sec;
        public int m_msec;


        // Constructor
        public SocketListener()
        {
            m_commandList = new List<ICommand>();
            m_framer = new Framer();
        }

        // Add a command
        public void RegisterCommand(ICommand commandToRegister)
        {
            m_commandList.Add(commandToRegister);
        }

        // Returns the socket handle
        public Socket GetSocketHandle() { return m_socketHandle; }

        // Command Format (without leading/trailing SOF/EOF framing characters
        // <time> <cmd> [<args>...]
        // Where:
        //      <time>   is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
        //      <cmd>    command verb
        //      <args>   zero or more command arguments
        //
        // NOTES:
        //  1) individual arguments are separated by spaces
        //
        private bool ProcessCommands(string data)
        {
            bool continueRunning = true;
            string frame = m_framer.Deframe(data);
            Console.WriteLine("ENTER: Frame=[{0}]", frame);
            while (frame != null && continueRunning)
            {
                char[] separators = { ' ' };
                List<string> tokens  = frame.Split(separators, StringSplitOptions.RemoveEmptyEntries ).Select(s => s.Trim()).ToList();
                if (tokens.Count() >= 3)
                {
                    string verb = tokens[2];
                    foreach (ICommand cmd in m_commandList)
                    {
                        if (verb == cmd.GetCommandName())
                        {
                            ParseTimeStamp(tokens);
                            continueRunning = cmd.ExecuteCommand(frame, tokens);
                        }
                    }
                }

                // Get the next frame if there is one
                frame = m_framer.Deframe();
                Console.WriteLine("NEXT: Frame=[{0}]", frame);
            }


            // Return true to get the next frame
            return continueRunning;
        }

        private void ParseTimeStamp(List<string> tokens)
        {
            // Format is: "DD HH:MM:SS.sss"
            m_dd   = int.Parse(tokens[0]);
            m_hh   =  int.Parse(tokens[1].Substring(0, 2));
            m_mm   = int.Parse(tokens[1].Substring(3, 2));
            m_sec  = int.Parse(tokens[1].Substring(6, 2));
            m_msec = int.Parse(tokens[1].Substring(9, 3));
            Console.WriteLine("time: {0:00} {1:00}:{2:00}:{3:00}.{4:000}", m_dd, m_hh, m_mm, m_sec, m_msec);
        }
        public static void Start(object info)
        {
            SocketListener listenerInstance = (SocketListener)info;

            // Data buffer for incoming data.  
            byte[] bytes = new Byte[1024];

            // Establish the local endpoint for the socket.  
            IPAddress ipAddress = new IPAddress(new byte[] { 0, 0, 0, 0 });
            IPEndPoint localEndPoint = new IPEndPoint(ipAddress, m_portNumber);

            // Create a TCP/IP socket.  
            Socket listener = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            // Bind the socket to the local endpoint and   
            // listen for incoming connections.  
            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(10);

                Console.WriteLine("Listening on port {0} ...", m_portNumber);
                // Program is suspended while waiting for an incoming connection.  
                listenerInstance.m_socketHandle = listener.Accept();

                // An incoming connection needs to be processed.  
                int notBlockingCounter = 0;
                while (true)
                {
                    bytes = new byte[1024];
                    int bytesRec = listenerInstance.m_socketHandle.Receive(bytes);
                    if (bytesRec > 0)
                    {
                        notBlockingCounter = 0;
                        string data = Encoding.ASCII.GetString(bytes, 0, bytesRec);
                Console.WriteLine("Data:[{0}]", data);
                        if (listenerInstance.ProcessCommands(data) == false)
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (++notBlockingCounter > 20)
                        {
                            break;
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            finally
            {
                // Shutdown the connection
                Console.WriteLine("Terminating Connection...");
                listenerInstance.Stop();

                // Shutdown the app
                System.Windows.Forms.Application.Exit();
            }
        }

        public void Stop()
        {
            // Shutdown the connection
            try
            {
                Console.WriteLine("Terminating Connection...");
                if (m_socketHandle != null)
                {
                    m_socketHandle.Shutdown(SocketShutdown.Both);
                    m_socketHandle.Close();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

    }
}

