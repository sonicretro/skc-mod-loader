using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace MusicConfigTool
{
	public partial class ListDialog : Form
	{
		IList<string> items;

		public ListDialog(IList<string> items)
		{
			InitializeComponent();
			this.items = items;
		}

		private void ListDialog_Load(object sender, EventArgs e)
		{
			listBox1.Items.AddRange(items.ToArray());
		}

		private void listBox1_DoubleClick(object sender, EventArgs e)
		{
			DialogResult = DialogResult.OK;
			Close();
		}

		private void listBox1_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				DialogResult = DialogResult.OK;
				Close();
			}
		}

		public string SelectedItem => (string)listBox1.SelectedItem;
	}
}
