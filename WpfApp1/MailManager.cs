using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace WpfApp1
{
    class MailManager
    {
        public event EventHandler<NewMailEventArgs> NewMail;
        protected virtual void OnNewMail(NewMailEventArgs e)
        {
            EventHandler<NewMailEventArgs> temp = Volatile.Read(ref NewMail);
            if(temp != null)
            {
                temp(this, e);
            }
        }
        public void RecvEmail()
        {
            NewMailEventArgs e = new NewMailEventArgs("Bob", "Sam", "Nothing to do");
            this.OnNewMail(e);
        }
    }
}
