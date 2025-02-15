#ifndef VGL_TASK_DISPATCHER_HPP
#define VGL_TASK_DISPATCHER_HPP

#include "vgl_fence_pool.hpp"
#include "vgl_task_queue.hpp"
#include "vgl_thread.hpp"
#include "vgl_vector.hpp"

#if defined(VGL_USE_LD)
#include <sstream>
#endif

namespace vgl
{

namespace detail
{

/// Task queue class.
class InternalTaskDispatcher
{
private:
    /// Threads created for this task queue.
    vector<Thread> m_threads;

    /// Queue for tasks (any thread).
    TaskQueue m_tasks_any;

    /// Queue for tasks (main thread).
    TaskQueue m_tasks_main;

    /// Pool of free data structures for fences.
    FencePool m_fence_pool;

    /// Guard mutex.
    Mutex m_mutex = Mutex(nullptr);

    /// Main thread ID.
    Thread::id_type m_main_thread_id = 0;

    /// Target concurrency level.
    unsigned m_concurrency = 0;

    /// Number of threads active currently.
    unsigned m_threads_active = 0;

    /// Number of threads waiting for tasks to execute.
    unsigned m_threads_waiting = 0;

#if defined(VGL_USE_LD)
    /// Flag signifying the task queue is being destroyed.
    ///
    /// The flag is disabled for optimized build, because the program should never exit cleanly.
    bool m_quitting = false;
#endif

public:
    /// Default constructor.
    constexpr explicit InternalTaskDispatcher() = default;

    /// Destructor.
    ~InternalTaskDispatcher()
    {
#if defined(VGL_USE_LD)
#if defined(DEBUG)
        if(!m_mutex.getMutexImpl())
        {
            if((m_main_thread_id != 0) ||
                    !m_tasks_any.empty() ||
                    !m_tasks_main.empty() ||
                    !m_threads.empty() ||
                    !m_fence_pool.empty())
            {
                VGL_THROW_RUNTIME_ERROR("task queue was never initialized but is not at initial state");
            }
        }
        else
#endif
        {
            {
                ScopedAcquire sa(m_mutex);
                m_quitting = true;
                m_tasks_any.uninitialize();
                m_tasks_main.uninitialize();
            }

            // Threads must be joined before destroying anything else.
            m_threads.clear();
        }
#endif
    }

    /// Deleted copy constructor.
    InternalTaskDispatcher(const InternalTaskDispatcher&) = delete;
    /// Deleted assignment.
    InternalTaskDispatcher& operator=(const InternalTaskDispatcher&) = delete;

private:
    /// Acquire or reuse fence data (locked).
    ///
    /// \return Fence data structure to use.
    FenceData* acquireFenceDataSafe()
    {
        ScopedAcquire sa(m_mutex);
        return m_fence_pool.acquire();
    }

    /// Immediately execute given task function.
    ///
    /// Return an inactive fence containing the return value.
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    Fence immediateDispatch(TaskFunc func, void* params)
    {
        FenceData* ret = acquireFenceDataSafe();
        ret->setActive(false);
        ret->setReturnValue(func(params));
        return Fence(ret);
    }

    /// Internally wait (create a fence) and dispatch.
    ///
    /// \param queue Internal task queue.
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    FenceData* internalDispatch(TaskQueue& task_queue, TaskFunc func, void* params)
    {
        FenceData* ret = m_fence_pool.acquire();
        ret->setActive(true);
        ret->setReturnValue(nullptr);
        task_queue.emplace(ret, func, params);
        return ret;
    }

    /// Is the calling thread the main thread.
    ///
    /// \return True if yes, false if no.
    bool isMainThread() const
    {
        return (m_main_thread_id == Thread::get_current_thread_id());
    }

    /// Is this a spawned thread?
    bool isSpawnedThread()
    {
        Thread::id_type current_thread_id = Thread::get_current_thread_id();
        for(const auto& vv : m_threads)
        {
            if(vv.getId() == current_thread_id)
            {
                return true;
            }
        }
        return false;
    }

    /// Spawn a new thread.
    ///
    /// Thread that has not entered execution is considered waiting.
    void spawnThread()
    {
#if defined(VGL_USE_LD)
        string threadName = "InternalTaskDispatcher(" + to_string(m_threads.size()) + ")";
        m_threads.emplace_back(task_thread_func, this, threadName.c_str());
#else
        m_threads.emplace_back(task_thread_func, this);
#endif
        ++m_threads_waiting;
    }
    /// Spawns a thread if it's necessary.
    ///
    /// Must be out of waiting threads and below concurrency limit.
    void spawnThreadIfBelowConcurrency()
    {
        if((m_threads_waiting < m_tasks_any.size()) &&
                (m_threads.size() < m_concurrency))
        {
            spawnThread();
        }
    }

    /// Thread function.
    /// \return Thread return value.
    Thread::return_type threadFunc()
    {
        ScopedAcquire sa(m_mutex);
        --m_threads_waiting;

#if defined(VGL_USE_LD)
        while(!m_quitting)
#else
        for(;;)
#endif
        {
            if((m_threads_active < m_concurrency) && !m_tasks_any.empty())
            {
                ++m_threads_active;
                {
                    // Release lock for the duration of executing the task.
                    Task task = m_tasks_any.acquire();
                    sa.release();
                    task();
                }
                sa.acquire();
                --m_threads_active;
            }
            else
            {
                ++m_threads_waiting;
                m_tasks_any.wait(sa);
                --m_threads_waiting;
            }
        }

        return 0;
    }

public:
    /// Initialize the task queue.
    ///
    /// \param op Number of threads to initialize.
    void initialize(unsigned op)
    {
        m_concurrency = op;
        m_main_thread_id = Thread::get_current_thread_id();

        m_tasks_any.initialize();
        m_tasks_main.initialize();
        m_mutex = Mutex();
    }

