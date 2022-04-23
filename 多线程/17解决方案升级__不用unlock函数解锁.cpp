#include <iostream>
#include <string>
#include<thread>
#include <vector>
#include<list>
#include<mutex>
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

			//my_mutex1.lock();//先锁一个锁，再锁一个锁
			////两个锁不一定挨着，可能保存不同的代码
			//my_mutex2.lock();
			//也可以使用lock_guard()
			//lock_guard<mutex> mylock1(my_mutex1);

			lock(my_mutex2, my_mutex1);//和顺序无关
			lock_guard<mutex> myGuard1(my_mutex1, adopt_lock);//adopt_lock的作用是告诉lock_guard不用负责去调用lock()方法
			lock_guard<mutex> myGuard2(my_mutex2, adopt_lock);
			//自动判断是否需要上锁

			msgRecvQueue.push_back(i);//假设这个数字i就是收到的玩家的命令
			/*my_mutex1.unlock();
			my_mutex2.unlock();*/
		}
	}
	//读共享数据函数的封装函数
	bool outMsgprocess(int&command)
	{
		/*my_mutex2.lock();
		my_mutex1.lock();*/
		lock(my_mutex1, my_mutex2);
		lock_guard<mutex> myGuard1(my_mutex1, adopt_lock);
		lock_guard<mutex> myGuard2(my_mutex2, adopt_lock);
		//相当于每一个互斥量都调用了lock()
		if (!msgRecvQueue.empty())
		{
			//消息队列不为空
			int command = msgRecvQueue.front();//返回第一个元素
			msgRecvQueue.pop_front();//移除第一个元素
			/*my_mutex1.unlock();
			my_mutex2.unlock();*/
			return true;
		}
	/*	my_mutex1.unlock();
		my_mutex2.unlock();*/
		return false;
		//所有分支都要有unlock()，两个出口必须有两个unlock()

	}


	//把数据从消息队列中取出的子线程
	void outMsgRecvQueue()
	{
		int command = 0;
		for (int i = 0; i < 10000; i++)
		{
			bool result = outMsgprocess(command);
			if (result == true)
			{
				cout << "取消息函数执行成功" << command << endl;
			}
			else
			{
				cout << "消息队列中的消息为空" << i << endl;
			}
		}

		cout << endl;
	}
private:
	list<int>msgRecvQueue;//容器用来存放玩家发送过来的命令
	//创建一个互斥量的成员变量
	mutex my_mutex1;
	mutex my_mutex2;
};


int main(void)
{
	A myobj;
	thread myOutMsgObj(&A::outMsgRecvQueue, &myobj);//第二个是引用才能保证线程中用的是统一个对象
	thread myInMsObj(&A::inMsgRecvQueue, &myobj);
	myOutMsgObj.join();
	myInMsObj.join();

	cout << "main线程" << endl;//最后执行这一句，整个线程退出
	system("pause");
	return 0;
}
/*
0)出发点:如果不想手动unlock,那么就需要使用lock_guard<mutex> myGuard1(my_mutex1, adopt_lock);
1)std::lock_guard的std::adopt_lock参数
std::lock_guardstd::mutex my_guard(my_mutex,std::adopt_lock);
加入adopt_lock后，在调用lock_guard的构造函数时，不再进行lock();
adopt_guard为结构体对象，起一个标记作用，表示这个互斥量已经lock()，不需要在lock()。

源码:
lock_guard(_Mutex& _Mtx, adopt_lock_t)
		: _MyMutex(_Mtx)
		{	// construct but don't lock
		}
		
2)总结:
lock()函数是一次锁定多个互斥量，没有先后顺序。（但是使用场景不多，而且不知道c++这货稳定不，所以老师建议还是一个一个mutex.lock()）
*/

