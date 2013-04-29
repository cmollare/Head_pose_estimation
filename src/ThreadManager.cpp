#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	_maxNumberThreads = boost::thread::hardware_concurrency();
    _currentThreads.reserve(_maxNumberThreads+2);//au cas où
    //_maxNumberThreads = 0;
    //_maxNumberThreads*=2;

    _check=1;
	
    _testThread = new boost::thread(&ThreadManager::tryJoin, this);
	

}

ThreadManager::~ThreadManager()
{
	this->waitTheEnd();
    _testThread->join();
	delete _testThread;
}

boost::thread* ThreadManager::addThread(boost::thread* thread)
{
    boost::mutex::scoped_lock(_mute);
	_currentThreads.push_back(thread);
}

int ThreadManager::getMaxNumberThreads()
{
    boost::mutex::scoped_lock(_mute);
	return _maxNumberThreads;
}

int ThreadManager::getCurrentNumberThreads()
{
    boost::mutex::scoped_lock(_mute);
    return _currentThreads.size();
}

void ThreadManager::tryJoin()
{
	for(;;)
	{
        boost::posix_time::milliseconds workTime(150);
		boost::this_thread::sleep(workTime);
		
        _mute.lock();
		if (_currentThreads.size()>0)
		{
            for (std::vector<boost::thread*>::iterator it = _currentThreads.begin() ; it != _currentThreads.end() ; it++)
			{

                if(!(*it))
                {
                    _mute.lock();
                    std::cout << "Attention !!!" << std::endl;
                    boost::posix_time::seconds workTime(120);
                    boost::this_thread::sleep(workTime);
                    _currentThreads.erase(it);
                    it--;
                    _mute.unlock();
                }
                _mute.unlock();

                if ((*it)->timed_join(boost::posix_time::milliseconds(10)))
				{
                    _mute.lock();
                    std::cout << "thread terminated" << std::endl;
                    delete *it;
                    _currentThreads.erase(it);
                    it--;
                    _mute.unlock();
				}
                _mute.lock();

			}
		}
		else if(!_check) break;
        _mute.unlock();
	}
	
    _mute.lock();
	_check=1;
    _mute.unlock();
}

void ThreadManager::waitTheEnd()
{
	_mute.lock();
	_check=0;
    _mute.unlock();

    std::cout << "Wait The End" << std::endl;
	
	for(;;)
	{
		boost::posix_time::seconds workTime(10);
		boost::this_thread::sleep(workTime);
		
        if (_testThread->timed_join(boost::posix_time::milliseconds(100))) break;
	}
}

