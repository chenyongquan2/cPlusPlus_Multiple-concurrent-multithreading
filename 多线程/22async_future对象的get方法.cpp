#include <iostream>
#include <string>
#include<thread>
#include <future>
using namespace std;

int mythread()
{
	chrono::milliseconds dura(5000);
	this_thread::sleep_for(dura);
	cout << "线程开始执行,线程的id是：" << std::this_thread::get_id() << endl;
	//休息5s

	return 5884;
}
int main(void)
{
	cout << "main thread id:" << std::this_thread::get_id() << endl;
	std::future<int> result = std::async(mythread);//自动创建一个线程，并启动线程入口函数
	cout << "continue........" << endl;
	int def = 0;
	
	//std::future对象的get()成员函数会等待线程执行结束并返回结果，拿不到结果它就会一直等待，感觉有点像join()。但是，它是可以获取结果的。
	cout << "未来的执行结果是："<<result.get() << endl;
	//result.get();//get()不能调用多遍 否则程序会崩溃。
	
	//或者result2.wait();//只等待线程结束，而不返回结果。（有点像join）
	system("pause");
	return 0;
}

/*
 * async--函数模板，用来启动一个异步任务，启动任务后，返回一个std::future对象
 * 什么叫启动一个异步任务--自动创建一个线程并启动线程的入口函数，返回一个future对象
 * 这个对象里面就包含线程入口函数锁返回的结果（就是线程返回的结果）也可以使用get（）方法来获取
 * 
 “future”将来的意思，也有人称呼std::future提供了一种访问异步操作结果的机制，就是说这个结果你可能没办法马上拿到，
 但是在不久的将来，这个线程执行完毕的时候，你就能够拿到结果了，
 所以，大家这么理解：future中保存着一个值，这个值是在将来的某个时刻能够拿到。
 
 */

