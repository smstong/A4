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
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            ThreadPool.QueueUserWorkItem(ThreadProc, this);
        }

        // non-main thread update GUI
        void ThreadProc(Object state)
        {
            MainWindow win = state as MainWindow;
            lock (this)
            {
                this.Dispatcher.BeginInvoke(
                    System.Windows.Threading.DispatcherPriority.Background,
                    new Action(
                        () => this.Btn1.Background = Brushes.Red)
                    );
            }
        }
    }

}
