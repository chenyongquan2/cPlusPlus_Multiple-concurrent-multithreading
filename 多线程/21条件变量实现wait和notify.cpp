#include <iostream>
#include <string>
#include<thread>
#include <vector>
#include<list>
#include<mutex>
#include <condition_variable>//需要包含这个，否则会报错类型不完整
using namespace std;

//用成员函数作为线程初始化函数

class A
{
public:

	//把收到的消息入到一个队列，子线程的启动函数
	void inMsgRecvQueue()
	{
		for (int i = 0; i < 10000; i++)
		{
			cout << "inMsgQueue插入一个元素" << i << endl;

			unique_lock<mutex> myGuard1(my_mutex1);
			msgRecvQueue.push_back(i);
			//my_cond.notify_one();//我们尝试在等待这个条件变量my_cond的线程唤醒（要再去重新申请锁，并且判断后面的第二个参数是否为true）
			//若此时没有线程卡在wait这里，则上面的notify无效果。
			my_cond.notify_all();//同时唤醒所有睡眠等待这个条件变量my_cond的线程,
			//(让这些线程都唤醒，再去争夺mutex(只有一个线程能拿这个锁成功，还得卡在这不断尝试获取锁)，以及去判断第二个条件)
			//所以此时notify_all和notify_one效果差不多(只是说在当前例子这种场景下)
		}
	}
	//读共享数据函数的封装函数
	//bool outMsgprocess(int&command)
	//{
	//	//添加双重检查---思考：能否改为当数据不为空的时候，通知我们
	//	if (!msgRecvQueue.empty())
	//	{
	//		unique_lock<mutex> myGuard1(my_mutex1);
	//		if (!msgRecvQueue.empty())
	//		{
	//			//消息队列不为空
	//			command = msgRecvQueue.front();//返回第一个元素
	//			msgRecvQueue.pop_front();//移除第一个元素
	//			return true;
	//		}
	//	}
	//	

	//	return false;
	//	//所有分支都要有unlock()，两个出口必须有两个unlock()
	//}

public:
	//把数据从消息队列中取出的子线程
	void outMsgRecvQueue()
	{
		int command = 0;
		while (true)
		{
			unique_lock<mutex>sbguard1(my_mutex1);
			my_cond.wait(sbguard1,[this]//第一个参数为互斥量，第二个参数为可调用对象(要求返回一个bool值)
			{
				if (!msgRecvQueue.empty())
				{
					return true;
				}
				return  false;
			});
			
			/*
			 * wait()用来等待一个东西
			 //分为两步:
			 //1.尝试去获取互斥量的值，尝试去上锁,若互斥量此时上锁不了，则也会睡眠等待notify唤醒。
			 //2.若此时互斥量已上锁，则判断第二个参数的值
			 * 如果第二个参数的lambda表达式返回值是true，并且此时互斥量Mutex处于加锁状态，那么wait()直接返回并继续执行。
			 * 第二个参数lambad表达式返回值是false，那么wait将解锁互斥量。并阻塞到本行（睡眠），
			 * 	阻塞到什么时候为止呢？阻塞到其他某个线程调用notify_one()成员函数为止；
			 * 如果没有第二个参数，那么效果跟第二个参数lambda表达式返回false效果一样。
				即wait()将解锁互斥量，并阻塞到本行，阻塞到其他某个线程调用notify_one()成员函数为止。

			当其他线程用notify_one()将原本阻塞睡眠在wait()的线程唤醒后，恢复后的会去干啥
			a)wait()不断尝试获取互斥量锁，如果获取不到互斥量锁,那么流程就卡在wait()这里等待获取互斥量锁(此时不会去睡眠(处于醒着),而是一直在这尝试去获取这个互斥量锁)，
				如果获取到了互斥量锁，那么wait()就继续执行到b)
			b)
				b.1)如果wait有第二个参数(可调用对象),就判断这个lambda表达式的返回值
					如果表达式为false，那wait又对互斥量解锁，然后又休眠，等待再次被notify_one()唤醒
					如果表达式为true，则wait返回，流程可以继续执行（此时互斥量已被锁住）。
				b.2)如果wait没有第二个参数，则wait返回，流程走下去（此时互斥量已被锁住）。
			*/
			
			
			//能走到这里，说明一定已经对互斥量Mutex上了锁，并且此时msgRecvQueue满足了不为空的条件
			
			command = msgRecvQueue.front();
			msgRecvQueue.pop_front();
			sbguard1.unlock();//因为unique_lock的灵活性，可以随时解锁，避免锁住太长时间。
			//下面是一些具体的动作，比如玩家抽卡等操作，，，，
			//加入现在正在处理一个事务，需要一段时间，而不是卡在wait()等待，此时这个
			//notify_one()函数并没有作用。也就是说notify_one()不一定可以唤醒wait（）
		}
		
		
	}

		/*cout << endl;*/
private:
	list<int>msgRecvQueue;//容器用来存放玩家发送过来的命令
	//创建一个互斥量的成员变量
	std::mutex my_mutex1;
	std::condition_variable my_cond;//2017可以不适用()
	//生成一个条件对象
};


int main(void)
{
	A myobj;
	//测试哪一个线程被唤醒
	thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);//第二个是引用才能保证线程中用的是同一个对象
	thread myOutMsgObj2(&A::outMsgRecvQueue, &myobj);

	thread myInMsObj(&A::inMsgRecvQueue, &myobj);
	myOutMsgObj.join();
	myInMsObj.join();
	myOutMsgObj2.join();

	cout << "main线程" << endl;//最后执行这一句，整个线程退出
	system("pause");
	return 0;
}
/*
 *条件变量 std::condition_variable 
 *条件变量--用在线程中，当线程A等待条件满足，线程B专门网消息队列中扔消息
 *是一个和条件相关的类，说白了就是等待条件满足，需要和互斥量配合使用
 *
 *
 *wait  ()--需要创建一个条件变量，睡眠进行
 *notify_one()--唤醒wait()进程，但是不能保证一定能唤醒
 *
 *深入思考：
 *上面的代码可能导致出现一种情况：
 *当两个线程拿到锁的次数不一样，也就说数据处理不过来怎么办
 *因为outMsgRecvQueue()与inMsgRecvQueue()并不是一对一执行的，所以当程序循环执行很多次以后，可能在msgRecvQueue 中已经有了很多消息，
 *但是，outMsgRecvQueue还是被唤醒一次只处理一条数据。
 *这时可以考虑把outMsgRecvQueue多执行几次，或者对inMsgRecvQueue进行限流。

 *
 notify_one()：通知一个线程的wait()
 notify_all()：通知所有线程的wait()
*/

