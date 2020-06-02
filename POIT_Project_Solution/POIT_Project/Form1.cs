using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.Net.Sockets;

namespace POIT_Project
{
    public partial class Form1 : Form
    {
        bool disconnect = false;
        NetworkStream nStream;
        TcpClient tcpClient;
        public Form1()
        {
            InitializeComponent();
        }

        private async void button1_Click(object sender, EventArgs e)
        {
            tcpClient = new TcpClient();
            tcpClient.Connect(IPAddress.Parse("192.168.168.70"), 12000);
            ArduinoComm();  
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void Disconnect_Click(object sender, EventArgs e)
        {
            disconnect = true;
        }

        private async void ArduinoComm()
        {
            try
            {
                if (tcpClient.Connected)
                {
                    await Task.Delay(40);
                    nStream = tcpClient.GetStream();
                    byte[] inputBytes = new byte[1024];
                    int bytesRead = nStream.Read(inputBytes, 0, inputBytes.Length);
                    string rawIntensity = Encoding.ASCII.GetString(inputBytes, 0, bytesRead);
                    string[] intensity = rawIntensity.Split(',');
                    int newIntensity = int.Parse(intensity[0]) / (1024 / 100);
                    string output = "#" + newIntensity.ToString();
                    byte[] outputBytes = Encoding.ASCII.GetBytes(output);
                    nStream.Write(outputBytes, 0, outputBytes.Length);
                    textBox1.Text = intensity[0];
                    Output.Text = newIntensity.ToString();
                    Console.WriteLine(rawIntensity);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            if (disconnect)
            {
                nStream.Close();
                disconnect = false;
                return;
            }

            ArduinoComm();
        }
    }
}
