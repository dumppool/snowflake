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
        private String[] LevelString = { "Info: ", "Warning: ", "Error: " };
        private bool bInitialized = false;

        public Form1()
        {
            InitializeComponent();

            button1.Click += OnLoadFBX;
            button2.Click += OnInit;
        }

        private void OnInit(Object sender, EventArgs e)
        {
            if (!bInitialized)
            {
                InitializeWindow(panel1.Handle, true, (uint)panel1.Width, (uint)panel1.Height);
                bInitialized = true;
            }
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

        private void LoggingActually(Int32 level, StringBuilder msg)
        {
            textBox1.Text += "\r\n" + LevelString[level] + msg;
        }

        private void OnLogging(Int32 level, StringBuilder msg)
        {
            textBox1.Invoke(new PFN_Logger(LoggingActually), level, msg);
        }

        public delegate void PFN_Logger(Int32 level, StringBuilder msg);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetLogger(PFN_Logger logger);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void InitializeWindow(IntPtr handle, bool windowed, UInt32 width, UInt32 height);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadFBX(String file, bool clearScene);

        private void Form1_Load(object sender, EventArgs e)
        {
            if (!bInitialized)
            {
                InitializeWindow(panel1.Handle, true, (uint)panel1.Width, (uint)panel1.Height);
                SetLogger(OnLogging);
                bInitialized = true;
            }

            //textBox1.Text
            textBox1.Text += "\r\nForm1_Load";
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            textBox1.Text += "\r\nopenFileDialog1_FileOk";
        }
    }
}
