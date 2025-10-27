#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

// 线程池类，管理多个工作线程以执行任务
class ThreadPool
{
private:
    std::vector<std::thread> threads; // 工作线程
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex tasks_mtx; // 任务队列互斥锁
    std::condition_variable cv; // 条件变量，用于线程同步
    bool stop; // 停止标志
public:

    /**
    * @brief 构造函数初始化线程池并启动工作线程。
    * @param size 线程池大小，默认为硬件线程数
    */
    ThreadPool(int size = std::thread::hardware_concurrency());

    /**
     * @brief 销毁线程池
     * @brief 停止所有线程并等待它们完成当前任务
     */
    ~ThreadPool();

    /**
     * @brief 向线程池中添加一个任务
     * @param f 任务函数
     */
    template<class F, class... Args>
    auto add(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>;

};


//不能放在cpp文件，C++编译器不支持模版的分离编译
/**
 * @brief 向线程池中添加一个任务并返回其 future 对象。
 * @tparam F 可调用对象的类型。
 * @tparam Args 可调用对象的参数类型。
 * @param f 可调用对象（函数、lambda 表达式等）。
 * @param args 可调用对象的参数。
 * @return std::future<typename std::result_of<F(Args...)>::type> 任务的 future 对象，用于获取任务执行结果。
 * @throws std::runtime_error 如果线程池已停止，则抛出异常。
 * @note 任务会被封装为 std::packaged_task，并通过 std::future 返回结果。
 */
template<class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    cv.notify_one();
    return res;
}