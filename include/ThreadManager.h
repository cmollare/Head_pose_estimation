#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <iostream>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/chrono.hpp>
#include <vector>

class ThreadManager
{
    public:
        ThreadManager();
        ~ThreadManager();
        boost::thread* addThread(boost::thread* thread);
        int getMaxNumberThreads();
        int getCurrentNumberThreads();
        void tryJoin();
        void waitTheEnd();

        boost::recursive_mutex _mute;

    protected:
        std::vector<boost::thread*> _currentThreads;
        int _maxNumberThreads;
        int _check;
        boost::thread* _testThread;

};

#endif
