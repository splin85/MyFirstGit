namespace RemCtl
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
            this.listView1 = new System.Windows.Forms.ListView();
            this.Seq = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Member = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Status = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.IP = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.District = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Type = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.ServerLabel = new System.Windows.Forms.Label();
            this.ServBox = new System.Windows.Forms.TextBox();
            this.ConnetLabel = new System.Windows.Forms.Label();
            this.DisconnectLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // listView1
            // 
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Seq,
            this.Member,
            this.Status,
            this.IP,
            this.District,
            this.Type});
            this.listView1.GridLines = true;
            this.listView1.Location = new System.Drawing.Point(12, 12);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(398, 189);
            this.listView1.TabIndex = 0;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // Seq
            // 
            this.Seq.Text = "序号";
            // 
            // Member
            // 
            this.Member.Text = "会员";
            // 
            // Status
            // 
            this.Status.Text = "状态";
            // 
            // IP
            // 
            this.IP.DisplayIndex = 4;
            this.IP.Text = "IP地址";
            // 
            // District
            // 
            this.District.DisplayIndex = 3;
            this.District.Text = "地区";
            // 
            // Type
            // 
            this.Type.Text = "挂机";
            // 
            // ServerLabel
            // 
            this.ServerLabel.AutoSize = true;
            this.ServerLabel.Location = new System.Drawing.Point(12, 241);
            this.ServerLabel.Name = "ServerLabel";
            this.ServerLabel.Size = new System.Drawing.Size(41, 12);
            this.ServerLabel.TabIndex = 1;
            this.ServerLabel.Text = "服务器";
            this.ServerLabel.Click += new System.EventHandler(this.ServerLabel_Click);
            // 
            // ServBox
            // 
            this.ServBox.Location = new System.Drawing.Point(59, 238);
            this.ServBox.Name = "ServBox";
            this.ServBox.Size = new System.Drawing.Size(100, 21);
            this.ServBox.TabIndex = 2;
            this.ServBox.TextChanged += new System.EventHandler(this.ServBox_TextChanged);
            // 
            // ConnetLabel
            // 
            this.ConnetLabel.AutoSize = true;
            this.ConnetLabel.Location = new System.Drawing.Point(208, 241);
            this.ConnetLabel.Name = "ConnetLabel";
            this.ConnetLabel.Size = new System.Drawing.Size(29, 12);
            this.ConnetLabel.TabIndex = 3;
            this.ConnetLabel.Text = "连接";
            this.ConnetLabel.Click += new System.EventHandler(this.ConnectLabel_Click);
            // 
            // DisconnectLabel
            // 
            this.DisconnectLabel.AutoSize = true;
            this.DisconnectLabel.Location = new System.Drawing.Point(280, 241);
            this.DisconnectLabel.Name = "DisconnectLabel";
            this.DisconnectLabel.Size = new System.Drawing.Size(29, 12);
            this.DisconnectLabel.TabIndex = 4;
            this.DisconnectLabel.Text = "断开";
            this.DisconnectLabel.Click += new System.EventHandler(this.DisconnectLabel_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(503, 343);
            this.Controls.Add(this.DisconnectLabel);
            this.Controls.Add(this.ConnetLabel);
            this.Controls.Add(this.ServBox);
            this.Controls.Add(this.ServerLabel);
            this.Controls.Add(this.listView1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader Seq;
        private System.Windows.Forms.ColumnHeader Member;
        private System.Windows.Forms.ColumnHeader Status;
        private System.Windows.Forms.ColumnHeader IP;
        private System.Windows.Forms.ColumnHeader District;
        private System.Windows.Forms.ColumnHeader Type;
        private System.Windows.Forms.Label ServerLabel;
        private System.Windows.Forms.TextBox ServBox;
        private System.Windows.Forms.Label ConnetLabel;
        private System.Windows.Forms.Label DisconnectLabel;
    }
}

