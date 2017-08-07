using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Runtime.InteropServices;

namespace LostWinForms
{
    public partial class Form1 : Form
    {
        private bool bInitialized = false;

        public Form1()
        {
            InitializeComponent();

            panel1.Paint += Panel1_Paint;
            button1.Click += OnLoadFBX;
        }

        private void Panel1_Paint(object sender, PaintEventArgs e)
        {
            //throw new NotImplementedException();
            if (!bInitialized)
            {
                InitializeWindow(panel1.Handle, true, (uint)panel1.Width, (uint)panel1.Height);
                bInitialized = true;
            }

            Tick();
        }

        private void OnLoadFBX(Object sender, EventArgs e)
        {
            //LoadFBX()
            openFileDialog1.Title = "选择要打开的FBX文件";
            openFileDialog1.Filter = "FBX文件|*.fbx";
            openFileDialog1.RestoreDirectory = false;
            openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Multiselect = true;
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                LoadFBX(openFileDialog1.FileName, true);
            }
        }

        private void OnLogging(Int32 level, String msg)
        {

        }

        public delegate void PFN_Logger(Int32 level, StringBuilder msg);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetLogger(PFN_Logger logger);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void InitializeWindow(IntPtr handle, bool windowed, UInt32 width, UInt32 height);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadFBX(String file, bool clearScene);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Tick();

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }
    }
}
