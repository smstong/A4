using System;
using System.Threading;
using System.Threading.Tasks;

namespace ThreadTest
{
    public delegate int MyCallback(int x, int y);
    class Program
    {
        static int count = 0;
        static Object mylock = new object();
   
        static void Main(string[] args)
        {
            Console.WriteLine(TaskScheduler.Default);
            //Thread thread1 = new Thread(start);
            //thread1.IsBackground = true;
            //thread1.Start(5);
            Task[] tasks = new Task[1000];
            for (int i = 0; i < tasks.Length; i++)
            {
                //ThreadPool.QueueUserWorkItem(start, i);
                tasks[i] = new Task(start, i);
                tasks[i].Start();
 
            }
            Task.WaitAll(tasks);
            Console.WriteLine(count);
        }
        protected static void start(object state)
        {
            lock (mylock)
            {
                count++;
            }
        }
    }
}