    /// Gets a main context task.
    ///
    /// \return Main context task.
    Task acquireMainTask()
    {
        ScopedAcquire sa(m_mutex);

#if defined(VGL_USE_LD)
        while(!m_quitting)
#else
        for(;;)
#endif
        {
            if(m_tasks_main.empty())
            {
                m_tasks_main.wait(sa);
            }
            else
            {
                return m_tasks_main.acquire();
            }
        }

        return Task();
    }

    /// Mark fence data as inactive (from locked context) and signal threads waiting on it.
    ///
    /// \param op Fence data.
    void fenceSignal(FenceData& op)
    {
        {
            ScopedAcquire sa(m_mutex);
            op.setActive(false);
        }
        op.signal();
    }

    /// Wait on a fence internal state.
    ///
    /// \param op Fence.
    /// \return Stored return value from the fence.
    void* fenceWait(Fence& op)
    {
        ScopedAcquire sa(m_mutex);

        // Fence may have turned inactive before the wait point is reached.
        if(op)
        {
#if defined(VGL_USE_LD) && defined(DEBUG)
            if(isMainThread())
            {
                VGL_THROW_RUNTIME_ERROR("cannot wait on main thread");
            }
#endif

            bool is_spawned = isSpawnedThread();

            // If waiting would lock the last concurrent thread, spawn a new thread.
            if(is_spawned)
            {
                if(m_threads_waiting <= 0)
                {
                    spawnThread();
                }
                --m_threads_active;
            }

            m_tasks_any.signal();
            op.wait(sa);

            if(is_spawned)
            {
                ++m_threads_active;
            }
        }

        FenceData* data = op.releaseData();
        m_fence_pool.emplace(data);
        return data->getReturnValue();
    }

    /// Dispatch a task (any thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    void dispatch(TaskFunc func, void* params)
    {
        ScopedAcquire sa(m_mutex);
        m_tasks_any.emplace(func, params);
        spawnThreadIfBelowConcurrency();
    }
    /// Dispatch a task (main thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    void dispatchMain(TaskFunc func, void* params)
    {
        ScopedAcquire sa(m_mutex);
        m_tasks_main.emplace(func, params);
    }

    /// Dispatch a task and wait for it to complete (any thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    /// \return Fence.
    Fence wait(TaskFunc func, void* params)
    {
        // Prevent deadlock - main thread cannot wait.
        if(isMainThread())
        {
            return immediateDispatch(func, params);
        }

        ScopedAcquire sa(m_mutex);
        FenceData* data = internalDispatch(m_tasks_any, func, params);
        spawnThreadIfBelowConcurrency();
        return Fence(data);
    }
    /// Dispatch a task and wait for it to complete (main thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    /// \return Fence.
    Fence waitMain(TaskFunc func, void* params)
    {
        // Prevent deadlock - main thread cannot wait.
        if(isMainThread())
        {
            return immediateDispatch(func, params);
        }

        ScopedAcquire sa(m_mutex);
        FenceData* data = internalDispatch(m_tasks_main, func, params);
        return Fence(data);
    }

private:
    /// Task dispatcher thread.
    ///
    /// \param op Pointer to task queue.
    static Thread::return_type task_thread_func(void* op)
    {
        return static_cast<InternalTaskDispatcher*>(op)->threadFunc();
    }
};

}

/// Task dispatcher interface class.
class TaskDispatcher
{
private:
    /// Global queue for tasks.
    static detail::InternalTaskDispatcher g_instance;

public:
    /// Initialize task system.
    ///
    /// \param op Concurrency level.
    static void initialize(unsigned op)
    {
        g_instance.initialize(op);
    }

    /// Signal fence data.
    ///
    /// \param op Fence data.
    static void fence_data_signal(detail::FenceData& op)
    {
        g_instance.fenceSignal(op);
    }

    /// Wait on fence.
    ///
    /// \param op Fence data.
    static void* fence_wait(Fence& op)
    {
        return g_instance.fenceWait(op);
    }

    /// Get a main loop task.
    ///
    /// This function blocks if main loop tasks are not available.
    ///
    /// \return New main loop task.
    static Task acquire_main()
    {
        return g_instance.acquireMainTask();
    }

    /// Dispatch task (any thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    static void dispatch(TaskFunc func, void* params)
    {
        g_instance.dispatch(func, params);
    }

    /// Dispatch task (main thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    static void dispatch_main(TaskFunc func, void* params)
    {
        g_instance.dispatchMain(func, params);
    }

    /// Wait on a task (any thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    /// \return Fence.
    static Fence wait(TaskFunc func, void* params)
    {
        return g_instance.wait(func, params);
    }

    /// Wait on a task (main thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    /// \return Fence.
    static Fence wait_main(TaskFunc func, void* params)
    {
        return g_instance.waitMain(func, params);
    }
};

namespace detail
{

/// Internal signal of fence data.
///
/// \param op Fence data.
inline void internal_fence_data_signal(FenceData& op)
{
    TaskDispatcher::fence_data_signal(op);
}

/// Internal wait for fence data on the task dispatcher.
///
/// \param op Fence data.
inline void* internal_fence_wait(Fence& op)
{
    return TaskDispatcher::fence_wait(op);
}

}

}

#if !defined(VGL_USE_LD)
#include "vgl_task_dispatcher.cpp"
#endif

#endif
