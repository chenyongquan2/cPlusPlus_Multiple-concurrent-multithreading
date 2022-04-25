#include <iostream>
#include <string>
#include<thread>
#include <future>

using namespace std;


int  mythread()
{
	cout << "子线程id=" << std::this_thread::get_id() << endl;
	return 1;
}


int main(void)
{
	cout << "main thread id =" << std::this_thread::get_id() << endl;
	//创建一个异步线程
	//std::future<int> result = std::async(mythread);

	std::future<int> result = std::async(mythread);
	std::future_status status = result.wait_for(std::chrono::seconds(0));//特定写0s,看是否被立即执行了
	//std::future_status status = result.wait_for(0s); 
	if (status==std::future_status::ready)//说明创建了新的线程,而且已经执行完成
	{
		cout << "线程执行完毕" << endl;
		cout << result.get() << endl;
	}
	else if (status == std::future_status::timeout)//说明创建了新的线程，但是还没执行完成。
	{
		cout << "线程超时间，还没有执行结束" << endl;
		cout << result.get() << endl;
	}
	else if (status == std::future_status::deferred)//说明是延迟调用，没有起新的线程(资源紧张)
	{
		cout << "线程被延迟调用" << endl;
		//cout << result.get() << endl;//手动在本线程调用这个任务，并且等待这个任务完成(相当于函数调用)
		//等待get返回才会往下继续推进执行。
	}
	
	system("pause");
	return 0;
}

/*
 *std::async参数详细谈
 *--用来创建一个异步任务
 	async()--我们一般叫创建一个异步任务，async()有时候不创建一个新线程，
	谁调用get()方法，就在那个西安测绘给你中执行
 
 *1)参数--延迟调用参数
 *std::async(std::launch::deferred)--延迟调用，并且不创建新线程--延迟到future对象调用get(),wait()方法才会创建
	a)std::async(std::launch::async);--强制创建一个异步任务（线程）,在新线程中执行
	b)std::async(std::launch::any);
	std::async(std::launch::sync);
	如果同时用 std::launch::async | std::launch::deferred
	
	c)deferred|async使用亦或
	这里这个 | 意味着async的行为可能是 std::launch::async 创建新线程立即执行，
	也可能是 std::launch::deferred 没有创建新线程并且延迟到调用get()执行，由系统根据实际情况来决定采取哪种方案。
	z异步（创建新线程，立即执行）或者同步（延迟到get,wait方法执行），是两个行为中的一种。

	d)不带参数--其实没有指定参数，默认参数是（deferred|async），上一课讲错了
	
自行决定是什么意思？？怎么决定是否创建新线程？？
2)std::async和std::thread()区别：
a)std::thread()如果系统资源紧张可能出现创建线程失败的情况，如果创建线程失败那么程序就可能崩溃，
而且不容易拿到函数返回值（不是拿不到，要使用一个全局量等手段接受）

b)std::async()创建异步任务。可能创建线程也可能不创建线程，线程的返回值可以使用future对象来接受
用async()创建线程一般不会报异常，如果系统资源紧张，不加额外参数的调用async()就不会创建线程，
而是后续谁调用get()来请求结果，线程入口函数就运行在调用的线程中。
如果强制创建新线程，就要使用std::async(std::launch::async);
 
由于系统资源限制：
(1)如果用std::thread创建的线程太多，则可能创建失败，系统报告异常，崩溃。
(2)如果用std::async，一般就不会报异常，因为如果系统资源紧张，无法创建新线程的时候，
async不加额外参数的调用方式就不会创建新线程。
而是在后续调用get()请求结果时，这个异步任务执行在这个调用get()的线程上。

如果你强制async一定要创建新线程就要使用 std::launch::async标记。(强制创建新线程) 承受的代价是，系统资源紧张时可能崩溃。
(3)根据经验，一个程序中线程数量 不宜超过100~200(因为线程切换会浪费很多cpu资源(要保存上下文))

3.std::async()不确定性问题解决
不加参数的async()调用，不能确定系统能否创建出新线程。
如果系统自主选择了推迟运行，意味着mythread线程入口函数不会马上执行，后面如果没有调用get()，压根就不执行。

std::future<int> result = std::async(mythread);问题焦点在于这个写法，任务到底有没有被推迟执行。
可通过future的wait_for返回状态来判断。

*/
