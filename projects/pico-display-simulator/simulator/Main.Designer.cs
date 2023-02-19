namespace Simulator
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.StatusStrip = new System.Windows.Forms.StatusStrip();
            this.StatusStripLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.LcdPanel = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.AButton = new System.Windows.Forms.Button();
            this.BButton = new System.Windows.Forms.Button();
            this.XButton = new System.Windows.Forms.Button();
            this.YButton = new System.Windows.Forms.Button();
            this.RbbLedOutline = new System.Windows.Forms.Panel();
            this.RgbLED = new System.Windows.Forms.Panel();
            this.Title = new System.Windows.Forms.Label();
            this.StatusStrip.SuspendLayout();
            this.panel1.SuspendLayout();
            this.RbbLedOutline.SuspendLayout();
            this.SuspendLayout();
            // 
            // StatusStrip
            // 
            this.StatusStrip.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.StatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.StatusStripLabel});
            this.StatusStrip.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.Flow;
            this.StatusStrip.Location = new System.Drawing.Point(0, 296);
            this.StatusStrip.Name = "StatusStrip";
            this.StatusStrip.Padding = new System.Windows.Forms.Padding(1, 0, 19, 0);
            this.StatusStrip.Size = new System.Drawing.Size(459, 6);
            this.StatusStrip.TabIndex = 0;
            this.StatusStrip.Text = "Hello";
            // 
            // StatusStripLabel
            // 
            this.StatusStripLabel.Name = "StatusStripLabel";
            this.StatusStripLabel.Size = new System.Drawing.Size(0, 0);
            // 
            // LcdPanel
            // 
            this.LcdPanel.Location = new System.Drawing.Point(4, 4);
            this.LcdPanel.Margin = new System.Windows.Forms.Padding(0);
            this.LcdPanel.Name = "LcdPanel";
            this.LcdPanel.Size = new System.Drawing.Size(240, 135);
            this.LcdPanel.TabIndex = 1;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.Window;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Controls.Add(this.LcdPanel);
            this.panel1.Location = new System.Drawing.Point(98, 74);
            this.panel1.Margin = new System.Windows.Forms.Padding(4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(252, 147);
            this.panel1.TabIndex = 3;
            // 
            // AButton
            // 
            this.AButton.Location = new System.Drawing.Point(46, 83);
            this.AButton.Margin = new System.Windows.Forms.Padding(4);
            this.AButton.Name = "AButton";
            this.AButton.Size = new System.Drawing.Size(42, 28);
            this.AButton.TabIndex = 4;
            this.AButton.Text = "A";
            this.AButton.UseVisualStyleBackColor = true;
            this.AButton.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AButton_MouseDown);
            this.AButton.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AButton_MouseUp);
            // 
            // BButton
            // 
            this.BButton.Location = new System.Drawing.Point(46, 169);
            this.BButton.Margin = new System.Windows.Forms.Padding(4);
            this.BButton.Name = "BButton";
            this.BButton.Size = new System.Drawing.Size(42, 28);
            this.BButton.TabIndex = 5;
            this.BButton.Text = "B";
            this.BButton.UseVisualStyleBackColor = true;
            this.BButton.MouseDown += new System.Windows.Forms.MouseEventHandler(this.BButton_MouseDown);
            this.BButton.MouseUp += new System.Windows.Forms.MouseEventHandler(this.BButton_MouseUp);
            // 
            // XButton
            // 
            this.XButton.Location = new System.Drawing.Point(358, 83);
            this.XButton.Margin = new System.Windows.Forms.Padding(4);
            this.XButton.Name = "XButton";
            this.XButton.Size = new System.Drawing.Size(42, 28);
            this.XButton.TabIndex = 6;
            this.XButton.Text = "X";
            this.XButton.UseVisualStyleBackColor = true;
            this.XButton.MouseDown += new System.Windows.Forms.MouseEventHandler(this.XButton_MouseDown);
            this.XButton.MouseUp += new System.Windows.Forms.MouseEventHandler(this.XButton_MouseUp);
            // 
            // YButton
            // 
            this.YButton.Location = new System.Drawing.Point(358, 169);
            this.YButton.Margin = new System.Windows.Forms.Padding(4);
            this.YButton.Name = "YButton";
            this.YButton.Size = new System.Drawing.Size(39, 28);
            this.YButton.TabIndex = 9;
            this.YButton.Text = "Y";
            this.YButton.UseVisualStyleBackColor = true;
            this.YButton.MouseDown += new System.Windows.Forms.MouseEventHandler(this.YButton_MouseDown);
            this.YButton.MouseUp += new System.Windows.Forms.MouseEventHandler(this.YButton_MouseUp);
            // 
            // RbbLedOutline
            // 
            this.RbbLedOutline.BackColor = System.Drawing.SystemColors.Window;
            this.RbbLedOutline.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.RbbLedOutline.Controls.Add(this.RgbLED);
            this.RbbLedOutline.Location = new System.Drawing.Point(368, 131);
            this.RbbLedOutline.Margin = new System.Windows.Forms.Padding(4);
            this.RbbLedOutline.Name = "RbbLedOutline";
            this.RbbLedOutline.Size = new System.Drawing.Size(20, 19);
            this.RbbLedOutline.TabIndex = 4;
            // 
            // RgbLED
            // 
            this.RgbLED.Location = new System.Drawing.Point(3, 3);
            this.RgbLED.Margin = new System.Windows.Forms.Padding(0);
            this.RgbLED.Name = "RgbLED";
            this.RgbLED.Size = new System.Drawing.Size(10, 10);
            this.RgbLED.TabIndex = 1;
            // 
            // Title
            // 
            this.Title.AutoSize = true;
            this.Title.Font = new System.Drawing.Font("Calibri", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Title.Location = new System.Drawing.Point(-1, 9);
            this.Title.MaximumSize = new System.Drawing.Size(460, 37);
            this.Title.MinimumSize = new System.Drawing.Size(460, 37);
            this.Title.Name = "Title";
            this.Title.Size = new System.Drawing.Size(460, 37);
            this.Title.TabIndex = 10;
            this.Title.Text = "240 X 135 Graphic Display";
            this.Title.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // MainForm
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(459, 302);
            this.Controls.Add(this.Title);
            this.Controls.Add(this.RbbLedOutline);
            this.Controls.Add(this.YButton);
            this.Controls.Add(this.XButton);
            this.Controls.Add(this.BButton);
            this.Controls.Add(this.AButton);
            this.Controls.Add(this.StatusStrip);
            this.Controls.Add(this.panel1);
            this.DoubleBuffered = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "MainForm";
            this.Text = "Pico Display Simulator";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyDown);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyUp);
            this.StatusStrip.ResumeLayout(false);
            this.StatusStrip.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.RbbLedOutline.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip StatusStrip;
        private System.Windows.Forms.Panel LcdPanel;
        private System.Windows.Forms.ToolStripStatusLabel StatusStripLabel;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button AButton;
        private System.Windows.Forms.Button BButton;
        private System.Windows.Forms.Button XButton;
        private System.Windows.Forms.Button YButton;
        private System.Windows.Forms.Panel RbbLedOutline;
        private System.Windows.Forms.Panel RgbLED;
        public System.Windows.Forms.Label Title;
    }
}

