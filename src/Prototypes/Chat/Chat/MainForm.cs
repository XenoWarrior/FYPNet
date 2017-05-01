using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Chat
{
    // Copied from FYPPacketType.h in FYPNet Core
    public enum FYPPacketType
    {
        // FYPNet Core
        FYP_ON_CONNECT,         // 0
        FYP_ON_MESSAGE,         // 1
        FYP_ON_INVALID_PACKET,  // 2
        FYP_ON_DISCONNECT,      // 3

        // GameServer Prototype
        FYPGP_ON_LOGIN,         // 4
        FYPGP_ON_CREATECHAR,    // 5
        FYPGB_ON_GETCHARS,      // 6

        // ChatServer Prototype
        FYPCP_ON_ACCEPT,        // 7
        FYPCP_ON_MESSAGE,       // 8
        FYPCP_ON_NEWUSER        // 9
    }

    public partial class MainForm : Form
    {
        private TcpClient clientSocket;
        private NetworkStream serverStream;

        private Dictionary<string, Dictionary<string, string>> RequestData = new Dictionary<string, Dictionary<string, string>>();

        private bool isConnected = false;

        public MainForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// When the send button is clicked, send a message.
        /// </summary>
        private void btSend_Click(object sender, EventArgs e)
        {
            if(Connected())
            {
                // Send FYPCP_ON_MESSAEG packet
                ClearBuffer();
                PacketType(FYPPacketType.FYPCP_ON_MESSAGE);
                AddValue("message", tbName.Text + ": " + tbMessage.Text);
                Dispatch();
            }
        }

        /// <summary>
        /// Returns if we're connected to the server
        /// </summary>
        /// <returns>bool: isConnected</returns>
        private bool Connected()
        {
            return isConnected;
        }

        /// <summary>
        /// When the connect button is clicked, connect to defined server ip and port.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btConnect_Click(object sender, EventArgs e)
        {
            lbMessages.Items.Add("Conecting to server [" + tbIP.Text + ":" + tbPort.Text + "]...");

            int port;
            if(!Int32.TryParse(tbPort.Text, out port))
            {
                lbMessages.Items.Add("Unable to parse port string to number: " + tbPort.Text + ". Using 12500");
                port = 12500;
            }

            try
            {
                // Setup connection
                clientSocket = new TcpClient(tbIP.Text, port);
                serverStream = clientSocket.GetStream();

                // Send FYP_ON_CONNECT packet
                ClearBuffer();
                PacketType(FYPPacketType.FYP_ON_CONNECT);
                AddValue("message", "Hello, world!");
                Dispatch();

                // Output success
                lbMessages.Items.Add("Connected successfully.");

                // Clear packet buffer
                ClearBuffer();

                // Start getting messages
                Thread ctThread = new Thread(GetMessages);
                ctThread.Start();

                isConnected = true;
            }
            catch(Exception ex)
            {
                // Error occured somewhere, show the user
                lbMessages.Items.Add("[Error] Exception: " + ex);
            }
        }

        /// <summary>
        /// Clears out all data from the dictionaries use to store packet data
        /// </summary>
        private void ClearBuffer()
        {
            RequestData.Clear();
        }

        /// <summary>
        /// Set the packet type for this buffer
        /// </summary>
        /// <param name="p">FYPPacketType: packet identifier</param>
        private void PacketType(FYPPacketType p)
        {
            Dictionary<string, string> Data = new Dictionary<string, string>();
            int packetInt = (int)p;

            Data.Add("value", packetInt.ToString());
            Data.Add("type", "FYPPacketType");

            RequestData.Add("packet", Data);
        }

        /// <summary>
        /// Adds a string value to the packet for sending
        /// </summary>
        /// <param name="k">string: key to add</param>
        /// <param name="v">string: value to add</param>
        private void AddValue(string k, string v)
        {
            Dictionary<string, string> Data = new Dictionary<string, string>();

            Data.Add("value", v);
            Data.Add("type", "string");

            RequestData.Add(k, Data);
        }

        /// <summary>
        /// Adds an int value to the packet for sending
        /// </summary>
        /// <param name="k">string: key to add</param>
        /// <param name="v">int: value to add</param>
        private void AddValue(string k, int v)
        {
            Dictionary<string, string> Data = new Dictionary<string, string>();

            Data.Add("value", v.ToString());
            Data.Add("type", "int");

            RequestData.Add(k, Data);
        }

        /// <summary>
        /// Adds a boolean value to the packet for sending
        /// </summary>
        /// <param name="k">string: key to add</param>
        /// <param name="v">bool: value to add</param>
        private void AddValue(string k, bool v)
        {
            Dictionary<string, string> Data = new Dictionary<string, string>();

            Data.Add("value", v.ToString());
            Data.Add("type", "int");

            RequestData.Add(k, Data);
        }

        /// <summary>
        /// Sends the buffer data to the server
        /// </summary>
        private void Dispatch()
        {
            try
            {
                byte[] outStream = System.Text.Encoding.ASCII.GetBytes(JsonConvert.SerializeObject(RequestData));
                serverStream.Write(outStream, 0, outStream.Length);
                serverStream.Flush();
            }
            catch(Exception ex)
            {
                MessageBox.Show("Unable to dispatch buffer, error:\n\n" + ex);
            }
        }

        /// <summary>
        /// Reads for any possible messages from the server
        /// </summary>
        private void GetMessages()
        {
            while(true)
            {
                try
                {
                    byte[] readBuffer = new byte[8196];
                    string fullReply = null;

                    if (serverStream.CanRead)
                    {
                        using (var writer = new MemoryStream())
                        {
                            while (serverStream.DataAvailable)
                            {
                                int numberOfBytesRead = serverStream.Read(readBuffer, 0, readBuffer.Length);
                                if (numberOfBytesRead <= 0)
                                {
                                    break;
                                }
                                writer.Write(readBuffer, 0, numberOfBytesRead);
                            }
                            fullReply = Encoding.UTF8.GetString(writer.ToArray());
                        }
                    }

                    if (fullReply != "")
                    {
                        this.Invoke((MethodInvoker)(() => lbMessages.Items.Add(fullReply)));
                        this.Invoke((MethodInvoker)(() => lbMessages.TopIndex = Math.Max(lbMessages.Items.Count - lbMessages.ClientSize.Height / lbMessages.ItemHeight + 1, 0)));
                    }
                }
                catch(Exception e)
                {

                }
            }
        }
    }
}
