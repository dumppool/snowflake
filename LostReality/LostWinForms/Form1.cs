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
using System.IO;
using System.Text.RegularExpressions;

namespace LostWinForms
{
    public partial class Form1 : Form
    {
        // Dll import
        public delegate void PFN_Logger(Int32 level, StringBuilder msg);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetLogger(PFN_Logger logger);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void InitializeWindow(IntPtr handle, bool windowed, UInt32 width, UInt32 height);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadFBX(
            String file,
            bool clearScene,
            bool importTexCoord,
            bool importAnimation,
            bool importVertexColor,
            bool importNormal,
            bool forceRegenerateNormal,
            bool generateNormalIfNotFound,
            bool importTangent,
            bool forceRegenerateTangent,
            bool generateTangentIfNotFound);

        private String[] LevelString = { "Info:                ", "Warning:             ", "Error:               " };
        private const String UserDataEncoding = "GB2312";
        private const String UserDataDirectory = "\\_LostReality\\";
        private const String UserDataFileName = "FBXEditor.usr";
        private const String UserDataKeyValueSeperator = " ............................. ";
        private const String UserDataOptionSeperator = ",\r\n";
        private bool bInitialized = false;

        private const String LastOpenPathKey = "UserData[LastOpenPathKey]";

        private Dictionary<string, string> UserData = new Dictionary<string, string>
        {
            { LastOpenPathKey, "C:\\" },
        };

        public Form1()
        {
            InitializeComponent();

            loadFBXToolStripMenuItem1.Click += OnLoadFBX;
            ImportOk.Click += ImportOk_Click;
            ImportCancel.Click += ImportCancel_Click;
        }

        // 获取用户本地数据保存路径
        private String GetLocalUserDataPath()
        {
            String userDataPath = System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData);

            // 如果文件夹不存在，创建
            userDataPath += UserDataDirectory;
            if (!Directory.Exists(userDataPath))
            {
                DirectoryInfo info = Directory.CreateDirectory(userDataPath);
            }

            // 如果文件不存在，创建
            userDataPath += UserDataFileName;
            if (!File.Exists(userDataPath))
            {
                FileStream file = File.Create(userDataPath);
                file.Close();
            }

            return userDataPath;
        }

        // 加载用户本地数据
        private void LoadLocalUserData()
        {
            string content = File.ReadAllText(GetLocalUserDataPath(), Encoding.GetEncoding(UserDataEncoding));
            string[] arr = Regex.Split(content, UserDataOptionSeperator);
            foreach (string elem in arr)
            {
                string[] keyVal = Regex.Split(elem, UserDataKeyValueSeperator);
                if (keyVal.Length == 2)
                {
                    UserData[keyVal[0]] = keyVal[1];
                }
                else
                {
                    throw new InvalidDataException();
                }
            }
        }

        // 保存用户本地数据
        private void SaveLocalUserData()
        {
            String content = "";
            foreach (var elem in UserData)
            {
                content += string.Format("{0}{1}{2}", elem.Key, UserDataKeyValueSeperator, elem.Value);
            }

            File.WriteAllText(GetLocalUserDataPath(), content, Encoding.GetEncoding(UserDataEncoding));
        }

        // 加载FBX按钮点击事件
        private void OnLoadFBX(Object sender, EventArgs e)
        {
            openFileDialog1.Title = "选择要打开的FBX文件";
            openFileDialog1.Filter = "FBX文件|*.fbx";
            openFileDialog1.RestoreDirectory = false;
            openFileDialog1.InitialDirectory = UserData[LastOpenPathKey];
            openFileDialog1.Multiselect = true;
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                ImportPanel.Visible = true;
                UserData[LastOpenPathKey] = openFileDialog1.FileName;
            }
        }

        // 日志界面线程输出日志
        private void LoggingActually(Int32 level, StringBuilder msg)
        {
            textBox1.Text += "\r\n" + DateTime.UtcNow.ToShortDateString() 
                + DateTime.UtcNow.ToShortTimeString() + " " + LevelString[level] + msg;

            textBox1.ScrollToCaret();
        }

        // 输出日志回调
        private void OnLogging(Int32 level, StringBuilder msg)
        {
            textBox1.Invoke(new PFN_Logger(LoggingActually), level, msg);
        }

        // Form1加载事件
        private void Form1_Load(object sender, EventArgs e)
        {
            if (!bInitialized)
            {
                // 初始化绘制窗口
                InitializeWindow(panel1.Handle, true, (uint)panel1.Width, (uint)panel1.Height);

                // 设置日志回调
                SetLogger(OnLogging);

                // 加载用户本地数据
                LoadLocalUserData();

                bInitialized = true;
            }
        }

        // Form1关闭事件
        private void Form1_Closed(object sender, EventArgs e)
        {
            SaveLocalUserData();
        }

        private void ImportOk_Click(object sender, EventArgs e)
        {
            ImportPanel.Visible = false;
            LoadFBX(
                openFileDialog1.FileName,
                true,
                ImportTexCoord.Checked,
                ImportAnimation.Checked,
                ImportVertexColor.Checked,
                ImportNormal.Checked,
                ForceRegenerateNormal.Checked,
                GenerateNormalIfNotFound.Checked,
                ImportTangent.Checked,
                ForceRegenerateTangent.Checked,
                GenerateTangentIfNotFound.Checked);
        }

        private void ImportCancel_Click(object sender, EventArgs e)
        {
            ImportPanel.Visible = false;
        }
    }
}
