#include <iostream>
#include <string>
#include<thread>
#include <future>
using namespace std;

class A
{
public:
	int mythread(int mypar)
	{
		cout << "mpar=" << mypar << endl;
		chrono::milliseconds dura(5000);
		this_thread::sleep_for(dura);
		cout << "线程开始执行,线程的id是：" << std::this_thread::get_id() << endl;
		//休息5s

		return 5884;
	}
	
};


int main(void)
{
	A a;
	int tempar = 12;
	cout << "main thread id:" << std::this_thread::get_id() << endl;
	//std::future<int> result = std::async(mythread);
	//std::future<int> result = std::async(&A::mythread,&a,tempar);//第二个对象使用对象引用，才能保证使用的是同一个对象
	std::future<int> result = std::async(std::launch::deferred,&A::mythread,&a,tempar);//第二个对象使用对象引用，才能保证使用的是同一个对象

	
	/*我们通过向async()函数传递一个额外的参数，该参数类型是std::launcher类型(枚举值)，来达到一些特殊的目的
	 *a)std::launch::deferred--表示线程入口函数调用被延迟到std::future的get或着wait()调用才执行
	 	*如果wait()或者get()没有被调用，则不会执行。
	 实际上用了deferred参数后，根本就没有创建新的子线程，是在主线程中直接调用的线程入口函数(所以这个函数里面打印的线程id=主线程id)。
	 *线程都没有创建新线程，在主线程中执行了线程入口函数。---执行挺奇怪
	 *b)std::launch::async --异步，在调用asyncde 时候就创建了新线程，如果不写第一个标记，默用的就是这个.
	 *c)std::launch::sync--同步
	 *d)std::launch::any
	 *
	 *
	 */

	cout << "continue........" << endl;
	int def = 0;
	//卡在这里等待线程执行完毕，通过future对象的get（）方法等待程序结束并得到返回结果
	cout << "未来的执行结果是：" << result.get() << endl;
	system("pause");
	return 0;
}////如果上面不get，那么这个函数也会在这等待async()的线程结束，才会退出这个函数。

/*
* async--函数模板，用来启动一个异步任务，启动任务后，返回一个std::future对象
* 什么叫启动一个异步任务--自动创建一个线程并启动线程的入口函数，返回一个future对象
* 这个对象里面就包含线程入口函数锁返回的结果（就是线程返回的结果）也可以使用get（）方法来获取
*
* future--未来---有人也称为future提供了一种访问异步操作结果的机制（复杂的运算，需要等待未来的运算结果
* --将来线程执行完毕就可以拿到结果了）
* future的wait()函数只等待线程结束，而不返回结果。
*/

