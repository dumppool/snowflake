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
        public static extern void InitializeProcessUnique();

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyProcessUnique();

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
        public static extern Int32 SetDisplayNormalLength(float len);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 MoveCamera(float x, float y, float z);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 RotateCamera(float p, float y, float r);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 SetAnimateRate(float rate);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 SetDisplayFlags(uint flags);

        public delegate void PFN_UpdateFlagAndString(uint flag, StringBuilder str);
        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 SetUpdateFlagAndString(PFN_UpdateFlagAndString pfn);

        public delegate void PFN_UpdateFlagAnd32Bit(uint flag, UInt32 val);
        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 SetUpdateFlagAnd32Bit(PFN_UpdateFlagAnd32Bit pfn);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 PlayAnimation(String anim);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 ClearScene();

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 Shutdown();

        public delegate void PFN_UpdatePosAndRot(float x, float y, float z, float pitch, float yaw, float roll);
        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 SetUpdatePosAndRot(PFN_UpdatePosAndRot pfn);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 OnHover(Int32 x, Int32 y);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 OnClick(Int32 x, Int32 y);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 OnDragging(Int32 x, Int32 y);

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 OnEndDrag();

        [DllImport("LostCore.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 AssetOperate(UInt32 flag, String url);

        private const uint AssetOperateLoadModel = 0;
        private const uint AssetOperateLoadAnimation = 1;
        private const uint AssetOperateLoadScene = 2;
        private const uint AssetOperateSaveScene = 3;

        private const uint UpdateAnimClear = 0;
        private const uint UpdateAnimAdd = 1;
        private const uint UpdateMonitorTargetName = 2;
        private const uint UpdateRayTestDistance = 3;

        private const uint FlagDisplayNormal = (1 << 0);
        private const uint FlagDisplayTangent = (1 << 1);
        private const uint FlagDisplaySkel = (1 << 2);
        private const uint FlagDisplayBB = (1 << 3);

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
        private const String LastOpenSceneKey           = "LastScene";
        private const String LastSaveSceneKey           = "LastSaveScene";
        private const String PrimitiveOutputKey         = "PrimitiveOutput";
        private const String AnimationOutputKey         = "AnimationOutput";

        private Dictionary<string, string> UserData = new Dictionary<string, string>
        {
            { LastOpenFbxKey, "C:\\" },
            { LastOpenModelKey, "C:\\" },
            { LastOpenAnimationKey, "C:\\" },
            { LastOpenSceneKey, "C:\\" },
            { LastSaveSceneKey, "C:\\" },
            { PrimitiveOutputKey, "" },
            { AnimationOutputKey, "" },
        };

        private PFN_Logger DelegateLogger;
        private PFN_UpdateFlagAndString DelegateUpdateFlagAndString;
        private PFN_UpdateFlagAnd32Bit DelegateUpdateFlagAnd32Bit;
        private PFN_UpdatePosAndRot DelegateUpdatePosAndRot;

        private bool bMouseDownLeft = false;
        private bool bMouseDownRight = false;
        private Point LastMouseLocationLeft;
        private Point LastMouseLocationRight;
        private float MoveCameraStep = 0.10f;
        private float RotateCameraStep = 0.1f;
        private float ClickThreshold = 4.0f;

        private String[] FileNamesToOpen;

        public Form1()
        {
            InitializeComponent();

            RenderPanel.MouseDown += Panel1_MouseDown;
            RenderPanel.MouseUp += Panel1_MouseUp;
            RenderPanel.MouseMove += Panel1_MouseMove;
            RenderPanel.MouseWheel += Panel1_MouseWheel;
            RenderPanel.MouseCaptureChanged += Panel1_MouseCaptureChanged;
            RenderPanel.KeyDown += Panel1_KeyDown;
            //RenderPanel.Press

            loadFBXToolStripMenuItem1.Click += OnLoadFBX;
            ImportOk.Click += ImportOk_Click;
            ImportCancel.Click += ImportCancel_Click;

            LoadModelToolStripMenuItem.Click += LoadModelToolStripMenuItem_Click;
            LoadAnimationToolStripMenuItem.Click += LoadAnimationToolStripMenuItem_Click;
            LoadSceneToolStripMenuItem.Click += LoadSceneToolStripMenuItem_Click;
            SaveSceneToolStripMenuItem.Click += SaveSceneToolStripMenuItem_Click;

            ViewPanelToolStripMenuItem.Click += ViewPanelToolStripMenuItem_Click;
            ViewPanelOk.Click += ViewPanelOk_Click;
            SegLengthSlider.Scroll += SegLengthSlider_Scroll;
            AnimateRateSlider.Scroll += AnimateRateSlider_Scroll;

            DelegateUpdateFlagAndString = new PFN_UpdateFlagAndString(OnUpdateFlagAndString);
            SetUpdateFlagAndString(DelegateUpdateFlagAndString);
            DelegateUpdateFlagAnd32Bit = new PFN_UpdateFlagAnd32Bit(OnUpdateFlagAnd32Bit);
            SetUpdateFlagAnd32Bit(DelegateUpdateFlagAnd32Bit);
            listBox1.SelectedIndexChanged += ListBox1_SelectedIndexChanged;

            DelegateUpdatePosAndRot = new PFN_UpdatePosAndRot(OnUpdatePosAndRot);
            SetUpdatePosAndRot(DelegateUpdatePosAndRot);

            ClearSceneToolStripMenuItem.Click += ClearSceneToolStripMenuItem_Click;
        }

        private void SaveSceneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Title = "选择保存路径及文件名";
            dlg.InitialDirectory = Path.GetDirectoryName(UserData[LastSaveSceneKey]);
            dlg.OverwritePrompt = true;
            dlg.AddExtension = true;
            dlg.DefaultExt = "json";
            if (dlg.ShowDialog(RenderPanel) == DialogResult.OK)
            {
                UserData[LastSaveSceneKey] = dlg.FileName;
                AssetOperate(AssetOperateSaveScene, dlg.FileName);
            }

            dlg.Dispose();
        }

        private void LoadSceneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "选择场景文件";
            dlg.Filter = "";
            dlg.RestoreDirectory = false;
            dlg.InitialDirectory = Path.GetDirectoryName(UserData[LastOpenSceneKey]);
            dlg.Multiselect = false;
            if (dlg.ShowDialog(RenderPanel) == DialogResult.OK)
            {
                UserData[LastOpenSceneKey] = dlg.FileName;
                foreach (String fileName in dlg.FileNames)
                {
                    AssetOperate(AssetOperateLoadScene, fileName);
                }
            }

            dlg.Dispose();
        }

        private void OnUpdatePosAndRot(float x, float y, float z, float pitch, float yaw, float roll)
        {
            MonitorPanel.BeginInvoke(new PFN_UpdatePosAndRot(InvokeUpdatePosAndRot), x, y, z, pitch, yaw, roll);
        }

        private void InvokeUpdatePosAndRot(float x, float y, float z, float pitch, float yaw, float roll)
        {
            ValuePosX.Text = x.ToString();
            ValuePosY.Text = y.ToString();
            ValuePosZ.Text = z.ToString();
            ValuePitch.Text = pitch.ToString();
            ValueYaw.Text = yaw.ToString();
            ValueRoll.Text = roll.ToString();
        }

        private void ClearSceneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClearScene();
        }

        private void LoadAnimationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "选择动画文件";
            dlg.Filter = "";
            dlg.RestoreDirectory = false;
            dlg.InitialDirectory = Path.GetDirectoryName(UserData[LastOpenAnimationKey]);
            dlg.Multiselect = true;
            if (dlg.ShowDialog(RenderPanel) == DialogResult.OK)
            {
                UserData[LastOpenAnimationKey] = dlg.FileName;
                foreach (String fileName in dlg.FileNames)
                {
                    AssetOperate(AssetOperateLoadAnimation, fileName);
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
            if (dlg.ShowDialog(RenderPanel) == DialogResult.OK)
            {
                UserData[LastOpenModelKey] = dlg.FileName;
                foreach (String fileName in dlg.FileNames)
                {
                    AssetOperate(AssetOperateLoadModel, dlg.FileName);
                }
            }

            dlg.Dispose();
        }

        private void ListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            PlayAnimation(listBox1.Items[listBox1.SelectedIndex].ToString());
        }

        private void OnUpdateFlagAndString(uint flag, StringBuilder str)
        {
            if ((flag == UpdateAnimClear || flag == UpdateAnimAdd))
            {
                listBox1.BeginInvoke(new PFN_UpdateFlagAndString(UpdateAnimList), flag, str);
            }
            else if (flag == UpdateMonitorTargetName)
            {
                MonitorPanel.BeginInvoke(new PFN_UpdateFlagAndString(UpdateMonitorTarget), flag, str);
            }
            else
            {
                throw new NotImplementedException();
            }
        }

        private void OnUpdateFlagAnd32Bit(uint flag, uint val)
        {
            if (flag == UpdateRayTestDistance)
            {
                RayTestResult.BeginInvoke(new PFN_UpdateFlagAnd32Bit(InvokeUpdateRayTestResult), flag, val);
            }
        }

        private void InvokeUpdateRayTestResult(uint flag, uint result)
        {
            RayTestResult.Text = BitConverter.ToSingle(BitConverter.GetBytes(result), 0).ToString();
        }

        private void UpdateAnimList(uint flag, StringBuilder anim)
        {
            if (flag == UpdateAnimClear)
            {
                // clear
                listBox1.Items.Clear();
            }
            else if (flag == UpdateAnimAdd)
            {
                // add
                listBox1.Items.Add(anim);
            }
        }

        private void UpdateMonitorTarget(uint flag, StringBuilder str)
        {
            ValueTargetName.Text = str.ToString();
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
            if (bMouseDownRight)
            {
                float dx = e.Location.X - LastMouseLocationRight.X;
                float dy = e.Location.Y - LastMouseLocationRight.Y;
                LastMouseLocationRight = e.Location;
                RotateCamera(dy * RotateCameraStep, -dx * RotateCameraStep, 0.0f);
            }
            else if (bMouseDownLeft)
            {
                OnDragging(e.Location.X, e.Location.Y);
            }
            else
            {
                OnHover(e.Location.X, e.Location.Y);
            }
        }

        private void Panel1_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                bMouseDownLeft = false;
                float dx = e.Location.X - LastMouseLocationLeft.X;
                float dy = e.Location.Y - LastMouseLocationLeft.Y;
                LastMouseLocationLeft = e.Location;
                float deltaSquared = dx * dx + dy * dy;
                bool clicked = deltaSquared < ClickThreshold;
                if (clicked)
                {
                    OnClick(e.Location.X, e.Location.Y);
                }
                else
                {
                    OnEndDrag();
                }
            }
            else if (e.Button == MouseButtons.Right)
            {
                bMouseDownRight = false;
            }
        }

        private void Panel1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                bMouseDownLeft = true;
                LastMouseLocationLeft = e.Location;
            }
            else if (e.Button == MouseButtons.Right)
            {
                bMouseDownRight = true;
                LastMouseLocationRight = e.Location;
            }
        }

        private void Panel1_MouseCaptureChanged(object sender, EventArgs e)
        {
            //throw new NotImplementedException();
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
            if (dlg.ShowDialog(RenderPanel) == DialogResult.OK)
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
            textBox1.BeginInvoke(new PFN_Logger(LoggingActually), level, msg);
        }

        // Form1加载事件
        private void Form1_Load(object sender, EventArgs e)
        {
            if (!bInitialized)
            {
                // 初始化tls
                InitializeProcessUnique();

                // 初始化绘制窗口
                InitializeWindow(RenderPanel.Handle, true, (uint)RenderPanel.Width, (uint)RenderPanel.Height);

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
            SetUpdateFlagAndString(null);
            SetUpdatePosAndRot(null);
            DelegateUpdateFlagAndString = null;
            DelegateUpdatePosAndRot = null;
            SaveLocalUserData();
            Shutdown();
            DestroyProcessUnique();
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

        private void ViewPanelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ViewPanel.Visible = true;
        }
    }
}
