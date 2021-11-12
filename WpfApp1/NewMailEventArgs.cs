using System;
using System.Collections.Generic;
using System.Text;

namespace WpfApp1
{
    class NewMailEventArgs : EventArgs
    {
        private readonly String m_from, m_to, m_subject;
        public NewMailEventArgs(String from, String to, String subject)
        {
            m_from = from;
            m_to = to;
            m_subject = subject;
        }
        public String From
        {
            get { return m_from; }
        }
        public String To
        {
            get { return m_to; }
        }

        public String Subject
        {
            get { return m_subject; }
        }
    }
}
