#ifndef OBSERVER_HPP
#define OBSERVER_HPP

// ~~~IMPORTANT this is a very naive observer implementation and should be improved in the future~~~
// Slightly better observer implementation I stole from HYZ0013. I still have doubts if this is the best implementation.

#include <vector>
#include <memory>
#include "Camera.hpp"

template<class T>
class Subject{
    public:
    std::vector<Observer<T>*> observers = std::vector<Observer<T>*>();

    void AddObserver(Observer<T>* observer){
        this->observers.push_back(observer);
    }

    void RemoveObserver(Observer<T>* observer){
        this->observers.erase(std::remove(this->observers.begin(), this->observers.end(), observer), this->observers.end());
    }

    void notifyObservers(){
        for (auto observer : this->observers){
            observer->update((T*)this)
        }
    }
};

template<class T>
class Observer{
    public:
    virtual void update(T* subject) = 0;
};

#endif