using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.Reflection;
using System.Collections;
using static System.Net.Mime.MediaTypeNames;

namespace Simulator
{
    public partial class MainForm : Form
    {
        public int m_width;
        public int m_height;
        public Bitmap m_lcd;
        public Bitmap m_rgbLED;
        public Simulator.SocketListener m_listener;
        public bool m_pressedStateA = false;
        public bool m_pressedStateB = false;
        public bool m_pressedStateX = false;
        public bool m_pressedStateY = false;

        public MainForm()
        {
            InitializeComponent();
            this.Icon = Properties.Resources.pico_display_image;
            m_width = LcdPanel.ClientRectangle.Width;
            m_height = LcdPanel.ClientRectangle.Height;
            m_lcd = new Bitmap(m_width, m_height);
            FillImage(m_lcd, Color.Black, m_width, m_height);
            LcdPanel.BackgroundImage = m_lcd;
            m_rgbLED = new Bitmap(RgbLED.ClientRectangle.Width, RgbLED.ClientRectangle.Height);
            FillImage(m_rgbLED, Color.Black, RgbLED.ClientRectangle.Width, RgbLED.ClientRectangle.Height);
            RgbLED.BackgroundImage = m_rgbLED;


            m_listener = new Simulator.SocketListener();

            m_listener.RegisterCommand(new Simulator.Write(this));
            m_listener.RegisterCommand(new Simulator.UpdateDisplay(this));
            m_listener.RegisterCommand(new Simulator.Led(this));
            m_listener.RegisterCommand(new Simulator.Exit());
            m_listener.RegisterCommand(new Simulator.Nop(this));
            m_listener.RegisterCommand(new Simulator.FillLCD(this));
            m_listener.RegisterCommand(new Simulator.Title(this));

            new Thread(new ParameterizedThreadStart(Simulator.SocketListener.Start)).Start(m_listener);
        }

        public void UpdateLcd()
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(UpdateLcd));
            }
            else
            {
                StatusStripLabel.Text = String.Format("MCU Elapsed Time: {0:00} {1:00}:{2:00}:{3:00}.{4:000}", m_listener.m_dd, m_listener.m_hh, m_listener.m_mm, m_listener.m_sec, m_listener.m_msec);
                LcdPanel.BackgroundImage = m_lcd;
                StatusStrip.Refresh();
                LcdPanel.Refresh();
                RgbLED.Refresh();
            }
        }

        public void UpdateStatus()
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(UpdateStatus));
            }
            else
            {
                StatusStripLabel.Text = String.Format("MCU Elapsed Time: {0:00} {1:00}:{2:00}:{3:00}.{4:000}", m_listener.m_dd, m_listener.m_hh, m_listener.m_mm, m_listener.m_sec, m_listener.m_msec);
                StatusStrip.Refresh();
                RgbLED.Refresh();
            }
        }

        public void UpdateLED()
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(UpdateLED));
            }
            else
            {
                StatusStripLabel.Text = String.Format("MCU Elapsed Time: {0:00} {1:00}:{2:00}:{3:00}.{4:000}", m_listener.m_dd, m_listener.m_hh, m_listener.m_mm, m_listener.m_sec, m_listener.m_msec);
                StatusStrip.Refresh();
            }
        }

        private delegate void SafeCallDelegate(string text);

        public void UpdateTitle( string newTitle )
        {
            if (InvokeRequired)
            {
                var d = new SafeCallDelegate(UpdateTitle);
                Invoke(d, new object[] { newTitle });
            }
            else
            {
                Title.Text = newTitle;
                Title.Refresh();
            }
        }

        public void FillImage(Bitmap image, Color new_color, int w, int h)
        {
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    image.SetPixel(x, y, new_color);
                }
            }
        }


        private void ChangeInButtonState()
        {
            if (m_listener.m_socketHandle != null)
            {
                string buttons = String.Format("^buttons A {0} B {1} X {2} Y {3};\n",
                    m_pressedStateA ? "DN" : "up",
                    m_pressedStateB ? "DN" : "up",
                    m_pressedStateX ? "DN" : "up",
                    m_pressedStateY ? "DN" : "up");
                byte[] msg = Encoding.ASCII.GetBytes(buttons);
                m_listener.m_socketHandle.Send(msg);
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_listener.Stop();
        }

        /////////////////////////////////////
        private void AButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_AButtonMouseDown();
        }
        private void AButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_AButtonMouseUp();
        }
        private void Process_AButtonMouseDown()
        {
            if (!m_pressedStateA)
            {
                m_pressedStateA = true;
                ChangeInButtonState();
            }
        }
        private void Process_AButtonMouseUp()
        {
            if (m_pressedStateA)
            {
                m_pressedStateA = false;
                ChangeInButtonState();
            }
        }

        private void BButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_BButtonMouseDown();
        }
        private void BButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_BButtonMouseUp();
        }
        private void Process_BButtonMouseDown()
        {
            if (!m_pressedStateB)
            {
                m_pressedStateB = true;
                ChangeInButtonState();
            }
        }

        private void Process_BButtonMouseUp()
        {
            if (m_pressedStateB)
            {
                m_pressedStateB = false;
                ChangeInButtonState();
            }
        }


        private void XButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_XButtonMouseDown();
        }
        private void XButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_XButtonMouseUp();
        }
        private void Process_XButtonMouseDown()
        {
            if (!m_pressedStateX)
            {
                m_pressedStateX = true;
                ChangeInButtonState();
            }
        }

        private void Process_XButtonMouseUp()
        {
            if (m_pressedStateX)
            {
                m_pressedStateX = false;
                ChangeInButtonState();
            }
        }

        private void YButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_YButtonMouseDown();
        }
        private void YButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_YButtonMouseUp();
        }
        private void Process_YButtonMouseDown()
        {
            if (!m_pressedStateY)
            {
                m_pressedStateY = true;
                ChangeInButtonState();
            }
        }
        private void Process_YButtonMouseUp()
        {
            if (m_pressedStateY)
            {
                m_pressedStateY = false;
                ChangeInButtonState();
            }
        }

        /////////////////////////////////////
        private void MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.A:
                    Process_AButtonMouseDown();
                    break;
                case Keys.B:
                    Process_BButtonMouseDown();
                    break;
                case Keys.X:
                    Process_XButtonMouseDown();
                    break;
                case Keys.Y:
                    Process_YButtonMouseDown();
                    break;
                default:
                    break;
            }
        }

        private void MainForm_KeyUp(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.A:
                    Process_AButtonMouseUp();
                    break;
                case Keys.B:
                    Process_BButtonMouseUp();
                    break;
                case Keys.X:
                    Process_XButtonMouseUp();
                    break;
                case Keys.Y:
                    Process_YButtonMouseUp();
                    break;
                default:
                    break;
            }
        }
    }

}
