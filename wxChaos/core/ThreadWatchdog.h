#ifndef WXCHAOS_SLN_THREAD_WATCHDOG_H
#define WXCHAOS_SLN_THREAD_WATCHDOG_H

#include <SFML/System.hpp>
#include "RenderFractal.h"

/**
* @class ThreadWatchdog
* @brief Control the execution of the threads.
*
* The watchdog's main purpose is to control the execution and flow of the threads. It provides methods to watch their status,
* stop them, reset them, and relaunch them.
* @tparam MT Must be a RenderFractal inherited class.
*/
template<class MT> class ThreadWatchdog : public sf::Thread
{
    MT** _threadList;               ///< An array with pointers to the execution threads.
    Thread** _sfmlThreads;          ///< An array to hold the actual sf::Thread objects.
    bool _threadRunning;            ///< State of the threads.
    unsigned int _threadCounter;    ///< Number of threads to watch over.
public:
    ThreadWatchdog();
    ~ThreadWatchdog();

    virtual void run();

    ///@brief Changes the number of execution threads. For this it will have to delete the previous ones.
    ///@param threadNumber Number of new threads.
    void SetThreadNumber(int threadNumber);

    ///@brief Sets a new thread to watch over.
    ///@param threadAddress Pointer to the thread to watch over.
    void SetThread(MT* threadAddress);

    ///@brief Resets the RenderFractal.
    void Reset();

    ///@brief Launch all the threads in the threadList.
    void LaunchThreads();

    ///@brief Stops all the threads in the threadList.
    void StopThreads();

    ///@brief Informs if there is a thread running.
    ///@return true if there is a thread running. false if not.
    bool ThreadRunning() const;

    ///@brief Ask the RenderFractal the render progress.
    ///@return A integer from 0 to 100 that is the progress.
    int GetThreadProgress();

    ///@brief Get the thread in the specified index.
    ///@param nThread Index of the thread to return.
    ///@return A pointer to the specified thread index.
    MT* GetThread(unsigned int nThread);
};

/**
* @brief Sets the watchdog for the specified threads.
* @param myRender Array of render threads.
* @param watchdog Pointer to the watchdog that will be used.
* @param threadNumber Number of threads to set.
*/
template<class MT> void SetWatchdog(MT* myRender, ThreadWatchdog<RenderFractal>* watchdog, const unsigned int threadNumber)
{
    watchdog->SetThreadNumber(threadNumber);
    for (unsigned int i = 0; i < threadNumber; i++)
        watchdog->SetThread(&myRender[i]);
}

template<class MT> ThreadWatchdog<MT>::ThreadWatchdog() : Thread(&ThreadWatchdog::run, this)
{
    _threadCounter = 0;
    _threadRunning = false;
    _threadList = nullptr;
    _sfmlThreads = nullptr;
}
template<class MT> ThreadWatchdog<MT>::~ThreadWatchdog()
{
    if (_threadList != nullptr)
        delete[] _threadList;
    if (_sfmlThreads != nullptr)
    {
        // Ensure threads are stopped and deleted
        if (_threadRunning) StopThreads();
        delete[] _sfmlThreads;
    }
}
template<class MT> void ThreadWatchdog<MT>::SetThreadNumber(const int threadNumber)
{
    if (_threadList != nullptr)
    {
        delete[] _threadList;
    }
    if (_sfmlThreads != nullptr)
    {
        delete[] _sfmlThreads;
    }
    _threadCounter = 0;
    _threadRunning = false;

    _threadList = new MT * [threadNumber];
    _sfmlThreads = new Thread * [threadNumber]; // Allocate for sf::Thread pointers
    for (int i = 0; i < threadNumber; ++i)
    {
        _sfmlThreads[i] = nullptr; // Initialize to null
    }
}
template<class MT> void ThreadWatchdog<MT>::SetThread(MT* threadAddress)
{
    _threadList[_threadCounter++] = threadAddress;
}
template<class MT> void ThreadWatchdog<MT>::run()
{
    // Wait for every thread to finish and change status.
    for (unsigned int i = 0; i < _threadCounter; i++)
    {
        if (_sfmlThreads[i])
        {
            _sfmlThreads[i]->wait();
            delete _sfmlThreads[i]; // Clean up after it's done
            _sfmlThreads[i] = nullptr;
        }
    }

    _threadRunning = false;
}
template<class MT> void ThreadWatchdog<MT>::Reset()
{
    for (unsigned int i = 0; i < _threadCounter; i++)
        _threadList[i]->Reset();

    _threadRunning = true;
}
template<class MT> bool ThreadWatchdog<MT>::ThreadRunning() const
{
    return _threadRunning;
}
template<class MT> void ThreadWatchdog<MT>::LaunchThreads()
{
    // Launches all the threads.
    _threadRunning = true;
    for (unsigned int i = 0; i < _threadCounter; i++)
    {
        // Create a new thread that will call the run() method of our RenderFractal object
        _sfmlThreads[i] = new Thread(&RenderFractal::run, _threadList[i]);
        _sfmlThreads[i]->launch();
    }
}
template<class MT> void ThreadWatchdog<MT>::StopThreads()
{
    for (unsigned int i = 0; i < _threadCounter; i++)
    {
        _threadList[i]->PreTerminate();
        _threadList[i]->Stop();
    }

    for (unsigned int i = 0; i < _threadCounter; i++)
    {
        if (_sfmlThreads[i])
        {
            _sfmlThreads[i]->wait();
            delete _sfmlThreads[i];
            _sfmlThreads[i] = nullptr;
        }
    }

    _threadRunning = false;
}
template<class MT> int ThreadWatchdog<MT>::GetThreadProgress()
{
    int progress = 0;
    for (unsigned int i = 0; i < _threadCounter; i++)
        progress += _threadList[i]->AskProgress();

    return (double)progress / (double)_threadCounter;
}
template<class MT> MT* ThreadWatchdog<MT>::GetThread(unsigned int nThread)
{
    return nThread < _threadCounter ? _threadList[nThread] : nullptr;
}

#endif
