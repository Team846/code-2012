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

            textBox1.Text = Globals.PPI_RATIO.ToString();
            textBox2.Text = Globals.EDGE_THRESHOLD.ToString();

            trackbar_Sat.Value = (int)(Globals.SAT_THRESHOLD * 100f);
            label_SatAmount.Text = ((float)trackbar_Sat.Value / 100f).ToString();

            filterType.SelectedIndex = Globals.FILTER_BLACKWHITE ? 0 : 1;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                Globals.PPI_RATIO = float.Parse(textBox1.Text);
                Globals.EDGE_THRESHOLD = float.Parse(textBox2.Text);
                Globals.SAT_THRESHOLD = (float)trackbar_Sat.Value / 100f;

                Globals.FILTER_BLACKWHITE = filterType.SelectedIndex == 0;

                this.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Invalid content...");
            }
        }

        private void Options_Load(object sender, EventArgs e)
        {

        }

        private void trackbar_Sat_ValueChanged(object sender, EventArgs e)
        {
            label_SatAmount.Text = ((float)trackbar_Sat.Value / 100f).ToString();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
