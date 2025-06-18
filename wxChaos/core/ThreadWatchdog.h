//
// Created by fisca on 17/06/2025.
//

#ifndef WXCHAOS_SLN_THREADWATCHDOG_H
#define WXCHAOS_SLN_THREADWATCHDOG_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "RenderFractal.h"

/**
* @class ThreadWatchdog
* @brief Control the execution of the threads.
*
* The watchdog main purpose is to control the execution and flow of the threads. It provides methods to watch their status,
* stop them, reset them and relaunch them.
* @tparam MT Must be a RenderFractal inherited class.
*/
template<class MT> class ThreadWatchdog : public sf::Thread
{
    MT** threadList;               ///< An array with pointers to the execution threads.
    sf::Thread** sfmlThreads;      ///< An array to hold the actual sf::Thread objects.
    bool threadRunning;            ///< State of the threads.
    unsigned int threadCounter;    ///< Number of threads to watch over.
public:
    ThreadWatchdog();
    ~ThreadWatchdog();

    virtual void run();

    ///@brief Changes the number of execution threads. For this it will have to delete the previous ones.
    ///@param nThreads Number of new threads.
    void SetThreadNumber(int nThreads);

    ///@brief Sets a new thread to watch over.
    ///@param threadAdress Pointer to the thread to watch over.
    void SetThread(MT* threadAdress);

    ///@brief Resets the RenderFractal.
    void Reset();

    ///@brief Launch all the threads in the threadList.
    void LaunchThreads();

    ///@brief Stops all the threads in the threadList.
    void StopThreads();

    ///@brief Informs if there is a thread running.
    ///@return true if there is a thread running. false if not.
    bool ThreadRunning();

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
* @param MT Must be a RenderFractal inherited class.
* @param myRender Array of render threads.
* @param watchdog Pointer to the watchdog that will be used.
* @param threadNumber Number of threads to set.
*/
template<class MT> inline void SetWatchdog(MT* myRender, ThreadWatchdog<RenderFractal>* watchdog, unsigned int threadNumber)
{
    watchdog->SetThreadNumber(threadNumber);
    for (unsigned int i = 0; i < threadNumber; i++)
        watchdog->SetThread(&myRender[i]);
}

template<class MT> ThreadWatchdog<MT>::ThreadWatchdog() : sf::Thread(&ThreadWatchdog<MT>::run, this)
{
    threadCounter = 0;
    threadRunning = false;
    threadList = nullptr;
    sfmlThreads = nullptr;
}
template<class MT> ThreadWatchdog<MT>::~ThreadWatchdog()
{
    if (threadList != nullptr)
        delete[] threadList;
    if (sfmlThreads != nullptr)
    {
        // Ensure threads are stopped and deleted
        if (threadRunning) StopThreads();
        delete[] sfmlThreads;
    }
}
template<class MT> void ThreadWatchdog<MT>::SetThreadNumber(int nThreads)
{
    if (threadList != nullptr)
    {
        delete[] threadList;
    }
    if (sfmlThreads != nullptr)
    {
        delete[] sfmlThreads;
    }
    threadCounter = 0;
    threadRunning = false;

    threadList = new MT * [nThreads];
    sfmlThreads = new sf::Thread * [nThreads]; // Allocate for sf::Thread pointers
    for (int i = 0; i < nThreads; ++i)
    {
        sfmlThreads[i] = nullptr; // Initialize to null
    }
}
template<class MT> void ThreadWatchdog<MT>::SetThread(MT* threadAdress)
{
    threadList[threadCounter++] = threadAdress;
}
template<class MT> void ThreadWatchdog<MT>::run()
{
    // We don't want to collapse our system.
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#endif
    // Wait for every thread to finish and change status.
    for (unsigned int i = 0; i < threadCounter; i++)
    {
        if (sfmlThreads[i])
        {
            sfmlThreads[i]->wait();
            delete sfmlThreads[i]; // Clean up after it's done
            sfmlThreads[i] = nullptr;
        }
    }

    threadRunning = false;
#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
template<class MT> void ThreadWatchdog<MT>::Reset()
{
    for (unsigned int i = 0; i < threadCounter; i++)
        threadList[i]->Reset();

    threadRunning = true;
}
template<class MT> bool ThreadWatchdog<MT>::ThreadRunning()
{
    return threadRunning;
}
template<class MT> void ThreadWatchdog<MT>::LaunchThreads()
{
    // Launches all the threads.
    threadRunning = true;
    for (unsigned int i = 0; i < threadCounter; i++)
    {
        // Create a new thread that will call the run() method of our RenderFractal object
        sfmlThreads[i] = new sf::Thread(&RenderFractal::run, threadList[i]);
        sfmlThreads[i]->launch();
    }
}
template<class MT> void ThreadWatchdog<MT>::StopThreads()
{
    for (unsigned int i = 0; i < threadCounter; i++)
    {
        threadList[i]->PreTerminate();
        threadList[i]->Stop();
    }

    for (unsigned int i = 0; i < threadCounter; i++)
    {
        if (sfmlThreads[i])
        {
            sfmlThreads[i]->wait();
            delete sfmlThreads[i];
            sfmlThreads[i] = nullptr;
        }
    }

    threadRunning = false;

#ifdef _WIN32
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}
template<class MT> int ThreadWatchdog<MT>::GetThreadProgress()
{
    int progress = 0;
    for (unsigned int i = 0; i < threadCounter; i++)
        progress += threadList[i]->AskProgress();

    return (double)progress / (double)threadCounter;
}
template<class MT> MT* ThreadWatchdog<MT>::GetThread(unsigned int nThread)
{
    if (nThread >= 0 && nThread < threadCounter)
        return threadList[nThread];
    else
        return nullptr;
}

#endif //WXCHAOS_SLN_THREADWATCHDOG_H
