#include <iostream>
#include <string>
#include<thread>
#include <memory>

using namespace std;



void MyPrint2(unique_ptr<int>pn)

{
	//cout << "thread is:" << this_thread::get_id() << endl;
	;
}


int main(void)
{


	unique_ptr<int> Myp(new int(100));
	//智能指针转移使用move
	thread mythread(MyPrint2, std::move(Myp));
	mythread.join();
	//mythread.detach();//这里千万不能用detach，否则程序可能会有问题。
	/*
	2.主线程智能指针分配的内存在主线程的堆上，std::move()后主线程只是将主线程的智能指针指向空
	3.但通过new分配的内存仍然在主线程中，子线程指针指向的内存仍是主线程中	
	4.因为是浅拷贝，所以只是更改了指针指向，并没有实际更改内存
	5.当主线程结束后(进程也结束了)，会进行内存回收(包括堆内存和栈内存等空间)，但是子线程智能指针指向的是主线程地址，但这时候这块内存空间可能已经被系统回收了
	
	d补充说明:
	在windows系统中，堆有两种：全局堆和局部堆。
	全局堆要用windows API分配和释放，如果非驻留内存应用程序不释放，则会造成“永久”内存泄露。
	而局部堆是windows启动应用程序时，划给应用程序的堆，当应用程序结束，windows系统会自动回收，因此不会造成"永久"内存泄漏,
	(简单来说：现代操作系统，你只要走常规的标准堆内存申请(malloc之类)，程序退出后是会被系统回收的。)
	//但是，好的编程习惯是必要的，局部堆内存不需要时或指针指向局部堆新内存块时，局部堆老内存块一定要释放，否则在你的应用程序运行期间，就可能用"光"了局部堆。


	
	*/

	system("pause");
	return 0;
}

/*
 *没有编译通过,在gcc编译器中正确
 *复习智能指针，move()*/
