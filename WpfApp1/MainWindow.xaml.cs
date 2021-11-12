using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;

namespace WpfApp1
{
    public delegate int cal(int x, int y);

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private MailManager mgr = new MailManager();
        private cal add;
        public MainWindow()
        {
            InitializeComponent();
            mgr.NewMail += Mgr_NewMail;
            mgr.NewMail += Mgr_NewMail1;
            add += (x,y)=> { return x + y; };
            this.Title = "Main Thread ID: " + Thread.CurrentThread.ManagedThreadId;
        }

        public int _add(int x, int y)
        {
            return x + y;
        }

        private void Mgr_NewMail1(object sender, NewMailEventArgs e)
        {
            MessageBox.Show("another handler");
        }

        private void Mgr_NewMail(object sender, NewMailEventArgs e)
        {
            this.Title = e.Subject + e.From + e.To;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // ThreadPool.QueueUserWorkItem(ThreadProc, this);
            //this.mgr.RecvEmail();
            //this.Title = "sum: " + this.add(1, 1);

            //ThreadPool threads are not allowed to update UI created by UI thread
            //ThreadPool.QueueUserWorkItem(obj => { this.Title = "hello"; });

            // Tasks runing by SynchronizationScheduler run in UI thread
            //Task task1 = new Task(obj => { 
            //    this.Title = "hello" + Thread.CurrentThread.ManagedThreadId; 
            //}, null);
            //task1.Start(TaskScheduler.FromCurrentSynchronizationContext());

            //this.Title = TaskScheduler.FromCurrentSynchronizationContext().ToString();

            //var uiThread = TaskScheduler.FromCurrentSynchronizationContext();

            ThreadPool.QueueUserWorkItem(obj =>
            {
                this.RunByUIThread(() => { this.Title = "hello"; });
            });
            // Timers
            //System.Threading.Timer timer1 = new Timer(obj =>
            //{
            //    this.timer_1++;
            //    new Task(()=>
            //    {
            //        this.Title = this.timer_1.ToString();
            //    }).Start(uiThread);
            //}, null, 0, 1000);

        }
        private TaskScheduler uiThreadScheduler = TaskScheduler.FromCurrentSynchronizationContext();
        public void RunByUIThread(UpdateUIFunc cb)
        {
            Task task = new Task(() =>
            {
                cb();
            });
            task.Start(this.uiThreadScheduler);
        }
        private int timer_1 = 0;
        // non-main thread update GUI
        void ThreadProc(Object state)
        {
            MainWindow win = state as MainWindow;
            MessageBox.Show(Thread.CurrentThread.ManagedThreadId.ToString());
            lock (this)
            {
                this.Dispatcher.BeginInvoke(
                    System.Windows.Threading.DispatcherPriority.Background,
                    new Action(
                        () => {
                            MessageBox.Show(Thread.CurrentThread.ManagedThreadId.ToString());
                            this.Btn1.Background = Brushes.Red; 
                        })
                    );
            }
        }

        
    }
    public delegate void UpdateUIFunc();

}
