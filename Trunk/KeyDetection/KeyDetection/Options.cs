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
            textBox1.Text = Convert.ToString(Globals.PPI_RATIO);
            textBox2.Text = Convert.ToString(Globals.satThreshold * 100);
            textBox3.Text = Convert.ToString(Globals.edgeThreshold);
            checkBox1.Checked = Globals.threshold;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                Globals.PPI_RATIO = float.Parse(textBox1.Text);
                Globals.satThreshold = float.Parse(textBox2.Text) / 100;
                Globals.edgeThreshold = float.Parse(textBox3.Text);
                Globals.threshold = checkBox1.Checked;
                textBox2.Enabled = checkBox1.Checked;
                this.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Invalid content...");
            }
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            textBox2.Enabled = checkBox1.Checked;
        }
    }
}
