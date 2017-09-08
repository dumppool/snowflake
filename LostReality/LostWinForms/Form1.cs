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
            String primitiveDir,
            String animationDir,
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
        public static extern void SetDisplayNormalLength(float len);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MoveCamera(float x, float y, float z);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RotateCamera(float p, float y, float r);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetAnimateRate(float rate);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDisplayFlags(uint flags);

        public delegate void PFN_AnimUpdate(uint flag, StringBuilder anim);
        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetAnimUpdater(PFN_AnimUpdate animUpdate);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayAnimation(String anim);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadAsset(UInt32 flag, String url);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ClearScene();

        private const uint AssetModel = 0;
        private const uint AssetAnimation = 1;

        private const uint FlagDisplayNormal = (1 << 0);
        private const uint FlagDisplayTangent = (1 << 1);
        private const uint FlagDisplaySkel = (1 << 2);
        private const uint FlagDisplayBB = (1 << 3);

        private uint DisplayFlags = 0;

        private String[] LevelString = {
            "Info:                ",
            "Warning:             ",
            "Error:               " };

        private const String UserDataEncoding = "GB2312";
        private const String UserDataDirectory = "\\_LostReality\\";
        private const String UserDataFileName = "FBXEditor.usr";
        private const String UserDataKeyValueSeperator = " ............................. ";
        private const String UserDataOptionSeperator = ",\r\n";
        private bool bInitialized = false;

        private const String LastOpenFbxKey             = "LastFbx";
        private const String LastOpenModelKey           = "LastModel";
        private const String LastOpenAnimationKey       = "LastAnimation";
        private const String PrimitiveOutputKey         = "PrimitiveOutput";
        private const String AnimationOutputKey         = "AnimationOutput";

        private Dictionary<string, string> UserData = new Dictionary<string, string>
        {
            { LastOpenFbxKey, "C:\\" },
            { LastOpenModelKey, "C:\\" },
            { LastOpenAnimationKey, "C:\\" },
            { PrimitiveOutputKey, "" },
            { AnimationOutputKey, "" },
        };

        private bool bMovingImportPanel = false;
        private Point LastMouseLocation;

        private PFN_Logger DelegateLogger;
        private PFN_AnimUpdate DelegateAnimUpdate;

        private bool bMouseDownPanel1 = false;
        private float MoveCameraStep = 0.10f;
        private float RotateCameraStep = 0.1f;

        private String[] FileNamesToOpen;

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

            LoadModelToolStripMenuItem.Click += LoadModelToolStripMenuItem_Click;
            LoadAnimationToolStripMenuItem.Click += LoadAnimationToolStripMenuItem_Click;

            ViewPanelToolStripMenuItem.Click += ViewPanelToolStripMenuItem_Click;
            ViewPanelOk.Click += ViewPanelOk_Click;
            SegLengthSlider.Scroll += SegLengthSlider_Scroll;
            AnimateRateSlider.Scroll += AnimateRateSlider_Scroll;

            DelegateAnimUpdate = new PFN_AnimUpdate(OnAnimUpdate);
            SetAnimUpdater(DelegateAnimUpdate);
            listBox1.SelectedIndexChanged += ListBox1_SelectedIndexChanged;

            ClearSceneToolStripMenuItem.Click += ClearSceneToolStripMenuItem_Click;
        }

        private void ClearSceneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClearScene();
        }

        private String GetDirectory(String path)
        {
            String outPath = Path.GetDirectoryName(path);
            return path;
        }

        private void LoadAnimationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "选择要打开的FBX文件";
            dlg.Filter = "";
            dlg.RestoreDirectory = false;
            dlg.InitialDirectory = Path.GetDirectoryName(UserData[LastOpenAnimationKey]);
            dlg.Multiselect = true;
            if (dlg.ShowDialog(panel1) == DialogResult.OK)
            {
                UserData[LastOpenAnimationKey] = dlg.FileName;
                foreach (String fileName in dlg.FileNames)
                {
                    LoadAsset(AssetAnimation, fileName);
                }
            }

            dlg.Dispose();
        }

        private void LoadModelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "选择要打开的模型文件";
            dlg.Filter = "";
            dlg.RestoreDirectory = false;
            dlg.InitialDirectory = Path.GetDirectoryName(UserData[LastOpenModelKey]);
            dlg.Multiselect = true;
            if (dlg.ShowDialog(panel1) == DialogResult.OK)
            {
                UserData[LastOpenModelKey] = dlg.FileName;
                foreach (String fileName in dlg.FileNames)
                {
                    LoadAsset(AssetModel, dlg.FileName);
                }
            }

            dlg.Dispose();
        }

        private void ListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
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
            float rate = AnimateRateSlider.Value * 0.5f;
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
            uint flag = 0;
            if (DisplaySkeleton.Checked)
            {
                flag |= FlagDisplaySkel;
            }

            if (DisplayBoundingBox.Checked)
            {
                flag |= FlagDisplayBB;
            }

            if (DisplayNormal.Checked)
            {
                flag |= FlagDisplayNormal;
            }

            if (DisplayTangent.Checked)
            {
                flag |= FlagDisplayTangent;
            }

            SetDisplayFlags(flag);
            ViewPanel.Visible = false;
        }

        // 获取用户本地数据保存路径
        private String GetLocalUserDataPath()
        {
            String userDataPath = System.Environment.GetFolderPath(System.Environment.SpecialFolder.ApplicationData);

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
                    // 也许是注释
                    continue;
                }
            }
        }

        // 保存用户本地数据
        private void SaveLocalUserData()
        {
            String content = "";
            foreach (var elem in UserData)
            {
                content += string.Format("{0}{1}{2}{3}", elem.Key, UserDataKeyValueSeperator, elem.Value, UserDataOptionSeperator);
            }

            File.WriteAllText(GetLocalUserDataPath(), content, Encoding.GetEncoding(UserDataEncoding));
        }

        // 加载FBX按钮点击事件
        private void OnLoadFBX(Object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "选择要打开的FBX文件";
            dlg.Filter = "FBX文件|*.fbx";
            dlg.RestoreDirectory = false;
            dlg.InitialDirectory = Path.GetDirectoryName(UserData[LastOpenFbxKey]);
            dlg.Multiselect = true;
            if (dlg.ShowDialog(panel1) == DialogResult.OK)
            {
                UserData[LastOpenFbxKey] = dlg.FileName;
                FileNamesToOpen = dlg.FileNames;
                PrimitiveOutputText.Text = UserData[PrimitiveOutputKey];
                AnimationOutputText.Text = UserData[AnimationOutputKey];
                ImportPanel.Visible = true;
            }

            dlg.Dispose();
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

            foreach (String fileName in FileNamesToOpen)
            {
                LoadFBX(
                    fileName,
                    PrimitiveOutputText.Text,
                    AnimationOutputText.Text,
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

            Array.Clear(FileNamesToOpen, 0, FileNamesToOpen.Length);

            UserData[PrimitiveOutputKey] = PrimitiveOutputText.Text;
            UserData[AnimationOutputKey] = AnimationOutputText.Text;
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
