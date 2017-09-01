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
            bool mergeNormalTangentAll,
            bool importNormal,
            bool forceRegenerateNormal,
            bool generateNormalIfNotFound,
            bool importTangent,
            bool forceRegenerateTangent,
            bool generateTangentIfNotFound);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDisplayNormal(bool enable);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDisplayTangent(bool enable);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDisplayNormalLength(float len);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MoveCamera(float x, float y, float z);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RotateCamera(float p, float y, float r);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetAnimateRate(float rate);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDisplaySkeleton(uint flag);

        public delegate void PFN_AnimUpdate(uint flag, StringBuilder anim);
        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetAnimUpdater(PFN_AnimUpdate animUpdate);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayAnimation(String anim);

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

        private bool bMovingImportPanel = false;
        private Point LastMouseLocation;

        private PFN_Logger DelegateLogger;
        private PFN_AnimUpdate DelegateAnimUpdate;

        private bool bMouseDownPanel1 = false;
        private float MoveCameraStep = 0.10f;
        private float RotateCameraStep = 0.1f; 

        public Form1()
        {
            InitializeComponent();

            panel1.MouseDown += Panel1_MouseDown;
            panel1.MouseUp += Panel1_MouseUp;
            panel1.MouseMove += Panel1_MouseMove;
            panel1.MouseWheel += Panel1_MouseWheel;
            panel1.KeyDown += Panel1_KeyDown;

            loadFBXToolStripMenuItem1.Click += OnLoadFBX;
            ImportOk.Click += ImportOk_Click;
            ImportCancel.Click += ImportCancel_Click;

            ViewPanelToolStripMenuItem.Click += ViewPanelToolStripMenuItem_Click;
            ViewPanelOk.Click += ViewPanelOk_Click;
            SegLengthSlider.Scroll += SegLengthSlider_Scroll;
            AnimateRateSlider.Scroll += AnimateRateSlider_Scroll;

            DelegateAnimUpdate = new PFN_AnimUpdate(OnAnimUpdate);
            SetAnimUpdater(DelegateAnimUpdate);
            listBox1.SelectedIndexChanged += ListBox1_SelectedIndexChanged;
        }

        private void ListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //Console.WriteLine(listBox1.Items[listBox1.SelectedIndex].ToString());
            PlayAnimation(listBox1.Items[listBox1.SelectedIndex].ToString());
        }

        private void OnAnimUpdate(uint flag, StringBuilder anim)
        {
            listBox1.Invoke(new PFN_AnimUpdate(AnimUpdate), flag, anim);
        }

        private void AnimUpdate(uint flag, StringBuilder anim)
        {
            if ((flag & (1<<0)) == (1<<0))
            {
                // clear
                listBox1.Items.Clear();
            }
            else if ((flag & (1<<1)) == (1<<1))
            {
                // add
                listBox1.Items.Add(anim);
            }
        }

        private void AnimateRateSlider_Scroll(object sender, EventArgs e)
        {
            float rate = AnimateRateSlider.Value * 0.05f;
            AnimateRateValue.Text = rate.ToString();
            SetAnimateRate(rate);
        }

        private void Panel1_MouseWheel(object sender, MouseEventArgs e)
        {
            MoveCamera(0.0f, 0.0f, e.Delta * MoveCameraStep);
            //Console.WriteLine("{0}", e.Delta * MoveCameraStep);
        }

        private void Panel1_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyCode <= Keys.F24 && Keys.D0 <= e.KeyCode)
            {
                e.IsInputKey = true;
            }
        }

        private void Panel1_KeyDown(object sender, KeyEventArgs e)
        {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            switch (e.KeyCode)
            {
                case Keys.W:
                    z += MoveCameraStep;
                    break;
                case Keys.S:
                    z -= MoveCameraStep;
                    break;
                case Keys.A:
                    x -= MoveCameraStep;
                    break;
                case Keys.D:
                    x += MoveCameraStep;
                    break;
                case Keys.Q:
                    y += MoveCameraStep;
                    break;
                case Keys.E:
                    y -= MoveCameraStep;
                    break;
                default:
                    break;
            }

            MoveCamera(x, y, z);
            Console.WriteLine("{0} {1} {2}", x, y, z);
        }

        private void Panel1_MouseMove(object sender, MouseEventArgs e)
        {
            if (bMouseDownPanel1)
            {
                float dx = e.Location.X - LastMouseLocation.X;
                float dy = e.Location.Y - LastMouseLocation.Y;
                LastMouseLocation = e.Location;
                RotateCamera(dy * RotateCameraStep, -dx * RotateCameraStep, 0.0f);
            }
        }

        private void Panel1_MouseUp(object sender, MouseEventArgs e)
        {
            bMouseDownPanel1 = false;
            LastMouseLocation = e.Location;
        }

        private void Panel1_MouseDown(object sender, MouseEventArgs e)
        {
            bMouseDownPanel1 = true;
            LastMouseLocation = e.Location;
        }

        private void SegLengthSlider_Scroll(object sender, EventArgs e)
        {
            SegLengthValue.Text = SegLengthSlider.Value.ToString();
            SetDisplayNormalLength(SegLengthSlider.Value);
        }

        private void ViewPanelOk_Click(object sender, EventArgs e)
        {
            uint flag;
            SetDisplayNormal(DisplayNormal.Checked);
            SetDisplayTangent(DisplayTangent.Checked);

            flag = 0;
            if (DisplaySkeleton.Checked)
            {
                flag |= (1 << 0);
            }

            if (DisplaySkeleton2.Checked)
            {
                flag |= (1 << 1);
            }
                
            SetDisplaySkeleton(flag);
            ViewPanel.Visible = false;
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
                DelegateLogger = new PFN_Logger(OnLogging);
                SetLogger(DelegateLogger);

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
                MergeNormalTangentAll.Checked,
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

        private void ImportPanel_MouseDown(object sender, MouseEventArgs e)
        {
            bMovingImportPanel = true;
            LastMouseLocation = e.Location;
        }

        private void ImportPanel_MouseMove(object sender, MouseEventArgs e)
        {
            // TODO: 需要在同一个坐标系下计算
            if (bMovingImportPanel)
            {
                Point final = ImportPanel.Location;
                final.X += e.Location.X - LastMouseLocation.X;
                final.Y += e.Location.Y - LastMouseLocation.Y;
                LastMouseLocation = e.Location;
                ImportPanel.Location = final;
            }
        }

        private void ImportPanel_MouseUp(object sender, MouseEventArgs e)
        {
            bMovingImportPanel = false;
            LastMouseLocation = e.Location;
        }

        private void ViewPanelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ViewPanel.Visible = true;
        }
    }
}
