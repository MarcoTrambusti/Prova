using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class input : Form
    {
        public List<int> arr= new List<int>();
        public input()
        {
            InitializeComponent();
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            arr.Add((int)numericUpDown1.Value);
        }

        private void btnMax_Click(object sender, EventArgs e)
        {
            txtMax.Text= arr.Max().ToString();
        }
    }
}
