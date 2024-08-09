#include<iostream>
#include<thread>
#include<memory>
#include<mutex>
#include<queue>
#include<condition_variable>
#include <windows.h> 
#include<functional>
using namespace std;
class ThreadPool {
public:
	ThreadPool(int numThread):stop(false) {
		for (int i = 0; i < numThread; i++) {
			threads.emplace_back([this] {
				while (1) {
					unique_lock<mutex>lock(mtx);
					con.wait(lock, [this] {
						return !tasks.empty() || stop;
						});
					if (stop&&tasks.empty())return;
					auto task(move(tasks.front()));
					tasks.pop();
					lock.unlock();
					task();
				}
				});
		}
	}

	~ThreadPool() {
		{
			unique_lock<mutex>lock(mtx);
			stop = true;
		}
		con.notify_all();
		for (auto& t : threads) {
			t.join();

		}
	}
	template<class F,class... Args>
	void enqueue(F&& f, Args&&... args) {
		auto task = bind(forward<F>(f) ,forward<Args>(args)...);
		{
			unique_lock<mutex>lock(mtx);
			tasks.emplace(move(task));
		}
		con.notify_one();

	}

private:
	vector<thread>threads;
	queue<function<void()>>tasks;
	mutex mtx;
	condition_variable con;

	bool stop;
};
int main() {
	mutex mtx;
	ThreadPool pool(4);
	for (int i = 0; i < 10; i++) {
		 
		pool.enqueue([i] {
			{	
				cout << "创建了： " << i << endl;
			}

			this_thread::sleep_for(chrono::seconds(1));
			cout << "jieshul " << i << endl;
			});



	}







	return 0;
}
//int a = 0;
//mutex mtx;
//queue<int>q;
//condition_variable con;
//void shengchan() {
//	for (int i = 0; i < 10; i++) {
//		unique_lock<mutex>lock(mtx);
//		q.push(i);
//		con.notify_one();
//		cout << "生产了：" << i << endl;
//		std::this_thread::sleep_for(chrono::microseconds(100));
//	}
//}
//void xiaofei() {
//	while (1) {
//		unique_lock<mutex>lock(mtx);
//		con.wait(lock, []() {return !q.empty(); });
//		auto t = q.front();
//		q.pop();
//		cout << "xiaofeil" << t << endl;
//	}
//}