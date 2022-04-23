#include <iostream>
#include <string>
#include<thread>
#include <memory>

using namespace std;

class A
{
public:
	mutable int m_i;//m_i的值是可以修改的，即使是const属性 也能被修改
	//int m_i;
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
	//void MyPrint(A&pMyBuf) 当传递thread myThread(myPrint, std::ref(myObj));时,可以把const去掉，这样子函数参数接收外面对象的引用，内部修改m_i时就会影响外面了
{
	pMyBuf.m_i = 3333;//我们修改这个值，不会影响到主线程中的值
	cout << "子线程的MyPrint的参数地址是:" << &pMyBuf << endl;
	cout << "thread is:" << this_thread::get_id() << endl;
}

//传递一个智能指针
void MyPrint2(unique_ptr<int>pint)
{
	cout << "子线程thread id is:" << this_thread::get_id() << endl;

}

int main(void)
{
	A myobj(10);
	/*
	//myPrint(const A& pmybuf)中引用不能去掉，如果去掉会多创建一个对象
	//const也不能去掉，去掉会出错
	//即使是传递的const引用，但在子线程中还是会调用拷贝构造函数构造一个新的对象，
	//所以在子线程中修改m_i的值不会影响到主线程
	//如果希望子线程中修改m_i的值影响到主线程，可以用thread myThread(myPrint, std::ref(myObj));
	//这样const就是真的引用了，myPrint定义中的const就可以去掉了，类A定义中的mutable也可以去掉了	
	*/
   	//thread mythread(MyPrint,myobj);//将类对象作为线程的参数
	thread mythread(MyPrint, ref(myobj));//将类对象作为线程的参数




	
	mythread.join();
	//mythread.detach();

	system("pause");
	return 0;
}

/*
 *传递类对象，智能指针作为线程参数传递
 *std::ref()函数
 *虽然用引用作为参数，但是依然调用拷贝构造函数，向子线程传递类对象类型的参数，不管接收者
 *使用什么类型接受（引用接受还是值接受），编译器都是按照拷贝的形式接受。
 *当我们需要传递一个影响主线程的引用，而不是拷贝，那么使用ref（reference）,就不会调用拷贝构造函数
 */
