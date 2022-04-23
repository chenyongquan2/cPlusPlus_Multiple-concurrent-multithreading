#include <iostream>
#include <string>
#include<thread>
#include <allocators>
#include <mutex>
using namespace std;

//互斥量
std::mutex resource_mutex;


//单例类
class MyCAS
{
private:
	//构造函数私有化
	MyCAS(){}
private:
	static MyCAS*m_instance;//静态成员变量指针
public:
	/*
	static MyCAS*GetInstance()
	{
		//这种写法效率低，因为我们只有在第一次调用的时候才会创建，其他并不会创建。但是这样子写的话 是只要我们线程调用这个函数，你就给我加锁，
		//而不管有没有创建过m_instance
		std::unique_lock<std::mutex>MyMutex(resource_mutex);//自动加锁，自动解锁
		if (m_instance == NULL)
		{	
			m_instance = new MyCAS();
			static ReleaseObject ro;//静态局部变量，当程序退出的时候，这货肯定会被析构的	
		}
		
		return m_instance;
	}
	*/
	//改进方案：
	static MyCAS*GetInstance()
	{
		if (m_instance == NULL)//双重锁定/双重检查。
		{
			std::unique_lock<std::mutex>MyMutex(resource_mutex);//自动加锁，自动解锁
			if (m_instance == NULL)
			{
				m_instance = new MyCAS();
				static ReleaseObject ro;//静态局部变量，当程序退出的时候，这货肯定会被析构的

			}
		}
		
		return m_instance;
	}
	//类中套一个类实现内存释放（好像没起作用）
	class ReleaseObject
	{
	public:
		~ReleaseObject()
		{
			if (MyCAS::m_instance)
			{
				cout << "销毁对象" << endl;
				delete MyCAS::m_instance;
				MyCAS::m_instance = NULL;
			}
		}
		
	};
	void function1()
	{
		cout << "测试函数" << endl;
	}
	
};
//需要初始化静态成员变量
MyCAS* MyCAS::m_instance = NULL;

//线程入口函数
void startThread()
{
	cout << "线程开始执行了" << endl;
	MyCAS*p_a = MyCAS::GetInstance();
	cout << "线程执行完毕了" << endl;
	return;

}

int main(void)
{	
	//在主线程创建类实例
	////返回该类对象的指针
	//MyCAS*p_a = MyCAS::GetInstance();
	//p_a->function1();
	
	//面临的问题：需要我们在我们自个创建的线程（而不是主线程）中来创建MyCAS这个单例类的对象，这种线程可能不止(最少2个) 
	//我们可能面临GetInstance()这种成员函数需要互斥。
	//可以在加锁前判断m_instance是否为空，否则每次调用Singleton::getInstance()都要加锁，十分影响效率。

	//两个线程是同一个入口函数，所以这里会有两个流程或者两条通路同时执行startThread函数。
	//当其中一个线程进入getInstance，但是还没有创建对象，切换到第二个线程，这样就可能创建多个对象
	//m_instance == NULL不代表没被创建对象，可能马上就会创建，但是线程切换了
	thread mythread1(startThread);
	thread mythread2(startThread);
	mythread1.join();
	mythread2.join();

	system("pause");
	return 0;
}
/*
 * 设计模式大概谈
 * “设计模式”--代码的一些特定的写法，程序灵活，程序维护起来很方便，但是别人阅读起来很痛苦
 * 用设计模式写出的代码很晦涩难懂，head first，
 * 这是别人应付特别大的项目 的时候，根据实际开发敬仰，模块划分经验，总结程设计模式，先有开发经验，后有设计模式
 * 但是到中国来，不太一样，拿着一个项目硬套设计模式，一个小小项目，非要套几个设计模式。
 * 设计模式有其独特的有点，要活学活用，不要生搬硬套
 * 
 * 单例设计模式：使用的频率比较高。
 * 在整个项目中，有某个特殊的类。且属于该类的对象只能创建一个。多了我创建不了（类的实例只能有一个）
 * 如在：配置文件读写的时候为方便管理，就只有一个类对象，方便管理。
 * 
 * 
 * //单例设计模式共享数据问题分析解决
 * 最理想的是主线程创建单例对象，但是实际中可能有多个对象同时调用GetInstance()函数来创建单例对象
 * 这个时候就需要使用互斥
 * 
 * 
 * //std::call_once是一个函数模板，是c++11引入的函数
 * 第二个参数是一个函数名字，call_once的功能是保证函数a只会被调用一次
 * call_once具有互斥量这种能力，而且效率上，这比互斥量消耗的资源更少
 * call_once需要和一个标记结合使用，这个标记once_flag，其实once_flag是一个结构体
 * call_once就是通过这个标记判断函数是否执行，执行过call_once(通过call_once再去调用函数a)后就把这个参数设置为已经调用状态
 * 后续再调用但是标志once_flag已经被设置为“已经调用”状态了，就不会再执行这个函数
 */

