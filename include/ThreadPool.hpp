#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <functional>
#include <condition_variable>
#include <future>
#include <atomic>
#include <thread>
#include <mutex>

class ThreadPool
{
public:
    ThreadPool(int argv_ThreadPool_size = 12);
    ~ThreadPool();

    template<class Func, class ...Argvs>
    auto add_task(Func&& func, Argvs&& ...argvs) -> std::future< typename std::result_of<Func(Argvs...)>::type >;

    const int get_task_size();

private:
    std::vector<std::thread> _ThreadPool;
    std::list< std::function<void()> > _TaskS;
    std::atomic<bool> _is_stop;

    std::mutex _mutex;
    std::condition_variable _condition;
};

inline ThreadPool::ThreadPool(int argv_ThreadPool_size)
    : _is_stop(false)
{
    for (int i = 0; i < argv_ThreadPool_size; ++i)
    {
        _ThreadPool.emplace_back
        (
            [this]()
            {
                for (; ;)
                {
                    std::function<void()> tmp_task;

                    {
                        std::unique_lock<std::mutex> tmp_lock(this->_mutex);
                        this->_condition.wait(tmp_lock, [this](){ return this->_is_stop || (this->_TaskS.size() != 0); });
                        if (this->_is_stop && (this->_TaskS.size() == 0))
                            return;

                        tmp_task = std::move(this->_TaskS.front());
                        this->_TaskS.pop_front();
                    }
                    tmp_task();
                }
            }
        );
    }
}

inline ThreadPool::~ThreadPool()
{
    std::cout << "关闭线程池\n"; 
    _is_stop = true;

    _condition.notify_all();

    for (auto &i : _ThreadPool)
        i.join();
}

template<class Func, class ...Argvs>
inline auto ThreadPool::add_task(Func&& func, Argvs&& ...argvs) -> std::future< typename std::result_of<Func(Argvs...)>::type >
{
    using func_return_type = typename std::result_of<Func(Argvs...)>::type;
    //std::packaged_task<func_return_type()>* tmp_func = new std::packaged_task<func_return_type()>(std::bind(std::forward<Func>(func), std::forward<Argvs>(argvs)...));
    auto tmp_func = std::make_shared< std::packaged_task<func_return_type()> >( std::bind(std::forward<Func>(func), std::forward<Argvs>(argvs)...) );
    std::future<func_return_type> tmp_func_return = tmp_func->get_future();

    {
        if (_is_stop)
            std::logic_error("线程池关闭");

        std::unique_lock<std::mutex>  tmp_lock(_mutex);
        _TaskS.emplace_back
        (
            [tmp_func]() -> void
            {
                (*tmp_func)();
            }
        );
    }

    _condition.notify_one();
    return tmp_func_return;
}


inline const int ThreadPool::get_task_size()
{
    return _TaskS.size();
}












