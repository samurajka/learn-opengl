#ifndef OBSERVER_HPP
#define OBSERVER_HPP

// ~~~IMPORTANT this is a very naive observer implementation and should be improved in the future~~~
// Slightly better observer implementation I stole from HYZ0013. I still have doubts if this is the best implementation.
// TODO: change the classes so they can also be used using the composition pattern. ~~~Use smart pointers~~~

#include <vector>
#include <memory>

template<class T>
class Observer{
    public:
    virtual void update(T& subject) = 0;
};

template<class T>
class Subject{
    public:
    std::vector<std::shared_ptr<Observer<T>>> observers;

    void AddObserver(std::shared_ptr<Observer<T>> observer){
        this->observers.push_back(observer);
    }

    void RemoveObserver(std::shared_ptr<Observer<T>> observer){
        this->observers.erase(std::remove(this->observers.begin(), this->observers.end(), observer), this->observers.end());
    }

    void notifyObservers(){
        for (auto observer : this->observers){
            observer->update(static_cast<T&>(*this));
        }
    }
};



#endif