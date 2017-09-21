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
            this.RenderPanel = new System.Windows.Forms.Panel();
            this.listBox1 = new System.Windows.Forms.ListBox();
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
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripComboBox1 = new System.Windows.Forms.ToolStripComboBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.RenderPanel.SuspendLayout();
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
            this.RenderPanel.Controls.Add(this.listBox1);
            this.RenderPanel.Controls.Add(this.ImportPanel);
            this.RenderPanel.Controls.Add(this.ViewPanel);
            this.RenderPanel.Location = new System.Drawing.Point(-1, 59);
            this.RenderPanel.Name = "RenderPanel";
            this.RenderPanel.Size = new System.Drawing.Size(984, 406);
            this.RenderPanel.TabIndex = 0;
            this.RenderPanel.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.Panel1_PreviewKeyDown);
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(58, 60);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(120, 220);
            this.listBox1.TabIndex = 7;
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
            this.ImportPanel.Location = new System.Drawing.Point(260, 17);
            this.ImportPanel.Name = "ImportPanel";
            this.ImportPanel.Size = new System.Drawing.Size(243, 411);
            this.ImportPanel.TabIndex = 5;
            this.ImportPanel.Visible = false;
            // 
            // AnimationOutputText
            // 
            this.AnimationOutputText.Location = new System.Drawing.Point(118, 48);
            this.AnimationOutputText.Name = "AnimationOutputText";
            this.AnimationOutputText.Size = new System.Drawing.Size(122, 21);
            this.AnimationOutputText.TabIndex = 15;
            // 
            // textBox3
            // 
            this.textBox3.BackColor = System.Drawing.SystemColors.Desktop;
            this.textBox3.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox3.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox3.ForeColor = System.Drawing.Color.Silver;
            this.textBox3.Location = new System.Drawing.Point(11, 51);
            this.textBox3.Name = "textBox3";
            this.textBox3.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.textBox3.Size = new System.Drawing.Size(98, 16);
            this.textBox3.TabIndex = 13;
            this.textBox3.Text = "animation文件夹";
            // 
            // textBox2
            // 
            this.textBox2.BackColor = System.Drawing.SystemColors.Desktop;
            this.textBox2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox2.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox2.ForeColor = System.Drawing.Color.Silver;
            this.textBox2.Location = new System.Drawing.Point(11, 20);
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.textBox2.Size = new System.Drawing.Size(98, 16);
            this.textBox2.TabIndex = 12;
            this.textBox2.Text = "primitive文件夹";
            // 
            // MergeNormalTangentAll
            // 
            this.MergeNormalTangentAll.AutoSize = true;
            this.MergeNormalTangentAll.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.MergeNormalTangentAll.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.MergeNormalTangentAll.Location = new System.Drawing.Point(25, 78);
            this.MergeNormalTangentAll.Name = "MergeNormalTangentAll";
            this.MergeNormalTangentAll.Size = new System.Drawing.Size(171, 21);
            this.MergeNormalTangentAll.TabIndex = 11;
            this.MergeNormalTangentAll.Text = "MergeNormalTangentAll";
            this.MergeNormalTangentAll.UseVisualStyleBackColor = true;
            // 
            // PrimitiveOutputText
            // 
            this.PrimitiveOutputText.Location = new System.Drawing.Point(118, 17);
            this.PrimitiveOutputText.Name = "PrimitiveOutputText";
            this.PrimitiveOutputText.Size = new System.Drawing.Size(122, 21);
            this.PrimitiveOutputText.TabIndex = 14;
            // 
            // ImportTexCoord
            // 
            this.ImportTexCoord.AutoSize = true;
            this.ImportTexCoord.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportTexCoord.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportTexCoord.Location = new System.Drawing.Point(25, 257);
            this.ImportTexCoord.Name = "ImportTexCoord";
            this.ImportTexCoord.Size = new System.Drawing.Size(124, 21);
            this.ImportTexCoord.TabIndex = 10;
            this.ImportTexCoord.Text = "ImportTexCoord";
            this.ImportTexCoord.UseVisualStyleBackColor = true;
            // 
            // ImportCancel
            // 
            this.ImportCancel.Font = new System.Drawing.Font("微软雅黑", 7.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportCancel.Location = new System.Drawing.Point(142, 362);
            this.ImportCancel.Name = "ImportCancel";
            this.ImportCancel.Size = new System.Drawing.Size(53, 23);
            this.ImportCancel.TabIndex = 9;
            this.ImportCancel.Text = "Cancel";
            this.ImportCancel.UseVisualStyleBackColor = true;
            // 
            // ImportOk
            // 
            this.ImportOk.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportOk.Location = new System.Drawing.Point(25, 356);
            this.ImportOk.Name = "ImportOk";
            this.ImportOk.Size = new System.Drawing.Size(109, 29);
            this.ImportOk.TabIndex = 8;
            this.ImportOk.Text = "Import";
            this.ImportOk.UseVisualStyleBackColor = true;
            // 
            // ImportAnimation
            // 
            this.ImportAnimation.AutoSize = true;
            this.ImportAnimation.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportAnimation.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportAnimation.Location = new System.Drawing.Point(25, 284);
            this.ImportAnimation.Name = "ImportAnimation";
            this.ImportAnimation.Size = new System.Drawing.Size(125, 21);
            this.ImportAnimation.TabIndex = 7;
            this.ImportAnimation.Text = "ImportAnimation";
            this.ImportAnimation.UseVisualStyleBackColor = true;
            // 
            // ImportVertexColor
            // 
            this.ImportVertexColor.AutoSize = true;
            this.ImportVertexColor.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportVertexColor.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportVertexColor.Location = new System.Drawing.Point(25, 311);
            this.ImportVertexColor.Name = "ImportVertexColor";
            this.ImportVertexColor.Size = new System.Drawing.Size(136, 21);
            this.ImportVertexColor.TabIndex = 6;
            this.ImportVertexColor.Text = "ImportVertexColor";
            this.ImportVertexColor.UseVisualStyleBackColor = true;
            // 
            // GenerateTangentIfNotFound
            // 
            this.GenerateTangentIfNotFound.AutoSize = true;
            this.GenerateTangentIfNotFound.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.GenerateTangentIfNotFound.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.GenerateTangentIfNotFound.Location = new System.Drawing.Point(25, 220);
            this.GenerateTangentIfNotFound.Name = "GenerateTangentIfNotFound";
            this.GenerateTangentIfNotFound.Size = new System.Drawing.Size(193, 21);
            this.GenerateTangentIfNotFound.TabIndex = 5;
            this.GenerateTangentIfNotFound.Text = "GenerateTangentIfNotFound";
            this.GenerateTangentIfNotFound.UseVisualStyleBackColor = true;
            // 
            // ForceRegenerateTangent
            // 
            this.ForceRegenerateTangent.AutoSize = true;
            this.ForceRegenerateTangent.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ForceRegenerateTangent.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ForceRegenerateTangent.Location = new System.Drawing.Point(25, 198);
            this.ForceRegenerateTangent.Name = "ForceRegenerateTangent";
            this.ForceRegenerateTangent.Size = new System.Drawing.Size(173, 21);
            this.ForceRegenerateTangent.TabIndex = 4;
            this.ForceRegenerateTangent.Text = "ForceRegenerateTangent";
            this.ForceRegenerateTangent.UseVisualStyleBackColor = true;
            // 
            // ImportTangent
            // 
            this.ImportTangent.AutoSize = true;
            this.ImportTangent.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportTangent.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportTangent.Location = new System.Drawing.Point(25, 176);
            this.ImportTangent.Name = "ImportTangent";
            this.ImportTangent.Size = new System.Drawing.Size(114, 21);
            this.ImportTangent.TabIndex = 3;
            this.ImportTangent.Text = "ImportTangent";
            this.ImportTangent.UseVisualStyleBackColor = true;
            // 
            // GenerateNormalIfNotFound
            // 
            this.GenerateNormalIfNotFound.AutoSize = true;
            this.GenerateNormalIfNotFound.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.GenerateNormalIfNotFound.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.GenerateNormalIfNotFound.Location = new System.Drawing.Point(25, 149);
            this.GenerateNormalIfNotFound.Name = "GenerateNormalIfNotFound";
            this.GenerateNormalIfNotFound.Size = new System.Drawing.Size(190, 21);
            this.GenerateNormalIfNotFound.TabIndex = 2;
            this.GenerateNormalIfNotFound.Text = "GenerateNormalIfNotFound";
            this.GenerateNormalIfNotFound.UseVisualStyleBackColor = true;
            // 
            // ForceRegenerateNormal
            // 
            this.ForceRegenerateNormal.AutoSize = true;
            this.ForceRegenerateNormal.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ForceRegenerateNormal.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ForceRegenerateNormal.Location = new System.Drawing.Point(25, 127);
            this.ForceRegenerateNormal.Name = "ForceRegenerateNormal";
            this.ForceRegenerateNormal.Size = new System.Drawing.Size(170, 21);
            this.ForceRegenerateNormal.TabIndex = 1;
            this.ForceRegenerateNormal.Text = "ForceRegenerateNormal";
            this.ForceRegenerateNormal.UseVisualStyleBackColor = true;
            // 
            // ImportNormal
            // 
            this.ImportNormal.AutoSize = true;
            this.ImportNormal.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ImportNormal.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.ImportNormal.Location = new System.Drawing.Point(25, 105);
            this.ImportNormal.Name = "ImportNormal";
            this.ImportNormal.Size = new System.Drawing.Size(111, 21);
            this.ImportNormal.TabIndex = 0;
            this.ImportNormal.Text = "ImportNormal";
            this.ImportNormal.UseVisualStyleBackColor = true;
            // 
            // ViewPanel
            // 
            this.ViewPanel.Controls.Add(this.DisplayBoundingBox);
            this.ViewPanel.Controls.Add(this.DisplaySkeleton);
            this.ViewPanel.Controls.Add(this.AnimateRateValue);
            this.ViewPanel.Controls.Add(this.AnimateRateSlider);
            this.ViewPanel.Controls.Add(this.SegLengthValue);
            this.ViewPanel.Controls.Add(this.SegLengthSlider);
            this.ViewPanel.Controls.Add(this.DisplayNormal);
            this.ViewPanel.Controls.Add(this.DisplayTangent);
            this.ViewPanel.Controls.Add(this.ViewPanelOk);
            this.ViewPanel.Location = new System.Drawing.Point(603, -15);
            this.ViewPanel.Name = "ViewPanel";
            this.ViewPanel.Size = new System.Drawing.Size(222, 412);
            this.ViewPanel.TabIndex = 6;
            this.ViewPanel.Visible = false;
            // 
            // DisplayBoundingBox
            // 
            this.DisplayBoundingBox.AutoSize = true;
            this.DisplayBoundingBox.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayBoundingBox.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayBoundingBox.Location = new System.Drawing.Point(32, 232);
            this.DisplayBoundingBox.Name = "DisplayBoundingBox";
            this.DisplayBoundingBox.Size = new System.Drawing.Size(147, 21);
            this.DisplayBoundingBox.TabIndex = 19;
            this.DisplayBoundingBox.Text = "DisplayBoundingBox";
            this.DisplayBoundingBox.UseVisualStyleBackColor = true;
            // 
            // DisplaySkeleton
            // 
            this.DisplaySkeleton.AutoSize = true;
            this.DisplaySkeleton.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplaySkeleton.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplaySkeleton.Location = new System.Drawing.Point(32, 216);
            this.DisplaySkeleton.Name = "DisplaySkeleton";
            this.DisplaySkeleton.Size = new System.Drawing.Size(119, 21);
            this.DisplaySkeleton.TabIndex = 18;
            this.DisplaySkeleton.Text = "DisplaySkeleton";
            this.DisplaySkeleton.UseVisualStyleBackColor = true;
            // 
            // AnimateRateValue
            // 
            this.AnimateRateValue.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.AnimateRateValue.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.AnimateRateValue.Location = new System.Drawing.Point(132, 181);
            this.AnimateRateValue.Name = "AnimateRateValue";
            this.AnimateRateValue.ReadOnly = true;
            this.AnimateRateValue.Size = new System.Drawing.Size(51, 23);
            this.AnimateRateValue.TabIndex = 17;
            // 
            // AnimateRateSlider
            // 
            this.AnimateRateSlider.LargeChange = 10;
            this.AnimateRateSlider.Location = new System.Drawing.Point(22, 181);
            this.AnimateRateSlider.Minimum = 1;
            this.AnimateRateSlider.Name = "AnimateRateSlider";
            this.AnimateRateSlider.Size = new System.Drawing.Size(104, 45);
            this.AnimateRateSlider.TabIndex = 16;
            this.AnimateRateSlider.Value = 10;
            // 
            // SegLengthValue
            // 
            this.SegLengthValue.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.SegLengthValue.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.SegLengthValue.Location = new System.Drawing.Point(132, 115);
            this.SegLengthValue.Name = "SegLengthValue";
            this.SegLengthValue.ReadOnly = true;
            this.SegLengthValue.Size = new System.Drawing.Size(51, 23);
            this.SegLengthValue.TabIndex = 15;
            // 
            // SegLengthSlider
            // 
            this.SegLengthSlider.LargeChange = 10;
            this.SegLengthSlider.Location = new System.Drawing.Point(22, 115);
            this.SegLengthSlider.Minimum = 1;
            this.SegLengthSlider.Name = "SegLengthSlider";
            this.SegLengthSlider.Size = new System.Drawing.Size(104, 45);
            this.SegLengthSlider.TabIndex = 14;
            this.SegLengthSlider.Value = 10;
            // 
            // DisplayNormal
            // 
            this.DisplayNormal.AutoSize = true;
            this.DisplayNormal.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayNormal.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayNormal.Location = new System.Drawing.Point(32, 61);
            this.DisplayNormal.Name = "DisplayNormal";
            this.DisplayNormal.Size = new System.Drawing.Size(113, 21);
            this.DisplayNormal.TabIndex = 13;
            this.DisplayNormal.Text = "DisplayNormal";
            this.DisplayNormal.UseVisualStyleBackColor = true;
            // 
            // DisplayTangent
            // 
            this.DisplayTangent.AutoSize = true;
            this.DisplayTangent.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplayTangent.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplayTangent.Location = new System.Drawing.Point(32, 88);
            this.DisplayTangent.Name = "DisplayTangent";
            this.DisplayTangent.Size = new System.Drawing.Size(116, 21);
            this.DisplayTangent.TabIndex = 12;
            this.DisplayTangent.Text = "DisplayTangent";
            this.DisplayTangent.UseVisualStyleBackColor = true;
            // 
            // ViewPanelOk
            // 
            this.ViewPanelOk.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ViewPanelOk.Location = new System.Drawing.Point(84, 333);
            this.ViewPanelOk.Name = "ViewPanelOk";
            this.ViewPanelOk.Size = new System.Drawing.Size(61, 29);
            this.ViewPanelOk.TabIndex = 11;
            this.ViewPanelOk.Text = "确定";
            this.ViewPanelOk.UseVisualStyleBackColor = true;
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.textBox1.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.textBox1.ForeColor = System.Drawing.SystemColors.Info;
            this.textBox1.Location = new System.Drawing.Point(0, -13);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(984, 134);
            this.textBox1.TabIndex = 2;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(-1, 467);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(984, 145);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.textBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(976, 119);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "日志";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.MonitorPanel);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(976, 119);
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
            this.MonitorPanel.Name = "MonitorPanel";
            this.MonitorPanel.Size = new System.Drawing.Size(973, 121);
            this.MonitorPanel.TabIndex = 0;
            // 
            // RayTestResult
            // 
            this.RayTestResult.BackColor = System.Drawing.SystemColors.Desktop;
            this.RayTestResult.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.RayTestResult.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.RayTestResult.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.RayTestResult.Location = new System.Drawing.Point(180, 11);
            this.RayTestResult.Name = "RayTestResult";
            this.RayTestResult.Size = new System.Drawing.Size(167, 16);
            this.RayTestResult.TabIndex = 13;
            // 
            // ValueRoll
            // 
            this.ValueRoll.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValueRoll.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValueRoll.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValueRoll.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValueRoll.Location = new System.Drawing.Point(241, 92);
            this.ValueRoll.Name = "ValueRoll";
            this.ValueRoll.Size = new System.Drawing.Size(100, 16);
            this.ValueRoll.TabIndex = 12;
            // 
            // ValueYaw
            // 
            this.ValueYaw.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValueYaw.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValueYaw.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValueYaw.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValueYaw.Location = new System.Drawing.Point(241, 65);
            this.ValueYaw.Name = "ValueYaw";
            this.ValueYaw.Size = new System.Drawing.Size(100, 16);
            this.ValueYaw.TabIndex = 11;
            // 
            // ValuePitch
            // 
            this.ValuePitch.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePitch.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePitch.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePitch.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePitch.Location = new System.Drawing.Point(241, 38);
            this.ValuePitch.Name = "ValuePitch";
            this.ValuePitch.Size = new System.Drawing.Size(100, 16);
            this.ValuePitch.TabIndex = 10;
            // 
            // TextRoll
            // 
            this.TextRoll.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextRoll.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextRoll.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextRoll.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextRoll.Location = new System.Drawing.Point(180, 92);
            this.TextRoll.Name = "TextRoll";
            this.TextRoll.Size = new System.Drawing.Size(55, 16);
            this.TextRoll.TabIndex = 9;
            this.TextRoll.Text = "Roll";
            // 
            // TextYaw
            // 
            this.TextYaw.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextYaw.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextYaw.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextYaw.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextYaw.Location = new System.Drawing.Point(180, 65);
            this.TextYaw.Name = "TextYaw";
            this.TextYaw.Size = new System.Drawing.Size(55, 16);
            this.TextYaw.TabIndex = 8;
            this.TextYaw.Text = "Yaw";
            // 
            // TextPitch
            // 
            this.TextPitch.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPitch.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPitch.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPitch.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPitch.Location = new System.Drawing.Point(180, 38);
            this.TextPitch.Name = "TextPitch";
            this.TextPitch.Size = new System.Drawing.Size(55, 16);
            this.TextPitch.TabIndex = 7;
            this.TextPitch.Text = "Pitch";
            // 
            // ValuePosZ
            // 
            this.ValuePosZ.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePosZ.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePosZ.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePosZ.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePosZ.Location = new System.Drawing.Point(74, 92);
            this.ValuePosZ.Name = "ValuePosZ";
            this.ValuePosZ.Size = new System.Drawing.Size(100, 16);
            this.ValuePosZ.TabIndex = 6;
            // 
            // ValuePosY
            // 
            this.ValuePosY.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePosY.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePosY.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePosY.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePosY.Location = new System.Drawing.Point(74, 65);
            this.ValuePosY.Name = "ValuePosY";
            this.ValuePosY.Size = new System.Drawing.Size(100, 16);
            this.ValuePosY.TabIndex = 5;
            // 
            // ValuePosX
            // 
            this.ValuePosX.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValuePosX.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValuePosX.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValuePosX.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValuePosX.Location = new System.Drawing.Point(74, 38);
            this.ValuePosX.Name = "ValuePosX";
            this.ValuePosX.Size = new System.Drawing.Size(100, 16);
            this.ValuePosX.TabIndex = 4;
            // 
            // TextPosZ
            // 
            this.TextPosZ.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPosZ.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPosZ.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPosZ.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPosZ.Location = new System.Drawing.Point(13, 92);
            this.TextPosZ.Name = "TextPosZ";
            this.TextPosZ.Size = new System.Drawing.Size(55, 16);
            this.TextPosZ.TabIndex = 3;
            this.TextPosZ.Text = "Pos Z";
            // 
            // TextPosY
            // 
            this.TextPosY.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPosY.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPosY.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPosY.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPosY.Location = new System.Drawing.Point(13, 65);
            this.TextPosY.Name = "TextPosY";
            this.TextPosY.Size = new System.Drawing.Size(55, 16);
            this.TextPosY.TabIndex = 2;
            this.TextPosY.Text = "Pos Y";
            // 
            // TextPosX
            // 
            this.TextPosX.BackColor = System.Drawing.SystemColors.Desktop;
            this.TextPosX.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TextPosX.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextPosX.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.TextPosX.Location = new System.Drawing.Point(13, 38);
            this.TextPosX.Name = "TextPosX";
            this.TextPosX.Size = new System.Drawing.Size(55, 16);
            this.TextPosX.TabIndex = 1;
            this.TextPosX.Text = "Pos X";
            // 
            // ValueTargetName
            // 
            this.ValueTargetName.BackColor = System.Drawing.SystemColors.Desktop;
            this.ValueTargetName.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.ValueTargetName.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.ValueTargetName.ForeColor = System.Drawing.SystemColors.ScrollBar;
            this.ValueTargetName.Location = new System.Drawing.Point(7, 11);
            this.ValueTargetName.Name = "ValueTargetName";
            this.ValueTargetName.Size = new System.Drawing.Size(167, 16);
            this.ValueTargetName.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.toolStripMenuItem2,
            this.toolStripMenuItem3});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(981, 25);
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
            this.toolStripMenuItem1.Size = new System.Drawing.Size(27, 21);
            this.toolStripMenuItem1.Text = "$";
            // 
            // loadFBXToolStripMenuItem1
            // 
            this.loadFBXToolStripMenuItem1.BackColor = System.Drawing.SystemColors.WindowFrame;
            this.loadFBXToolStripMenuItem1.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.loadFBXToolStripMenuItem1.Name = "loadFBXToolStripMenuItem1";
            this.loadFBXToolStripMenuItem1.Size = new System.Drawing.Size(146, 22);
            this.loadFBXToolStripMenuItem1.Text = "载入FBX文件";
            // 
            // LoadModelToolStripMenuItem
            // 
            this.LoadModelToolStripMenuItem.Name = "LoadModelToolStripMenuItem";
            this.LoadModelToolStripMenuItem.Size = new System.Drawing.Size(146, 22);
            this.LoadModelToolStripMenuItem.Text = "载入模型";
            // 
            // LoadAnimationToolStripMenuItem
            // 
            this.LoadAnimationToolStripMenuItem.Name = "LoadAnimationToolStripMenuItem";
            this.LoadAnimationToolStripMenuItem.Size = new System.Drawing.Size(146, 22);
            this.LoadAnimationToolStripMenuItem.Text = "载入动画";
            // 
            // LoadSceneToolStripMenuItem
            // 
            this.LoadSceneToolStripMenuItem.Name = "LoadSceneToolStripMenuItem";
            this.LoadSceneToolStripMenuItem.Size = new System.Drawing.Size(146, 22);
            this.LoadSceneToolStripMenuItem.Text = "载入场景";
            // 
            // SaveSceneToolStripMenuItem
            // 
            this.SaveSceneToolStripMenuItem.Name = "SaveSceneToolStripMenuItem";
            this.SaveSceneToolStripMenuItem.Size = new System.Drawing.Size(146, 22);
            this.SaveSceneToolStripMenuItem.Text = "保存场景";
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ViewPanelToolStripMenuItem,
            this.ClearSceneToolStripMenuItem});
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(31, 21);
            this.toolStripMenuItem2.Text = "%";
            // 
            // ViewPanelToolStripMenuItem
            // 
            this.ViewPanelToolStripMenuItem.Name = "ViewPanelToolStripMenuItem";
            this.ViewPanelToolStripMenuItem.Size = new System.Drawing.Size(124, 22);
            this.ViewPanelToolStripMenuItem.Text = "显示选项";
            // 
            // ClearSceneToolStripMenuItem
            // 
            this.ClearSceneToolStripMenuItem.Name = "ClearSceneToolStripMenuItem";
            this.ClearSceneToolStripMenuItem.Size = new System.Drawing.Size(124, 22);
            this.ClearSceneToolStripMenuItem.Text = "清空场景";
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem4});
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(25, 21);
            this.toolStripMenuItem3.Text = "*";
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripComboBox1});
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(192, 22);
            this.toolStripMenuItem4.Text = "toolStripMenuItem4";
            // 
            // toolStripComboBox1
            // 
            this.toolStripComboBox1.Name = "toolStripComboBox1";
            this.toolStripComboBox1.Size = new System.Drawing.Size(121, 25);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.ClientSize = new System.Drawing.Size(981, 604);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.RenderPanel);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.Closed += new System.EventHandler(this.Form1_Closed);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_Closed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.RenderPanel.ResumeLayout(false);
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
        private System.Windows.Forms.ListBox listBox1;
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
    }
}

