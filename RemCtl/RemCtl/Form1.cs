using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;



namespace RemCtl
{
    public partial class Form1 : Form
    {
        int ServPort = 63301;
        string ServIp;

        public Form1()
        {
            InitializeComponent();
        }

        private void DisconnectLabel_Click(object sender, EventArgs e)
        {

        }

        private void ServerLabel_Click(object sender, EventArgs e)
        {

        }

        private void ConnectLabel_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void ServBox_TextChanged(object sender, EventArgs e)
        {
            ServIp = ServBox.Text;
        }
    }
}
