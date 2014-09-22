/**
 * @filename observer.h
 * @brief Observer模式
 * @author macwe1024 at gmail dot com
 */
 
#ifndef _THEFOX_BASE_OBSERVER_H_
#define _THEFOX_BASE_OBSERVER_H_

#include <set>

namespace thefox {

class Observable;

class Observer
{
public:
    virtual ~Observer() {}

    virtual void update(Observable *p, void *arg) = 0;
};


class Observable
{
public:
    Observable() 
		: _observers(false) {}

    virtual ~Observable() {}

    virtual bool addObserver(Observer *obj)
    {
        std::pair<std::set<Observer *>::iterator, bool> ret = _observers.insert(obj);
        return ret.second;
    }

    virtual bool deleteObserver(Observer *obj)
    {
        std::set<Observer *>::iterator it = _observers.find(obj);
        if (it != _observers.end()) {
            _observers.erase(it);
            return true;
        }
        return false;
    }

    virtual void notifyObservers(void* arg = 0)
    {
        if (_changed) {
            std::set<Observer *>::iterator it = _observers.begin();
            std::set<Observer *>::iterator itEnd = _observers.end();
            for (; it != itEnd; ++it) {
                (*it)->update(this, arg);
            }
            _changed = false;
        }
    }

    virtual void setChanged()
    {
        _changed = true;
    }

protected:
    std::set<Observer *> _observers;
    bool _changed;
};

} // namespace thefox

#endif // _THEFOX_BASE_OBSERVER_H_
