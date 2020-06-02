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
using rtChart;
using Microsoft.Office.Interop.Excel;
using _Excel = Microsoft.Office.Interop.Excel;

namespace POIT_Project
{
    public partial class Form1 : Form
    {
        List<int> intensityList = new List<int>();

        bool disconnect = false;
        int newIntensity;
        float outputAnglef = 90f;
        string outputAngle = "#90", desiredIntensity, path = "data.xlsx";

        _Application excel = new _Excel.Application();
        Workbook wb;
        Worksheet ws;
        kayChart intensityChart;
        NetworkStream nStream;
        TcpClient tcpClient;
        public Form1()
        {
            InitializeComponent();
        }

        private async void button1_Click(object sender, EventArgs e)
        {
            tcpClient = new TcpClient();
            tcpClient.Connect(IPAddress.Parse(IPTextBox.Text), int.Parse(PortTextBox.Text));         
            ArduinoComm();  
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            intensityChart = new kayChart(chart1, 100);
            intensityChart.serieName = "Intenzita";

            wb = excel.Workbooks.Add(XlWBATemplate.xlWBATWorksheet);
            ws = wb.Worksheets[1];

            ws.Cells[1, 1].Value2 = "ID";
            ws.Cells[1, 2].Value2 = "Intenzita";
        }

        private void Disconnect_Click(object sender, EventArgs e)
        {
            disconnect = true;
        }

        //handles communication with arduino
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
                    //recalculate intensity within 0-100 range
                    newIntensity = int.Parse(intensity[0]) / (1024 / 100);
                    intensityList.Add(newIntensity);
                    outputAngle = "#" + outputAnglef;
                    //send value to arduino
                    byte[] outputBytes = Encoding.ASCII.GetBytes(outputAngle);
                    nStream.Write(outputBytes, 0, outputBytes.Length);

                    aGauge1.Value = newIntensity;
                    intensityChart.TriggeredUpdate((double)newIntensity);
                    dataOutput.AppendText("Received: " + newIntensity + "\n");
                    //send values to excel
                    ws.Cells[intensityList.Count + 1, 1].Value2 = intensityList.Count;
                    ws.Cells[intensityList.Count + 1, 2].Value2 = intensityList[intensityList.Count - 1];
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            if (disconnect)
            {
                nStream.Close();
                aGauge1.Value = 0;
                disconnect = false;
                return;
            }

            ArduinoComm();
        }

        private double getIntensity()
        {
            return (double)newIntensity;
        }

        private void dataOutput_TextChanged(object sender, EventArgs e)
        {
            dataOutput.SelectionStart = dataOutput.Text.Length;
            dataOutput.ScrollToCaret();
        }

        //send data button handler
        private void sendButton_Click(object sender, EventArgs e)
        {
            desiredIntensity = sendText.Text;
            CalculateAngle();
        }

        //quit application button handler
        private void QuitButton_Click(object sender, EventArgs e)
        {
            nStream.Close();
            this.Close();
        }

        //save to file button handler
        private void saveButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog SaveFileDialog1 = new SaveFileDialog();

            SaveFileDialog1.Filter = "Excel 2010|.xlsx|Excel|.xls";

            if (SaveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                path = SaveFileDialog1.FileName;
                wb.SaveAs(path);
                wb.Close();

            }
        }

        //calculate, where to move servo in order to achieve desired intesity
        private async void CalculateAngle()
        {
            await Task.Delay(100);
            if (int.Parse(desiredIntensity) < newIntensity)
            {
                if (outputAnglef > 0)
                    outputAnglef -= 0.5f;
            }
            else
            {
                if (outputAnglef < 180)
                    outputAnglef += 0.5f;
            }

            CalculateAngle();
        }
    }
}
