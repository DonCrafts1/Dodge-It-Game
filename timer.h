#pragma once

#include <iostream>
#include <functional>
#include <map>

class Timer {
    long time_to_reach;
    long elapsed_time;
    public:
        Timer(long time_to_reach = 1000000, long elapsed_time = 0);
        
        bool is_over() const;
        virtual void update(long frameTime);
        virtual void reset();
        void set_time_to_reach(long time_to_reach);
        long get_time_to_reach() const;
        long get_time_elapsed() const;
        long get_time_remaining() const;
};

struct Action {
    std::function<void()> func;
    float proportion_to_activate;
    bool activated;
};

class ActionTimer : public Timer{
    std::map<std::string, Action> actions;
    public:
        ActionTimer(long time_to_reach = 1000000, long elapsed_time = 0);

        // I really really tried to use function pointers but I couldn't pass in lambda with capture :(
        void add_action(std::string name, std::function<void()> func, float proportion_to_activate);
        void remove_action(std::string name);
        void remove_action(float proportion_to_activate);
        virtual void update(long frameTime) override;
        virtual void reset() override;
        void clear_actions();
};