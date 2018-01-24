namespace LostWinForms
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("X");
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Y");
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("Z");
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("位置", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3});
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("旋转");
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode("缩放");
            System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode("空间", new System.Windows.Forms.TreeNode[] {
            treeNode4,
            treeNode5,
            treeNode6});
            System.Windows.Forms.TreeNode treeNode8 = new System.Windows.Forms.TreeNode("加载动作");
            System.Windows.Forms.TreeNode treeNode9 = new System.Windows.Forms.TreeNode("动作列表");
            System.Windows.Forms.TreeNode treeNode10 = new System.Windows.Forms.TreeNode("动作", new System.Windows.Forms.TreeNode[] {
            treeNode8,
            treeNode9});
            System.Windows.Forms.TreeNode treeNode11 = new System.Windows.Forms.TreeNode("属性", new System.Windows.Forms.TreeNode[] {
            treeNode7,
            treeNode10});
            this.RenderPanel = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.ImportPanel = new System.Windows.Forms.Panel();
            this.AnimationOutputText = new System.Windows.Forms.TextBox();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.MergeNormalTangentAll = new System.Windows.Forms.CheckBox();
            this.PrimitiveOutputText = new System.Windows.Forms.TextBox();
            this.ImportTexCoord = new System.Windows.Forms.CheckBox();
            this.ImportCancel = new System.Windows.Forms.Button();
            this.ImportOk = new System.Windows.Forms.Button();
            this.ImportAnimation = new System.Windows.Forms.CheckBox();
            this.ImportVertexColor = new System.Windows.Forms.CheckBox();
            this.GenerateTangentIfNotFound = new System.Windows.Forms.CheckBox();
            this.ForceRegenerateTangent = new System.Windows.Forms.CheckBox();
            this.ImportTangent = new System.Windows.Forms.CheckBox();
            this.GenerateNormalIfNotFound = new System.Windows.Forms.CheckBox();
            this.ForceRegenerateNormal = new System.Windows.Forms.CheckBox();
            this.ImportNormal = new System.Windows.Forms.CheckBox();
            this.ViewPanel = new System.Windows.Forms.Panel();
            this.DisplayAnimationListCheckBox = new System.Windows.Forms.CheckBox();
            this.DisplayBoundingBox = new System.Windows.Forms.CheckBox();
            this.DisplaySkeleton = new System.Windows.Forms.CheckBox();
            this.AnimateRateValue = new System.Windows.Forms.TextBox();
            this.AnimateRateSlider = new System.Windows.Forms.TrackBar();
            this.SegLengthValue = new System.Windows.Forms.TextBox();
            this.SegLengthSlider = new System.Windows.Forms.TrackBar();
            this.DisplayNormal = new System.Windows.Forms.CheckBox();
            this.DisplayTangent = new System.Windows.Forms.CheckBox();
            this.ViewPanelOk = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.MonitorPanel = new System.Windows.Forms.Panel();
            this.RayTestResult = new System.Windows.Forms.TextBox();
            this.ValueRoll = new System.Windows.Forms.TextBox();
            this.ValueYaw = new System.Windows.Forms.TextBox();
            this.ValuePitch = new System.Windows.Forms.TextBox();
            this.TextRoll = new System.Windows.Forms.TextBox();
            this.TextYaw = new System.Windows.Forms.TextBox();
            this.TextPitch = new System.Windows.Forms.TextBox();
            this.ValuePosZ = new System.Windows.Forms.TextBox();
            this.ValuePosY = new System.Windows.Forms.TextBox();
            this.ValuePosX = new System.Windows.Forms.TextBox();
            this.TextPosZ = new System.Windows.Forms.TextBox();
            this.TextPosY = new System.Windows.Forms.TextBox();
            this.TextPosX = new System.Windows.Forms.TextBox();
            this.ValueTargetName = new System.Windows.Forms.TextBox();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.loadFBXToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.LoadModelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.LoadAnimationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.LoadSceneToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.SaveSceneToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
            this.ViewPanelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ClearSceneToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.RecordConsoleTSMI = new System.Windows.Forms.ToolStripMenuItem();
            this.SwitchConsoleTSMI = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripComboBox1 = new System.Windows.Forms.ToolStripComboBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.RenderPanel.SuspendLayout();
            this.panel1.SuspendLayout();
            this.ImportPanel.SuspendLayout();
            this.ViewPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AnimateRateSlider)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SegLengthSlider)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.MonitorPanel.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // RenderPanel
            // 
            this.RenderPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.RenderPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.RenderPanel.Controls.Add(this.panel1);
            this.RenderPanel.Controls.Add(this.ImportPanel);
            this.RenderPanel.Controls.Add(this.ViewPanel);
            this.RenderPanel.Location = new System.Drawing.Point(-1, 74);
            this.RenderPanel.Margin = new System.Windows.Forms.Padding(4);
            this.RenderPanel.Name = "RenderPanel";
            this.RenderPanel.Size = new System.Drawing.Size(1312, 508);
            this.RenderPanel.TabIndex = 0;
            this.RenderPanel.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.Panel1_PreviewKeyDown);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.treeView1);
            this.panel1.Controls.Add(this.textBox4);
            this.panel1.Location = new System.Drawing.Point(64, 21);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 412);
            this.panel1.TabIndex = 7;
            this.panel1.Visible = false;
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(17, 54);
            this.treeView1.Name = "treeView1";
            treeNode1.Name = "Node7";
            treeNode1.Text = "X";
            treeNode2.Name = "Node8";
            treeNode2.Text = "Y";
            treeNode3.Name = "Node9";
            treeNode3.Text = "Z";
            treeNode4.Name = "Node3";
            treeNode4.Text = "位置";
            treeNode5.Name = "Node4";
            treeNode5.Text = "旋转";
            treeNode6.Name = "Node5";
            treeNode6.Text = "缩放";
            treeNode7.Name = "Node1";
            treeNode7.Text = "空间";
            treeNode8.Name = "Node6";
            treeNode8.Text = "加载动作";
            treeNode9.Name = "Node10";
            treeNode9.Text = "动作列表";
            treeNode10.Name = "Node2";
            treeNode10.Text = "动作";
            treeNode11.Name = "Node0";
            treeNode11.Text = "属性";
            this.treeView1.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode11});
            this.treeView1.Size = new System.Drawing.Size(166, 342);
            this.treeView1.TabIndex = 1;
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(17, 4);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(166, 25);
            this.textBox4.TabIndex = 0;
            // 
            // ImportPanel
            // 
            this.ImportPanel.Controls.Add(this.AnimationOutputText);
            this.ImportPanel.Controls.Add(this.textBox3);
            this.ImportPanel.Controls.Add(this.textBox2);
            this.ImportPanel.Controls.Add(this.MergeNormalTangentAll);
            this.ImportPanel.Controls.Add(this.PrimitiveOutputText);
            this.ImportPanel.Controls.Add(this.ImportTexCoord);
            this.ImportPanel.Controls.Add(this.ImportCancel);
            this.ImportPanel.Controls.Add(this.ImportOk);
            this.ImportPanel.Controls.Add(this.ImportAnimation);
            this.ImportPanel.Controls.Add(this.ImportVertexColor);
            this.ImportPanel.Controls.Add(this.GenerateTangentIfNotFound);
            this.ImportPanel.Controls.Add(this.ForceRegenerateTangent);
            this.ImportPanel.Controls.Add(this.ImportTangent);
            this.ImportPanel.Controls.Add(this.GenerateNormalIfNotFound);
            this.ImportPanel.Controls.Add(this.ForceRegenerateNormal);
            this.ImportPanel.Controls.Add(this.ImportNormal);
            this.ImportPanel.Location = new System.Drawing.Point(347, 21);
            this.ImportPanel.Margin = new System.Windows.Forms.Padding(4);
            this.ImportPanel.Name = "ImportPanel";
            this.ImportPanel.Size = new System.Drawing.Size(324, 514);
            this.ImportPanel.TabIndex = 5;
            this.ImportPanel.Visible = false;
            // 
            // AnimationOutputText
            // 
            this.AnimationOutputText.Location = new System.Drawing.Point(157, 60);
            this.AnimationOutputText.Margin = new System.Windows.Forms.Padding(4);
            this.AnimationOutputText.Name = "AnimationOutputText";
            this.AnimationOutputText.Size = new System.Drawing.Size(161, 25);
            this.AnimationOutputText.TabIndex = 15;
            // 
            // textBox3
            // 
            this.textBox3.BackColor = System.Drawing.SystemColors.Desktop;
            this.textBox3.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox3.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox3.ForeColor = System.Drawing.Color.Silver;
            this.textBox3.Location = new System.Drawing.Point(15, 64);
            this.textBox3.Margin = new System.Windows.Forms.Padding(4);
            this.textBox3.Name = "textBox3";
            this.textBox3.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.textBox3.Size = new System.Drawing.Size(131, 20);
            this.textBox3.TabIndex = 13;
            this.textBox3.Text = "animation文件夹";
            // 
            // textBox2
            // 
            this.textBox2.BackColor = System.Drawing.SystemColors.Desktop;
            this.textBox2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox2.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox2.ForeColor = System.Drawing.Color.Silver;
            this.textBox2.Location = new System.Drawing.Point(15, 25);
            this.textBox2.Margin = new System.Windows.Forms.Padding(4);
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.textBox2.Size = new System.Drawing.Size(131, 20);
            this.textBox2.TabIndex = 12;
            this.textBox2.Text = "primitive文件夹";
            // 
            // MergeNormalTangentAll
            // 
            this.MergeNormalTangentAll.AutoSize = true;
            this.MergeNormalTangentAll.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.MergeNormalTangentAll.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.MergeNormalTangentAll.Location = new System.Drawing.Point(33, 98);
            this.MergeNormalTangentAll.Margin = new System.Windows.Forms.Padding(4);
            this.MergeNormalTangentAll.Name = "MergeNormalTangentAll";
            this.MergeNormalTangentAll.Size = new System.Drawing.Size(213, 24);
            this.MergeNormalTangentAll.TabIndex = 11;
            this.MergeNormalTangentAll.Text = "MergeNormalTangentAll";
            this.MergeNormalTangentAll.UseVisualStyleBackColor = true;
            // 
            // PrimitiveOutputText
            // 
            this.PrimitiveOutputText.Location = new System.Drawing.Point(157, 21);
            this.PrimitiveOutputText.Margin = new System.Windows.Forms.Padding(4);
            this.PrimitiveOutputText.Name = "PrimitiveOutputText";
            this.PrimitiveOutputText.Size = new System.Drawing.Size(161, 25);
            this.PrimitiveOutputText.TabIndex = 14;
            // 
            // ImportTexCoord
            // 
            this.ImportTexCoord.AutoSize = true;
            this.ImportTexCoord.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportTexCoord.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportTexCoord.Location = new System.Drawing.Point(33, 321);
            this.ImportTexCoord.Margin = new System.Windows.Forms.Padding(4);
            this.ImportTexCoord.Name = "ImportTexCoord";
            this.ImportTexCoord.Size = new System.Drawing.Size(153, 24);
            this.ImportTexCoord.TabIndex = 10;
            this.ImportTexCoord.Text = "ImportTexCoord";
            this.ImportTexCoord.UseVisualStyleBackColor = true;
            // 
            // ImportCancel
            // 
            this.ImportCancel.Font = new System.Drawing.Font("Microsoft YaHei", 7.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportCancel.Location = new System.Drawing.Point(189, 452);
            this.ImportCancel.Margin = new System.Windows.Forms.Padding(4);
            this.ImportCancel.Name = "ImportCancel";
            this.ImportCancel.Size = new System.Drawing.Size(71, 29);
            this.ImportCancel.TabIndex = 9;
            this.ImportCancel.Text = "Cancel";
            this.ImportCancel.UseVisualStyleBackColor = true;
            // 
            // ImportOk
            // 
            this.ImportOk.Font = new System.Drawing.Font("Microsoft YaHei", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportOk.Location = new System.Drawing.Point(33, 445);
            this.ImportOk.Margin = new System.Windows.Forms.Padding(4);
            this.ImportOk.Name = "ImportOk";
            this.ImportOk.Size = new System.Drawing.Size(145, 36);
            this.ImportOk.TabIndex = 8;
            this.ImportOk.Text = "Import";
            this.ImportOk.UseVisualStyleBackColor = true;
            // 
            // ImportAnimation
            // 
            this.ImportAnimation.AutoSize = true;
            this.ImportAnimation.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportAnimation.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportAnimation.Location = new System.Drawing.Point(33, 355);
            this.ImportAnimation.Margin = new System.Windows.Forms.Padding(4);
            this.ImportAnimation.Name = "ImportAnimation";
            this.ImportAnimation.Size = new System.Drawing.Size(156, 24);
            this.ImportAnimation.TabIndex = 7;
            this.ImportAnimation.Text = "ImportAnimation";
            this.ImportAnimation.UseVisualStyleBackColor = true;
            // 
            // ImportVertexColor
            // 
            this.ImportVertexColor.AutoSize = true;
            this.ImportVertexColor.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportVertexColor.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportVertexColor.Location = new System.Drawing.Point(33, 389);
            this.ImportVertexColor.Margin = new System.Windows.Forms.Padding(4);
            this.ImportVertexColor.Name = "ImportVertexColor";
            this.ImportVertexColor.Size = new System.Drawing.Size(169, 24);
            this.ImportVertexColor.TabIndex = 6;
            this.ImportVertexColor.Text = "ImportVertexColor";
            this.ImportVertexColor.UseVisualStyleBackColor = true;
            // 
            // GenerateTangentIfNotFound
            // 
            this.GenerateTangentIfNotFound.AutoSize = true;
            this.GenerateTangentIfNotFound.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.GenerateTangentIfNotFound.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.GenerateTangentIfNotFound.Location = new System.Drawing.Point(33, 275);
            this.GenerateTangentIfNotFound.Margin = new System.Windows.Forms.Padding(4);
            this.GenerateTangentIfNotFound.Name = "GenerateTangentIfNotFound";
            this.GenerateTangentIfNotFound.Size = new System.Drawing.Size(241, 24);
            this.GenerateTangentIfNotFound.TabIndex = 5;
            this.GenerateTangentIfNotFound.Text = "GenerateTangentIfNotFound";
            this.GenerateTangentIfNotFound.UseVisualStyleBackColor = true;
            // 
            // ForceRegenerateTangent
            // 
            this.ForceRegenerateTangent.AutoSize = true;
            this.ForceRegenerateTangent.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ForceRegenerateTangent.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ForceRegenerateTangent.Location = new System.Drawing.Point(33, 248);
            this.ForceRegenerateTangent.Margin = new System.Windows.Forms.Padding(4);
            this.ForceRegenerateTangent.Name = "ForceRegenerateTangent";
            this.ForceRegenerateTangent.Size = new System.Drawing.Size(217, 24);
            this.ForceRegenerateTangent.TabIndex = 4;
            this.ForceRegenerateTangent.Text = "ForceRegenerateTangent";
            this.ForceRegenerateTangent.UseVisualStyleBackColor = true;
            // 
            // ImportTangent
            // 
            this.ImportTangent.AutoSize = true;
            this.ImportTangent.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportTangent.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportTangent.Location = new System.Drawing.Point(33, 220);
            this.ImportTangent.Margin = new System.Windows.Forms.Padding(4);
            this.ImportTangent.Name = "ImportTangent";
            this.ImportTangent.Size = new System.Drawing.Size(141, 24);
            this.ImportTangent.TabIndex = 3;
            this.ImportTangent.Text = "ImportTangent";
            this.ImportTangent.UseVisualStyleBackColor = true;
            // 
            // GenerateNormalIfNotFound
            // 
            this.GenerateNormalIfNotFound.AutoSize = true;
            this.GenerateNormalIfNotFound.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.GenerateNormalIfNotFound.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.GenerateNormalIfNotFound.Location = new System.Drawing.Point(33, 186);
            this.GenerateNormalIfNotFound.Margin = new System.Windows.Forms.Padding(4);
            this.GenerateNormalIfNotFound.Name = "GenerateNormalIfNotFound";
            this.GenerateNormalIfNotFound.Size = new System.Drawing.Size(235, 24);
            this.GenerateNormalIfNotFound.TabIndex = 2;
            this.GenerateNormalIfNotFound.Text = "GenerateNormalIfNotFound";
            this.GenerateNormalIfNotFound.UseVisualStyleBackColor = true;
            // 
            // ForceRegenerateNormal
            // 
            this.ForceRegenerateNormal.AutoSize = true;
            this.ForceRegenerateNormal.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ForceRegenerateNormal.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ForceRegenerateNormal.Location = new System.Drawing.Point(33, 159);
            this.ForceRegenerateNormal.Margin = new System.Windows.Forms.Padding(4);
            this.ForceRegenerateNormal.Name = "ForceRegenerateNormal";
            this.ForceRegenerateNormal.Size = new System.Drawing.Size(211, 24);
            this.ForceRegenerateNormal.TabIndex = 1;
            this.ForceRegenerateNormal.Text = "ForceRegenerateNormal";
            this.ForceRegenerateNormal.UseVisualStyleBackColor = true;
            // 
            // ImportNormal
            // 
            this.ImportNormal.AutoSize = true;
            this.ImportNormal.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportNormal.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportNormal.Location = new System.Drawing.Point(33, 131);
            this.ImportNormal.Margin = new System.Windows.Forms.Padding(4);
            this.ImportNormal.Name = "ImportNormal";
            this.ImportNormal.Size = new System.Drawing.Size(135, 24);
            this.ImportNormal.TabIndex = 0;
            this.ImportNormal.Text = "ImportNormal";
            this.ImportNormal.UseVisualStyleBackColor = true;
            // 
            // ViewPanel
            // 
            this.ViewPanel.Controls.Add(this.DisplayAnimationListCheckBox);
            this.ViewPanel.Controls.Add(this.DisplayBoundingBox);
            this.ViewPanel.Controls.Add(this.DisplaySkeleton);
            this.ViewPanel.Controls.Add(this.AnimateRateValue);
            this.ViewPanel.Controls.Add(this.AnimateRateSlider);
            this.ViewPanel.Controls.Add(this.SegLengthValue);
            this.ViewPanel.Controls.Add(this.SegLengthSlider);
            this.ViewPanel.Controls.Add(this.DisplayNormal);
            this.ViewPanel.Controls.Add(this.DisplayTangent);
            this.ViewPanel.Controls.Add(this.ViewPanelOk);
            this.ViewPanel.Location = new System.Drawing.Point(804, -19);
            this.ViewPanel.Margin = new System.Windows.Forms.Padding(4);
            this.ViewPanel.Name = "ViewPanel";
            this.ViewPanel.Size = new System.Drawing.Size(296, 515);
            this.ViewPanel.TabIndex = 6;
            this.ViewPanel.Visible = false;
            // 
            // DisplayAnimationListCheckBox
            // 
            this.DisplayAnimationListCheckBox.AutoSize = true;
            this.DisplayAnimationListCheckBox.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayAnimationListCheckBox.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayAnimationListCheckBox.Location = new System.Drawing.Point(43, 249);
            this.DisplayAnimationListCheckBox.Margin = new System.Windows.Forms.Padding(4);
            this.DisplayAnimationListCheckBox.Name = "DisplayAnimationListCheckBox";
            this.DisplayAnimationListCheckBox.Size = new System.Drawing.Size(183, 24);
            this.DisplayAnimationListCheckBox.TabIndex = 20;
            this.DisplayAnimationListCheckBox.Text = "DisplayAnimationList";
            this.DisplayAnimationListCheckBox.UseVisualStyleBackColor = true;
            // 
            // DisplayBoundingBox
            // 
            this.DisplayBoundingBox.AutoSize = true;
            this.DisplayBoundingBox.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayBoundingBox.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayBoundingBox.Location = new System.Drawing.Point(43, 290);
            this.DisplayBoundingBox.Margin = new System.Windows.Forms.Padding(4);
            this.DisplayBoundingBox.Name = "DisplayBoundingBox";
            this.DisplayBoundingBox.Size = new System.Drawing.Size(180, 24);
            this.DisplayBoundingBox.TabIndex = 19;
            this.DisplayBoundingBox.Text = "DisplayBoundingBox";
            this.DisplayBoundingBox.UseVisualStyleBackColor = true;
            // 
            // DisplaySkeleton
            // 
            this.DisplaySkeleton.AutoSize = true;
            this.DisplaySkeleton.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplaySkeleton.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplaySkeleton.Location = new System.Drawing.Point(43, 270);
            this.DisplaySkeleton.Margin = new System.Windows.Forms.Padding(4);
            this.DisplaySkeleton.Name = "DisplaySkeleton";
            this.DisplaySkeleton.Size = new System.Drawing.Size(147, 24);
            this.DisplaySkeleton.TabIndex = 18;
            this.DisplaySkeleton.Text = "DisplaySkeleton";
            this.DisplaySkeleton.UseVisualStyleBackColor = true;
            // 
            // AnimateRateValue
            // 
            this.AnimateRateValue.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.AnimateRateValue.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.AnimateRateValue.Location = new System.Drawing.Point(176, 315);
            this.AnimateRateValue.Margin = new System.Windows.Forms.Padding(4);
            this.AnimateRateValue.Name = "AnimateRateValue";
            this.AnimateRateValue.ReadOnly = true;
            this.AnimateRateValue.Size = new System.Drawing.Size(67, 27);
            this.AnimateRateValue.TabIndex = 17;
            // 
            // AnimateRateSlider
            // 
            this.AnimateRateSlider.LargeChange = 10;
            this.AnimateRateSlider.Location = new System.Drawing.Point(29, 315);
            this.AnimateRateSlider.Margin = new System.Windows.Forms.Padding(4);
            this.AnimateRateSlider.Minimum = 1;
            this.AnimateRateSlider.Name = "AnimateRateSlider";
            this.AnimateRateSlider.Size = new System.Drawing.Size(139, 56);
            this.AnimateRateSlider.TabIndex = 16;
            this.AnimateRateSlider.Value = 10;
            // 
            // SegLengthValue
            // 
            this.SegLengthValue.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.SegLengthValue.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.SegLengthValue.Location = new System.Drawing.Point(176, 135);
            this.SegLengthValue.Margin = new System.Windows.Forms.Padding(4);
            this.SegLengthValue.Name = "SegLengthValue";
            this.SegLengthValue.ReadOnly = true;
            this.SegLengthValue.Size = new System.Drawing.Size(67, 27);
            this.SegLengthValue.TabIndex = 15;
            // 
            // SegLengthSlider
            // 
            this.SegLengthSlider.LargeChange = 10;
            this.SegLengthSlider.Location = new System.Drawing.Point(29, 135);
            this.SegLengthSlider.Margin = new System.Windows.Forms.Padding(4);
            this.SegLengthSlider.Minimum = 1;
            this.SegLengthSlider.Name = "SegLengthSlider";
            this.SegLengthSlider.Size = new System.Drawing.Size(139, 56);
            this.SegLengthSlider.TabIndex = 14;
            this.SegLengthSlider.Value = 10;
            // 
            // DisplayNormal
            // 
            this.DisplayNormal.AutoSize = true;
            this.DisplayNormal.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayNormal.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayNormal.Location = new System.Drawing.Point(43, 76);
            this.DisplayNormal.Margin = new System.Windows.Forms.Padding(4);
            this.DisplayNormal.Name = "DisplayNormal";
            this.DisplayNormal.Size = new System.Drawing.Size(137, 24);
            this.DisplayNormal.TabIndex = 13;
            this.DisplayNormal.Text = "DisplayNormal";
            this.DisplayNormal.UseVisualStyleBackColor = true;
            // 
            // DisplayTangent
            // 
            this.DisplayTangent.AutoSize = true;
            this.DisplayTangent.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayTangent.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayTangent.Location = new System.Drawing.Point(43, 104);
            this.DisplayTangent.Margin = new System.Windows.Forms.Padding(4);
            this.DisplayTangent.Name = "DisplayTangent";
            this.DisplayTangent.Size = new System.Drawing.Size(143, 24);
            this.DisplayTangent.TabIndex = 12;
            this.DisplayTangent.Text = "DisplayTangent";
            this.DisplayTangent.UseVisualStyleBackColor = true;
            // 
            // ViewPanelOk
            // 
            this.ViewPanelOk.Font = new System.Drawing.Font("Microsoft YaHei", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ViewPanelOk.Location = new System.Drawing.Point(112, 416);
            this.ViewPanelOk.Margin = new System.Windows.Forms.Padding(4);
            this.ViewPanelOk.Name = "ViewPanelOk";
            this.ViewPanelOk.Size = new System.Drawing.Size(81, 36);
            this.ViewPanelOk.TabIndex = 11;
            this.ViewPanelOk.Text = "确定";
            this.ViewPanelOk.UseVisualStyleBackColor = true;
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.textBox1.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox1.ForeColor = System.Drawing.SystemColors.Info;
            this.textBox1.Location = new System.Drawing.Point(0, -16);
            this.textBox1.Margin = new System.Windows.Forms.Padding(4);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(1311, 166);
            this.textBox1.TabIndex = 2;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(-1, 584);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(4);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(1312, 181);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.textBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 25);
            this.tabPage1.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(4);
            this.tabPage1.Size = new System.Drawing.Size(1304, 152);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "日志";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.MonitorPanel);
            this.tabPage2.Location = new System.Drawing.Point(4, 25);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(4);
            this.tabPage2.Size = new System.Drawing.Size(1304, 152);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "监控";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // MonitorPanel
            // 
            this.MonitorPanel.BackColor = System.Drawing.SystemColors.Desktop;
            this.MonitorPanel.Controls.Add(this.RayTestResult);
            this.MonitorPanel.Controls.Add(this.ValueRoll);
            this.MonitorPanel.Controls.Add(this.ValueYaw);
            this.MonitorPanel.Controls.Add(this.ValuePitch);
            this.MonitorPanel.Controls.Add(this.TextRoll);
            this.MonitorPanel.Controls.Add(this.TextYaw);
            this.MonitorPanel.Controls.Add(this.TextPitch);
            this.MonitorPanel.Controls.Add(this.ValuePosZ);
            this.MonitorPanel.Controls.Add(this.ValuePosY);
            this.MonitorPanel.Controls.Add(this.ValuePosX);
            this.MonitorPanel.Controls.Add(this.TextPosZ);
            this.MonitorPanel.Controls.Add(this.TextPosY);
            this.MonitorPanel.Controls.Add(this.TextPosX);
            this.MonitorPanel.Controls.Add(this.ValueTargetName);
            this.MonitorPanel.Location = new System.Drawing.Point(0, 0);
            this.MonitorPanel.Margin = new System.Windows.Forms.Padding(4);
            this.MonitorPanel.Name = "MonitorPanel";
            this.MonitorPanel.Size = new System.Drawing.Size(1297, 151);
            this.MonitorPanel.TabIndex = 0;
            // 
            // RayTestResult
            // 
            this.RayTestResult.BackColor = System.Drawing.SystemColors.Desktop;
            this.RayTestResult.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.RayTestResult.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.RayTestResult.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.RayTestResult.Location = new System.Drawing.Point(240, 14);
            this.RayTestResult.Margin = new System.Windows.Forms.Padding(4);
            this.RayTestResult.Name = "RayTestResult";
            this.RayTestResult.Size = new System.Drawing.Size(223, 20);
            this.RayTestResult.TabIndex = 13;
            // 
            // ValueRoll
            // 
            this.ValueRoll.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValueRoll.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValueRoll.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValueRoll.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValueRoll.Location = new System.Drawing.Point(321, 115);
            this.ValueRoll.Margin = new System.Windows.Forms.Padding(4);
            this.ValueRoll.Name = "ValueRoll";
            this.ValueRoll.Size = new System.Drawing.Size(133, 20);
            this.ValueRoll.TabIndex = 12;
            // 
            // ValueYaw
            // 
            this.ValueYaw.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValueYaw.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValueYaw.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValueYaw.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValueYaw.Location = new System.Drawing.Point(321, 81);
            this.ValueYaw.Margin = new System.Windows.Forms.Padding(4);
            this.ValueYaw.Name = "ValueYaw";
            this.ValueYaw.Size = new System.Drawing.Size(133, 20);
            this.ValueYaw.TabIndex = 11;
            // 
            // ValuePitch
            // 
            this.ValuePitch.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePitch.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePitch.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePitch.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePitch.Location = new System.Drawing.Point(321, 48);
            this.ValuePitch.Margin = new System.Windows.Forms.Padding(4);
            this.ValuePitch.Name = "ValuePitch";
            this.ValuePitch.Size = new System.Drawing.Size(133, 20);
            this.ValuePitch.TabIndex = 10;
            // 
            // TextRoll
            // 
            this.TextRoll.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextRoll.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextRoll.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextRoll.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextRoll.Location = new System.Drawing.Point(240, 115);
            this.TextRoll.Margin = new System.Windows.Forms.Padding(4);
            this.TextRoll.Name = "TextRoll";
            this.TextRoll.Size = new System.Drawing.Size(73, 20);
            this.TextRoll.TabIndex = 9;
            this.TextRoll.Text = "Roll";
            // 
            // TextYaw
            // 
            this.TextYaw.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextYaw.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextYaw.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextYaw.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextYaw.Location = new System.Drawing.Point(240, 81);
            this.TextYaw.Margin = new System.Windows.Forms.Padding(4);
            this.TextYaw.Name = "TextYaw";
            this.TextYaw.Size = new System.Drawing.Size(73, 20);
            this.TextYaw.TabIndex = 8;
            this.TextYaw.Text = "Yaw";
            // 
            // TextPitch
            // 
            this.TextPitch.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPitch.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPitch.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPitch.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPitch.Location = new System.Drawing.Point(240, 48);
            this.TextPitch.Margin = new System.Windows.Forms.Padding(4);
            this.TextPitch.Name = "TextPitch";
            this.TextPitch.Size = new System.Drawing.Size(73, 20);
            this.TextPitch.TabIndex = 7;
            this.TextPitch.Text = "Pitch";
            // 
            // ValuePosZ
            // 
            this.ValuePosZ.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePosZ.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePosZ.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePosZ.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePosZ.Location = new System.Drawing.Point(99, 115);
            this.ValuePosZ.Margin = new System.Windows.Forms.Padding(4);
            this.ValuePosZ.Name = "ValuePosZ";
            this.ValuePosZ.Size = new System.Drawing.Size(133, 20);
            this.ValuePosZ.TabIndex = 6;
            // 
            // ValuePosY
            // 
            this.ValuePosY.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePosY.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePosY.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePosY.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePosY.Location = new System.Drawing.Point(99, 81);
            this.ValuePosY.Margin = new System.Windows.Forms.Padding(4);
            this.ValuePosY.Name = "ValuePosY";
            this.ValuePosY.Size = new System.Drawing.Size(133, 20);
            this.ValuePosY.TabIndex = 5;
            // 
            // ValuePosX
            // 
            this.ValuePosX.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePosX.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePosX.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePosX.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePosX.Location = new System.Drawing.Point(99, 48);
            this.ValuePosX.Margin = new System.Windows.Forms.Padding(4);
            this.ValuePosX.Name = "ValuePosX";
            this.ValuePosX.Size = new System.Drawing.Size(133, 20);
            this.ValuePosX.TabIndex = 4;
            // 
            // TextPosZ
            // 
            this.TextPosZ.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPosZ.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPosZ.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPosZ.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPosZ.Location = new System.Drawing.Point(17, 115);
            this.TextPosZ.Margin = new System.Windows.Forms.Padding(4);
            this.TextPosZ.Name = "TextPosZ";
            this.TextPosZ.Size = new System.Drawing.Size(73, 20);
            this.TextPosZ.TabIndex = 3;
            this.TextPosZ.Text = "Pos Z";
            // 
            // TextPosY
            // 
            this.TextPosY.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPosY.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPosY.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPosY.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPosY.Location = new System.Drawing.Point(17, 81);
            this.TextPosY.Margin = new System.Windows.Forms.Padding(4);
            this.TextPosY.Name = "TextPosY";
            this.TextPosY.Size = new System.Drawing.Size(73, 20);
            this.TextPosY.TabIndex = 2;
            this.TextPosY.Text = "Pos Y";
            // 
            // TextPosX
            // 
            this.TextPosX.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPosX.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPosX.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPosX.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPosX.Location = new System.Drawing.Point(17, 48);
            this.TextPosX.Margin = new System.Windows.Forms.Padding(4);
            this.TextPosX.Name = "TextPosX";
            this.TextPosX.Size = new System.Drawing.Size(73, 20);
            this.TextPosX.TabIndex = 1;
            this.TextPosX.Text = "Pos X";
            // 
            // ValueTargetName
            // 
            this.ValueTargetName.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValueTargetName.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValueTargetName.Font = new System.Drawing.Font("Microsoft YaHei", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValueTargetName.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValueTargetName.Location = new System.Drawing.Point(9, 14);
            this.ValueTargetName.Margin = new System.Windows.Forms.Padding(4);
            this.ValueTargetName.Name = "ValueTargetName";
            this.ValueTargetName.Size = new System.Drawing.Size(223, 20);
            this.ValueTargetName.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.toolStripMenuItem2,
            this.toolStripMenuItem3});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(8, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(1308, 28);
            this.menuStrip1.TabIndex = 7;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.BackColor = System.Drawing.SystemColors.WindowFrame;
            this.toolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadFBXToolStripMenuItem1,
            this.LoadModelToolStripMenuItem,
            this.LoadAnimationToolStripMenuItem,
            this.LoadSceneToolStripMenuItem,
            this.SaveSceneToolStripMenuItem});
            this.toolStripMenuItem1.ForeColor = System.Drawing.SystemColors.ButtonFace;
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(30, 24);
            this.toolStripMenuItem1.Text = "$";
            // 
            // loadFBXToolStripMenuItem1
            // 
            this.loadFBXToolStripMenuItem1.BackColor = System.Drawing.SystemColors.WindowFrame;
            this.loadFBXToolStripMenuItem1.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.loadFBXToolStripMenuItem1.Name = "loadFBXToolStripMenuItem1";
            this.loadFBXToolStripMenuItem1.Size = new System.Drawing.Size(171, 26);
            this.loadFBXToolStripMenuItem1.Text = "载入FBX文件";
            // 
            // LoadModelToolStripMenuItem
            // 
            this.LoadModelToolStripMenuItem.Name = "LoadModelToolStripMenuItem";
            this.LoadModelToolStripMenuItem.Size = new System.Drawing.Size(171, 26);
            this.LoadModelToolStripMenuItem.Text = "载入模型";
            // 
            // LoadAnimationToolStripMenuItem
            // 
            this.LoadAnimationToolStripMenuItem.Name = "LoadAnimationToolStripMenuItem";
            this.LoadAnimationToolStripMenuItem.Size = new System.Drawing.Size(171, 26);
            this.LoadAnimationToolStripMenuItem.Text = "载入动画";
            // 
            // LoadSceneToolStripMenuItem
            // 
            this.LoadSceneToolStripMenuItem.Name = "LoadSceneToolStripMenuItem";
            this.LoadSceneToolStripMenuItem.Size = new System.Drawing.Size(171, 26);
            this.LoadSceneToolStripMenuItem.Text = "载入场景";
            // 
            // SaveSceneToolStripMenuItem
            // 
            this.SaveSceneToolStripMenuItem.Name = "SaveSceneToolStripMenuItem";
            this.SaveSceneToolStripMenuItem.Size = new System.Drawing.Size(171, 26);
            this.SaveSceneToolStripMenuItem.Text = "保存场景";
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ViewPanelToolStripMenuItem,
            this.ClearSceneToolStripMenuItem,
            this.RecordConsoleTSMI,
            this.SwitchConsoleTSMI});
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(34, 24);
            this.toolStripMenuItem2.Text = "%";
            // 
            // ViewPanelToolStripMenuItem
            // 
            this.ViewPanelToolStripMenuItem.Name = "ViewPanelToolStripMenuItem";
            this.ViewPanelToolStripMenuItem.Size = new System.Drawing.Size(159, 26);
            this.ViewPanelToolStripMenuItem.Text = "显示选项";
            // 
            // ClearSceneToolStripMenuItem
            // 
            this.ClearSceneToolStripMenuItem.Name = "ClearSceneToolStripMenuItem";
            this.ClearSceneToolStripMenuItem.Size = new System.Drawing.Size(159, 26);
            this.ClearSceneToolStripMenuItem.Text = "清空场景";
            // 
            // RecordConsoleTSMI
            // 
            this.RecordConsoleTSMI.Name = "RecordConsoleTSMI";
            this.RecordConsoleTSMI.Size = new System.Drawing.Size(159, 26);
            this.RecordConsoleTSMI.Text = "记录控制台";
            // 
            // SwitchConsoleTSMI
            // 
            this.SwitchConsoleTSMI.Name = "SwitchConsoleTSMI";
            this.SwitchConsoleTSMI.Size = new System.Drawing.Size(159, 26);
            this.SwitchConsoleTSMI.Text = "切换控制台";
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem4});
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(28, 24);
            this.toolStripMenuItem3.Text = "*";
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripComboBox1});
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(233, 26);
            this.toolStripMenuItem4.Text = "toolStripMenuItem4";
            // 
            // toolStripComboBox1
            // 
            this.toolStripComboBox1.Name = "toolStripComboBox1";
            this.toolStripComboBox1.Size = new System.Drawing.Size(121, 28);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.ClientSize = new System.Drawing.Size(1308, 755);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.RenderPanel);
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Closed += new System.EventHandler(this.Form1_Closed);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_Closed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.RenderPanel.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ImportPanel.ResumeLayout(false);
            this.ImportPanel.PerformLayout();
            this.ViewPanel.ResumeLayout(false);
            this.ViewPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AnimateRateSlider)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SegLengthSlider)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.MonitorPanel.ResumeLayout(false);
            this.MonitorPanel.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel RenderPanel;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Panel ImportPanel;
        private System.Windows.Forms.CheckBox ImportNormal;
        private System.Windows.Forms.CheckBox ImportAnimation;
        private System.Windows.Forms.CheckBox ImportVertexColor;
        private System.Windows.Forms.CheckBox GenerateTangentIfNotFound;
        private System.Windows.Forms.CheckBox ForceRegenerateTangent;
        private System.Windows.Forms.CheckBox ImportTangent;
        private System.Windows.Forms.CheckBox GenerateNormalIfNotFound;
        private System.Windows.Forms.CheckBox ForceRegenerateNormal;
        private System.Windows.Forms.Button ImportCancel;
        private System.Windows.Forms.Button ImportOk;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem loadFBXToolStripMenuItem1;
        private System.Windows.Forms.CheckBox ImportTexCoord;
        private System.Windows.Forms.CheckBox MergeNormalTangentAll;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem ViewPanelToolStripMenuItem;
        private System.Windows.Forms.Panel ViewPanel;
        private System.Windows.Forms.Button ViewPanelOk;
        private System.Windows.Forms.TrackBar SegLengthSlider;
        private System.Windows.Forms.CheckBox DisplayNormal;
        private System.Windows.Forms.CheckBox DisplayTangent;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.TextBox SegLengthValue;
        private System.Windows.Forms.TextBox AnimateRateValue;
        private System.Windows.Forms.TrackBar AnimateRateSlider;
        private System.Windows.Forms.CheckBox DisplaySkeleton;
        private System.Windows.Forms.CheckBox DisplayBoundingBox;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem4;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBox1;
        private System.Windows.Forms.ToolStripMenuItem LoadModelToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem LoadAnimationToolStripMenuItem;
        private System.Windows.Forms.TextBox AnimationOutputText;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox PrimitiveOutputText;
        private System.Windows.Forms.ToolStripMenuItem ClearSceneToolStripMenuItem;
        private System.Windows.Forms.Panel MonitorPanel;
        private System.Windows.Forms.TextBox ValueRoll;
        private System.Windows.Forms.TextBox ValueYaw;
        private System.Windows.Forms.TextBox ValuePitch;
        private System.Windows.Forms.TextBox TextRoll;
        private System.Windows.Forms.TextBox TextYaw;
        private System.Windows.Forms.TextBox TextPitch;
        private System.Windows.Forms.TextBox ValuePosZ;
        private System.Windows.Forms.TextBox ValuePosY;
        private System.Windows.Forms.TextBox ValuePosX;
        private System.Windows.Forms.TextBox TextPosZ;
        private System.Windows.Forms.TextBox TextPosY;
        private System.Windows.Forms.TextBox TextPosX;
        private System.Windows.Forms.TextBox ValueTargetName;
        private System.Windows.Forms.ToolStripMenuItem LoadSceneToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem SaveSceneToolStripMenuItem;
        private System.Windows.Forms.TextBox RayTestResult;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.CheckBox DisplayAnimationListCheckBox;
        private System.Windows.Forms.ToolStripMenuItem RecordConsoleTSMI;
        private System.Windows.Forms.ToolStripMenuItem SwitchConsoleTSMI;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.TextBox textBox4;
    }
}

