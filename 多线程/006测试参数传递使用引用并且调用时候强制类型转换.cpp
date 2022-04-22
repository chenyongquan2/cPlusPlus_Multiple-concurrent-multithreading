#include <iostream>
#include <string>
#include<thread>
using namespace std;
class A
{
public:
	int m_i;
	//构造函数--类型转换构造函数，把一个整形转换为一个类A对象
	A(int a) :m_i(a)
	{
		cout << "A的构造函数执行" << endl;
	}
	//拷贝构造函数
	A(const A&m) :m_i(m.m_i)
	{
		cout << "A的拷贝构造函数执行" << endl;
	}
	~A()
	{
		cout << "A的析构函数执行" << std::endl;
	}
};

//实际上线程的参数对象使用引用也会额外复制出一个对象，而不是使用引用
void MyPrint(const int i, const A&pMyBuf)//这里虽然能绑定右值，但是还是会把生成的临时对象tmp给拷贝构造给这里的pMyBuf
//void MyPrint(const int i, const A pMyBuf)//若这里参数不用引用来接，则还会多一次拷贝构造函数的开销
{
	cout << &pMyBuf << endl;//打印的是地址
	return;
}

int main(void)
{
	int myVar = 10;
	int mySecondVar = 20;

	//thread mythread(MyPrint, myVar, mySecondVar);//当main()函数执行完毕，可能这时候才会去使用mySecondVar来隐式类型转换来构造一个类对象，但这时候mySecondVar可能已经不在了。。
	
	//改进方式:
	/* 那么我在传递参数的时候是否可以构造一个临时对象的时候，时候可以解决问题？？
	 * 能解决问题，会执行一个拷贝构造函数，也就是在主函数有效的时候已经构造一个A
 	 * 对象传递给线程的参数中，从此不再依赖于主函数中的对象。*/
	thread mythread(MyPrint, myVar, A(mySecondVar));//在创建线程的时候构造临时对象传递参数的方法是可行的。Todo:学习一下这种写法!!!

	
	//mythread.join();
	mythread.detach();
	cout << "这是主函数的执行" << endl;
	system("pause");
	return 0;
	
	//打印输出:
	//构造函数
	//拷贝构造函数
	//析构函数
	//"这是主函数的执行"
}
/* 
 * 本质要求：
 * （1）只要把临时对象构造出来传递给线程，那么就能在主线程执行完毕后，子线程也是安全的。
 * （2）若传递int这种简单类型，建议都是值传递，不要使用引用传递
 * （3）如果传递类对象，避免隐式类型准换(因为真正发生隐式类型转换的时机可能在main函数之后)
 *	最好在创建线程这一行手动构造出临时对象的方式，在函数的形式参数里面使用引用来进行接受，否则系统还会构造出来一次对象
 *
 *	(所作的工作都是预防主线程的退出导致子线程的非法引用/访问。)
 *
 ************终极结论：建议不适用detach() ，只使用join()，这样就不存在局部变量失效使引用失效的问题(本节课讲的问题都不在是问题。)
 *	     //////思考：为什么使用临时对象就安全了，而使用自动类型转换就是不安全的？？
 *	
 *	
 */



