#pragma once

#include <queue>
#include <optional>
#include <future>

class thread_pool
{
public:
	thread_pool(unsigned int num_threads)
	{
		init_mutex.lock();
		for (unsigned int i = 0; i < num_threads; i++)
		{
			threads.emplace_back([this]()
			{
				thread_func();
			});
		}
		init_mutex.unlock();
	}

	~thread_pool()
	{
		terminate();
	}

	void wait() const
	{
		while (!tasks.empty() || in_progress.load() > 0)
			std::this_thread::yield();
	}

	void interrupt()
	{
		task_mutex.lock();
		while (!tasks.empty())
		{
			tasks.pop();
		}
		task_mutex.unlock();

		wait();
	}

	bool is_terminated = false;
	void terminate()
	{
		task_mutex.lock();
		is_terminated = true;
		task_mutex.unlock();
		while (!threads.empty())
		{
			threads.front().join();
			threads.pop_front();
		}
	}

	template <typename Fn, typename... Args>
	auto async(Fn f, Args ... fargs)
	{
		using return_t = decltype(f(fargs...));
		auto* p = new std::promise<return_t>();
		
		auto task_wrapper = std::bind([p, f{std::move(f)}](Args... args)
		{
			if constexpr (std::is_same<return_t, void>::value)
			{
				f(std::move(args)...);
				p->set_value();
			}
			else
			{
				p->set_value(std::move(f(std::move(args)...)));
			}
		}, std::move(fargs)...);

		auto return_wrapper = [p]()
		{
			if constexpr (std::is_same<return_t, void>::value)
			{
				p->get_future().get();
				delete p;
			}
			else
			{
				auto temp = std::move(p->get_future().get());
				delete p;
				return std::move(temp);
			}
		};
		
		task_mutex.lock();
		tasks.emplace(std::move(std::async(std::launch::deferred, task_wrapper)));
		task_mutex.unlock();
		return std::move(return_wrapper);
	}

	std::list<std::thread> threads;
	std::queue<std::future<void>> tasks;
	std::atomic<int> in_progress;

private:
	void thread_func()
	{
		init_mutex.lock();
		init_mutex.unlock();

		while (true)
		{
			task_mutex.lock();
			bool has_task = false;
			std::remove_reference_t<std::future<void>&> task;
			if (has_task = !tasks.empty(), has_task)
			{
				task = std::move(tasks.front());
				tasks.pop();
			}
			task_mutex.unlock();

			if (!has_task)
			{
				if (is_terminated)
					return;
				std::this_thread::yield();
				continue;
			}
			else
			{
				in_progress += 1;
				task.get();
				in_progress -= 1;
			}
		}
	}

	std::mutex init_mutex;
	std::mutex task_mutex;
};
