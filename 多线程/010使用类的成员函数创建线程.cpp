#include <iostream>
#include <string>
#include<thread>

using namespace std;


class A
{
public:
	mutable int m_i;//m_i的值是可以修改的，即使在const中
	//构造函数--类型转换构造函数，把一个整形转换为一个类A对象
	A(int a) :m_i(a)
	{
		cout << "A的构造函数执行" << endl;
		cout << this << "thread id" << this_thread::get_id() << endl;
	}
	//拷贝构造函数
	A(const A&m) :m_i(m.m_i)
	{
		cout << "A的拷贝构造函数执行" << endl;
		cout << this << "thread id" << this_thread::get_id() << endl;

	}
	~A()
	{
		cout << "A的析构函数执行" << std::endl;
		cout << this << "thread id" << this_thread::get_id() << endl;

	}


	void thread_work(int num)
	{
		cout << "子线程中的成员函数" << endl;
	}

	void operator()(int num)
	{
		cout << "使用类对象创建线程（）" << endl;
	}
};


int main(void)
{
	A obj(1100);
	//传递的线程的函数需要取地址，并且加上类作用域，对象，形式参数的值
	//thread mythread(&A::thread_work, obj, 14);
	//mythread.join();//这里用detach也行 因为会拷贝构造一份obj给子线程。
	/*
	构造函数被执行,thread_id=7276
	拷贝构造函数被执行,thread_id=7276
	类成员函数被当作线程的函数来执行，thread_id=9980
	主线程id=7276
	析构函数,thread_id=9980
	析构函数,thread_id=7276
	*/
	
	
	//如果真的传递一个引用，则拷贝构造函数不会执行
	thread mythread(&A::thread_work, ref(obj), 14); //等价于 => thread mythread(&A::thread_work, &obj, 14);
	
	/*
	构造函数被执行,thread_id=2952
	类成员函数被当作线程的函数来执行，thread_id=6720
	主线程id=2952
	析构函数,thread_id=2952
	
	//mythread.detach();此时用detach不安全
	*/
	
	//用仿函数的例子:
	//thread mythread(obj, 14);
	/*
	构造函数被执行,thread_id=5076
	拷贝构造函数被执行,thread_id=5076
	()仿函数被调用,num=20，thread_id=820
	主线程id=5076
	析构函数,thread_id=820
	析构函数,thread_id=5076
	*/
	
	//thread mythread(ref(obj), 14);//不掉用拷贝构造函数了
	/*
	构造函数被执行,thread_id=7436
	()仿函数被调用,num=20，thread_id=8324
	主线程id=7436
	析构函数,thread_id=7436
	*/

	mythread.join();
	system("pause");
	return 0;
}

//使用类的成员函数(包含operator()以及任意的类成员函数)执行一个线程
