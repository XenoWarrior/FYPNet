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

    public partial class Form1 : Form
    {
        private TcpClient clientSocket;
        private NetworkStream serverStream;

        private Dictionary<string, Dictionary<string, string>> RequestData = new Dictionary<string, Dictionary<string, string>>();
        private Dictionary<string, string> Packet = new Dictionary<string, string>();
        private Dictionary<string, string> Data = new Dictionary<string, string>();

        public Form1()
        {
            InitializeComponent();
        }

        private void btSend_Click(object sender, EventArgs e)
        {
            // Bundle up a request set
            var PacketInt = (int)FYPPacketType.FYPCP_ON_MESSAGE;
            Packet.Add("value", PacketInt.ToString());
            Packet.Add("type", "FYPPacketType");
            Data.Add("value", tbName.Text + ": " + tbMessage.Text);
            Data.Add("type", "string");

            // Pack them into the request data
            RequestData.Add("packet", Packet);
            RequestData.Add("message", Data);

            byte[] outStream = System.Text.Encoding.ASCII.GetBytes(JsonConvert.SerializeObject(RequestData));
            serverStream.Write(outStream, 0, outStream.Length);
            serverStream.Flush();

            ClearPacketBuffer();
        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            lbMessages.Items.Add("Conecting to server [" + tbIP.Text + ":" + tbPort.Text + "]...");

            int port;
            Int32.TryParse(tbPort.Text, out port);

            clientSocket = new TcpClient(tbIP.Text, port);
            serverStream = clientSocket.GetStream();

            // Bundle up a request set
            var PacketInt = (int)FYPPacketType.FYP_ON_CONNECT;
            Packet.Add("value", PacketInt.ToString());
            Packet.Add("type", "FYPPacketType");
            Data.Add("value", "Hello, world!");
            Data.Add("type", "string");

            // Pack them into the request data
            RequestData.Add("packet", Packet);
            RequestData.Add("message", Data);

            byte[] outStream = System.Text.Encoding.ASCII.GetBytes(JsonConvert.SerializeObject(RequestData));
            serverStream.Write(outStream, 0, outStream.Length);
            serverStream.Flush();

            ClearPacketBuffer();

            Thread ctThread = new Thread(GetMessages);
            ctThread.Start();
        }

        private void ClearPacketBuffer()
        {
            Packet.Clear();
            Data.Clear();
            RequestData.Clear();
        }

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
