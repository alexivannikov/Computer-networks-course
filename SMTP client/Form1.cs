using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Net.Mail;
using System.Windows.Forms;

namespace SMTP_client
{
    public partial class Form1 : Form
    {
        public void SendMessage()
        {
            MailAddress from = new MailAddress("aleksejivanniko@yandex.ru", "Alexey");

            MailAddress to = new MailAddress(Convert.ToString(textBox1.Text));

            MailMessage msg = new MailMessage(from, to);

            /*Тема письма*/
            msg.Subject = Convert.ToString(textBox2.Text);

            /*Содержание письма*/
            msg.Body = Convert.ToString(richTextBox1.Text);

            Console.WriteLine();

            /*Письмо представляет код html*/
            msg.IsBodyHtml = false;

            msg.Attachments.Add(new Attachment(textBox3.Text));

            SmtpClient smtp = new SmtpClient("smtp.yandex.ru", 587);

            /*Логин и пароль*/
            smtp.Credentials = new NetworkCredential("aleksejivanniko@yandex.ru", "*");

            smtp.EnableSsl = true;

            try
            {
                smtp.Send(msg);
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SendMessage();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
