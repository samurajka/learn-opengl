#ifndef OBSERVER_HPP
#define OBSERVER_HPP

// IMPORTANT this is a very naive observer implementation and should be improved in the future

#include <vector>
#include <memory>
#include "Camera.hpp"

class Subject{
    public:
    std::vector<std::shared_ptr<Observer>> observers;

    void Attach(std::shared_ptr<Observer> observer){
        this->observers.push_back(observer);
        this->NotifyObservers();
    };
    virtual void NotifyObservers() = 0;
};

class Observer{
    public:
    virtual void update(Camera &camera) = 0;
};

#endif