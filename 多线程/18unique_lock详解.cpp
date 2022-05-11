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
	//通过函数返回一个临时的unique_lock对象
	unique_lock<mutex> rtn_unique_lock()
	{
		unique_lock<mutex> temGuard(my_mutex1);
		return temGuard;//从函数返回一个局部的unique_lock是可以的 
		//返回这种临时temGuard对象会导致系统生成了临时的unique_lock对象，并调用了unique_lock的移动构造函数
	}


	//把收到的消息入到一个队列，子线程的启动函数
	void inMsgRecvQueue()
	{
		for (int i = 0; i < 10000; i++)
		{
			cout << "inMsgQueue插入一个元素" << i << endl;
		
			//lock_guard<mutex> myGuard2(my_mutex1);
			
			/*1）使用std::adopt_lock参数的例子。
			//std::lock(m_mutex);//必须得先lock 如果下面参数带有std::adopt_lock的话。
			std::unique_lock<mutex> lockguard1(m_mutex,std::adopt_lock);
			*/
			
			//2）使用try_to_lock参数的例子
			//unique_lock<mutex> myGuard1(my_mutex1,try_to_lock);//线程不会阻塞在这一行，若拿不到锁，也会继续往下执行。
			//if (myGuard1.owns_lock())//如果拿到了锁
			//{
				//======但其实这里能拿到锁的机会比较小，因为每次都是try_to_lock()，而不像另外一个线程那样 一直在阻塞等待这个锁(蹲点)
			//	msgRecvQueue.push_back(i);//假设这个数字i就是收到的玩家的命令
			//}
			//}
			//else
			//{
			//	cout << "没有拿到锁----------------------" << endl;
			//}

			//3）defer_lock参数
			/*
			unique_lock<mutex> myGuard1(myMutex1,defer_lock);//如果后面没有手动unlock的话，则这个类对象myGuard1析构函数会去执行unlock().
			
			//需要处理一些共享代码，就得你自个手动lock()
			myUniLock.lock();
			//Todo:处理一些共享代码1 eg: msgRecvQueue.push_back(i);
			//Todo:处理一些共享代码2
			
			//因为下面可能要先处理一些非共享的代码，所以可以先提前unlock()
			myUniLock.unlock();
			//Todo:处理一些非共享代码1 eg:cout << "插入一个元素" << i << endl;
			//Todo:处理一些非共享代码2
			//Todo:处理一些非共享代码3
			
			//需要处理一些共享代码，就得你自个手动lock()
			myUniLock.lock();
			//Todo:处理一些共享代码1
			//Todo:处理一些共享代码2
			
			*/
			
			//unique_lock<mutex> myGuard1(my_mutex1,defer_lock);
			//没有加锁的my_mutex1
			//加锁之后不用自己解锁
			///*myGuard1.lock();*/
			//if (myGuard1.try_lock()==true)
			//{
			//	msgRecvQueue.push_back(i);
			//}
			//else
			//{
			//	cout << "没有拿到锁----------------------" << endl;
			//}

			//4)release()方法
			/*	
			unique_lock<mutex> myGuard1(my_mutex1);
			mutex*ptx = myGuard1.release();//注意：现在你有责任要负责unlock了，否则这个若一直不unlock，则另外一个线程会一直在等待这个锁。
			msgRecvQueue.push_back(i);//此时还没执行unlock()，因此还可以放心操作共享数据
			ptx->unlock();//手动unlock()
			*/

			//错误用法1:
			//unique_lock<mutex> myGuard1(my_mutex1);
			//unique_lock<mutex> myGuard2(my_mutex1);//这里会再次去lock() 崩溃。。。
			
			//错误用法2:
			//unique_lock<mutex> myGuard1(my_mutex1);
			//unique_lock<mutex> myGuard2(myGuard1);//这里直接报错，不允许所有权复制.
			
			//所有权转移
			//方法一:
			unique_lock<mutex> myGuard1(my_mutex1);//myGuard1拥有my_mutex1的所有权
			unique_lock<mutex> myGuard2(move(myGuard1));//这里属于移动语义，myGuard1和my_mutex1没关系了指向空，而现在myGuard2指向my_mutex1

			//方法二：
			//unique_lock<mutex> lockguard1 = getUniqueLock();//返回的临时对象移动构造在了这里lockguard1的预留空间上


		}
	}
	//读共享数据函数的封装函数
	bool outMsgprocess(int&command)
	{
		
		//lock_guard<mutex> myGuard1(my_mutex1);
		unique_lock<mutex> myGuard1(my_mutex1);

		//睡眠20s
		/*chrono::milliseconds dura(20000);
		this_thread::sleep_for(dura);*/

		if (!msgRecvQueue.empty())
		{
			//消息队列不为空
			command = msgRecvQueue.front();//返回第一个元素
			msgRecvQueue.pop_front();//移除第一个元素
			return true;
		}
		
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
 1)unique_lock()取代lock_guard()
 *unique_lock是一个模板类，(工作中一般推荐使用lock_guard()，其取代了mutex的lock(),unlock()函数)
 *用法和lock_guard很相似
 eg:
 	unique_lock<mutex> myUniLock(myMutex);

 对比:unique_lock比lock_guard灵活很多（unique_lock支持更多的参数,多出来很多用法），效率差一点。内存占用多一点。
 
 2)unique_lock支持的第二个参数:
 	a)adopt_lock:一个标记，表示互斥量已经被lock了，互斥量必须在前面先手动lock了，否则会报异常。(和lock_guard第二个参数带adopt_lock参数效果一样)
 	
 *
 2）try_to_lock()是我们会尝试用mutex的lock去锁定mutex，但是如果没有锁定成功，会立即返回，不会等待锁住。
 (使用前提:前面自个线程先提前调用lock()锁住，因为可能会出现在本线程lock()两次)
 
 
 3)defer_lock
 （使用前提：不能提前lock）
 *defer_lock的意思就是没有给mutex加锁，初始化了一个没有加锁的mutex，
 
 //我们借着default_lock的话题，可以灵活的调用一些unique_lock的成员函数。
 4)unique_lock的一些成员函数:
	 * lock()
	 * unlock()--会自动解锁（可能在中间处理一些非共享代码，然后再锁上）
	 * try_lock()--尝试加锁，不成功也不阻塞
	 * release()--返回它锁管理的mutex对象指针，并释放管理权
	 	使得unique_lock和mutex不再有关系,返回的是原始的mutex指针，如果原来mutex处于加锁状态，就需要自己接管解锁;若本来没加锁过，则无需手动unlock()。
	
	 * 用参数和使用成员函数都是可以的。
 *
 *为什么有时候需要unlock()，因为你lock锁住的代码越少，整个程序越快
 *有时候也把锁住的代码多少(行数等等代码量)称为锁的粒度，粒度一般用粗细描述，
 *锁住的代码越少，粒度叫细，执行效率越高。
 *反之，粒度越粗，执行的效率越低。
 *所以选择合适的粒度的代码进行保护，是程序员能力和实力的体现
 *
 5）
 *unique_lock()所有权的传递，
 方法一:std::move()
 *通常情况下unique_lock应该和一个mutex相互关联。
 unique_lock<mutex> myGuard1(my_mutex1);
 *myGuard拥有my_mutex1的所有权，并且所有权可以转移给其他unique_lock对象
 *所有权不能复制，只能转移(和unique_ptr很类似)。
 
 方法二:函数return返回一个临时对象，也可实现转移
 
 *
*/

