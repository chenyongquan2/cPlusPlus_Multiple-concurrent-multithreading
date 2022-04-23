#include <iostream>
#include <string>
#include<thread>

using namespace std;
/*
 * 线程id：id是一个数组，每一个主线程和子线程都对应着一个id数字，不同的线程
 * 他的线程id是不同的，线程id可以获取的，使用c++标准库中的函数获取thread::get_id Method
 * 
 */
class A
{
public:
	int m_i;
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
};


void MyPrint(const A&pMyBuf)
{
	cout << "子线程的MyPrint的参数地址是:" << &pMyBuf << endl;
	cout << "thread is:" << this_thread::get_id() << endl;
}

int main(void)
{
	int myVar = 10;
	int mySecondVar = 20;
	cout << "这是主函数的执行" << endl;
	cout << "主线程id是：" << this_thread::get_id() << endl;

	//thread mythread(MyPrint, myVar, mySecondVar);
	/*
	主线程id=8976
	构造函数被执行,thread_id=4908
	a的地址为009FF4FC,thread_id=4908
	析构函数,thread_id=4908
	*/
	//结论:测试可得，使用隐式类型转换，是在子线程中执行的，这时候有可能外面的main函数/主线程推出了...这时候变量mySecondVar就失效了(致命的问题。。。)
	
	thread mythread(MyPrint, A(mySecondVar));
	/*
	c场景1：方法定义为=void MyPrint(const A&pMyBuf)
	主线程id=5820
	构造函数被执行,thread_id=5820
	拷贝构造函数被执行,thread_id=5820
	a的地址为0080EA08,thread_id=7320
	析构函数,thread_id=5820
	析构函数,thread_id=7320
	主线程执行完毕
	
	场景2：方法定义为=void MyPrint(const ApMyBuf)//不用引用来接
	主线程id=616
	构造函数被执行,thread_id=616
	拷贝构造函数被执行,thread_id=616
	拷贝构造函数被执行,thread_id=5456(这里还会比上面的方式多一种拷贝构造)
	a的地址为01A4F758,thread_id=5456
	析构函数,thread_id=616
	析构函数,thread_id=5456
	析构函数,thread_id=5456
	
	*/
	//结论:如果使用使用强制类型转换构造一个临时对象，那么会在主线程中构建对象。
	//只要在主线程中构造（拷贝构造）完毕临时对象，那么子线程就可以使用

	//把一个整形对象MySecondVar传递给A类型对象的类型转换构造函数
	mythread.join();
	//mythread.detach();
	

	system("pause");
	return 0;
}

/*

 *
 *
 */
