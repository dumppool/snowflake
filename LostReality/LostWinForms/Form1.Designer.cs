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
            this.panel1 = new System.Windows.Forms.Panel();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.ViewPanel = new System.Windows.Forms.Panel();
            this.DisplaySkeleton2 = new System.Windows.Forms.CheckBox();
            this.DisplaySkeleton = new System.Windows.Forms.CheckBox();
            this.AnimateRateValue = new System.Windows.Forms.TextBox();
            this.AnimateRateSlider = new System.Windows.Forms.TrackBar();
            this.SegLengthValue = new System.Windows.Forms.TextBox();
            this.SegLengthSlider = new System.Windows.Forms.TrackBar();
            this.DisplayNormal = new System.Windows.Forms.CheckBox();
            this.DisplayTangent = new System.Windows.Forms.CheckBox();
            this.ViewPanelOk = new System.Windows.Forms.Button();
            this.ImportPanel = new System.Windows.Forms.Panel();
            this.MergeNormalTangentAll = new System.Windows.Forms.CheckBox();
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
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.loadFBXToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
            this.ViewPanelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripComboBox1 = new System.Windows.Forms.ToolStripComboBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.载入模型ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.载入动画ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.AnimationOutputText = new System.Windows.Forms.TextBox();
            this.PrimitiveOutputText = new System.Windows.Forms.TextBox();
            this.panel1.SuspendLayout();
            this.ViewPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AnimateRateSlider)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SegLengthSlider)).BeginInit();
            this.ImportPanel.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.panel1.Controls.Add(this.listBox1);
            this.panel1.Controls.Add(this.ImportPanel);
            this.panel1.Controls.Add(this.ViewPanel);
            this.panel1.Location = new System.Drawing.Point(-1, 59);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(984, 406);
            this.panel1.TabIndex = 0;
            this.panel1.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.Panel1_PreviewKeyDown);
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
            // ViewPanel
            // 
            this.ViewPanel.Controls.Add(this.DisplaySkeleton2);
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
            // DisplaySkeleton2
            // 
            this.DisplaySkeleton2.AutoSize = true;
            this.DisplaySkeleton2.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.DisplaySkeleton2.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            this.DisplaySkeleton2.Location = new System.Drawing.Point(32, 232);
            this.DisplaySkeleton2.Name = "DisplaySkeleton2";
            this.DisplaySkeleton2.Size = new System.Drawing.Size(126, 21);
            this.DisplaySkeleton2.TabIndex = 19;
            this.DisplaySkeleton2.Text = "DisplaySkeleton2";
            this.DisplaySkeleton2.UseVisualStyleBackColor = true;
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
            this.tabControl1.Location = new System.Drawing.Point(-1, 462);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(984, 150);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.textBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(976, 124);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "日志";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.richTextBox1);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(976, 124);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "监控";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // richTextBox1
            // 
            this.richTextBox1.BackColor = System.Drawing.SystemColors.WindowFrame;
            this.richTextBox1.ForeColor = System.Drawing.SystemColors.Window;
            this.richTextBox1.Location = new System.Drawing.Point(74, 6);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(782, 119);
            this.richTextBox1.TabIndex = 0;
            this.richTextBox1.Text = "";
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
            this.载入模型ToolStripMenuItem,
            this.载入动画ToolStripMenuItem});
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
            this.loadFBXToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.loadFBXToolStripMenuItem1.Text = "载入FBX文件";
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ViewPanelToolStripMenuItem});
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
            // 载入模型ToolStripMenuItem
            // 
            this.载入模型ToolStripMenuItem.Name = "载入模型ToolStripMenuItem";
            this.载入模型ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.载入模型ToolStripMenuItem.Text = "载入模型";
            // 
            // 载入动画ToolStripMenuItem
            // 
            this.载入动画ToolStripMenuItem.Name = "载入动画ToolStripMenuItem";
            this.载入动画ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.载入动画ToolStripMenuItem.Text = "载入动画";
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
            // AnimationOutputText
            // 
            this.AnimationOutputText.Location = new System.Drawing.Point(118, 48);
            this.AnimationOutputText.Name = "AnimationOutputText";
            this.AnimationOutputText.Size = new System.Drawing.Size(122, 21);
            this.AnimationOutputText.TabIndex = 15;
            // 
            // PrimitiveOutputText
            // 
            this.PrimitiveOutputText.Location = new System.Drawing.Point(118, 17);
            this.PrimitiveOutputText.Name = "PrimitiveOutputText";
            this.PrimitiveOutputText.Size = new System.Drawing.Size(122, 21);
            this.PrimitiveOutputText.TabIndex = 14;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.ClientSize = new System.Drawing.Size(981, 604);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.panel1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.Closed += new System.EventHandler(this.Form1_Closed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.ViewPanel.ResumeLayout(false);
            this.ViewPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.AnimateRateSlider)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SegLengthSlider)).EndInit();
            this.ImportPanel.ResumeLayout(false);
            this.ImportPanel.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.RichTextBox richTextBox1;
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
        private System.Windows.Forms.CheckBox DisplaySkeleton2;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem4;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBox1;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.ToolStripMenuItem 载入模型ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 载入动画ToolStripMenuItem;
        private System.Windows.Forms.TextBox AnimationOutputText;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox PrimitiveOutputText;
    }
}

