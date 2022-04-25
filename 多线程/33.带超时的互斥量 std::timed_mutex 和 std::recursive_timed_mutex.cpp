/*
5.1 std::timed_mutex：是待超时的独占互斥量
1)try_lock_for()：
等待一段时间，如果拿到了锁，或者超时了未拿到锁，就继续执行（有选择执行）如下：
*/

std::chrono::milliseconds timeout(100);
if (my_mymutex.try_lock_for(timeout)){
    //......拿到锁返回ture
}
else{
    std::chrono::milliseconds sleeptime(100);
    std::this_thread::sleep_for(sleeptime);
}
2)try_lock_until()：
参数是一个未来的时间点，在这个未来的时间没到的时间内，如果拿到了锁头，流程就走下来，如果时间到了没拿到锁，流程也可以走下来。
std::chrono::milliseconds timeout(100);
if (my_mymutex.try_lock_until(chrono::steady_clock::now() + timeout)){
    //......拿到锁返回ture
}
else{
    std::chrono::milliseconds sleeptime(100);
    std::this_thread::sleep_for(sleeptime);
}

两者的区别就是一个参数是时间段，一个参数是时间点
5.2 std::recursive_timed_mutex：是待超时的递归独占互斥量
/*

struct C
{
	void in()
	{
		//this_thread::sleep_for(1s);
		//if (m_mutex.try_lock_for(1s))
		if(m_mutex.try_lock_until(chrono::steady_clock::now()+1s))
		{
			cout << "1s内获取了锁" << endl;
			m_mutex.unlock();
		}
		else
		{
			cout << "超过了1s还没拿到锁" << endl;
		}
	}
	void out()
	{
		//
		m_mutex.lock();
		this_thread::sleep_for(5s);
		cout << "即将休眠5s" << endl;
		
		m_mutex.unlock();
		cout << "即将休眠5s结束" << endl;
	}

	timed_mutex m_mutex;
};

void test15()
{
	C cobj;
	thread t1(&C::in,std::ref(cobj));
	thread t2(&C::out,std::ref(cobj));
	t1.join();
	t2.join();
}

int main()
{
	test15();
	
    return 0;
}

//5.2 std::recursive_timed_mutex：是待超时的递归独占互斥量

