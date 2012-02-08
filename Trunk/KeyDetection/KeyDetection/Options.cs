using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace KeyDetection
{
    public partial class Options : Form
    {
        public Options()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                Globals.PPI_RATIO = float.Parse(textBox1.Text);
                this.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Invalid content...");
            }
        }
    }
}
