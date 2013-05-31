using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace CSUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private List<List<Point>> shapes = new List<List<Point>>();

        private void exportButton_Click(object sender, EventArgs e)
        {
            using (TextWriter writer = new StreamWriter(Properties.Settings.Default.OutputFile))
            {
                writer.WriteLine(exportBox());
                foreach (List<Point> shape in shapes)
                    exportShape(shape, writer);
            }
        }

        private void exportShape(List<Point> shape, TextWriter writer)
        {
            writer.WriteLine("Shape");

            foreach (Point point in shape)
                writer.WriteLine("Point " + point.X.ToString() + " " + point.Y.ToString());

            writer.WriteLine("epahS");
        }

        private string exportBox()
        {
            return "Box " + drawingPanel.Width.ToString() + " " + drawingPanel.Height.ToString();
        }

        private void drawingPanel_MouseDown(object sender, MouseEventArgs e)
        {
            startShape();
        }

        private void drawingPanel_MouseUp(object sender, MouseEventArgs e)
        {
            endShape();
        }

        private void drawingPanel_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                addPoint(e.X, e.Y);
            }
        }

        private void startShape()
        {
            shapes.Add(new List<Point>());
        }

        private void endShape()
        {

        }

        private void addPoint(int x, int y)
        {
            shapes[shapes.Count-1].Add(new Point(x,y));
            drawingPanel.Invalidate();
        }

        private Pen[] pens = {
            System.Drawing.Pens.Black,
            System.Drawing.Pens.Red,
            System.Drawing.Pens.Blue,
            System.Drawing.Pens.Green,
            System.Drawing.Pens.Yellow,
        };

        private void drawingPanel_Paint(object sender, PaintEventArgs e)
        {
            for (int si = 0; si < shapes.Count; ++si )
            {
                List<Point> shape = shapes[si];
                Pen pen = pens[si % pens.Length];
                for (int i = 0; i < shape.Count - 1; ++i)
                {
                    e.Graphics.DrawLine(pen, shape[i], shape[i + 1]);
                }
            }
        }

        private void clearButton_Click(object sender, EventArgs e)
        {
            shapes.Clear();
            drawingPanel.Invalidate();
        }
    }
}