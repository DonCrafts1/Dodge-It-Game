#include "timer.h"

Timer::Timer(long time_to_reach, long elapsed_time) : time_to_reach(time_to_reach), elapsed_time(elapsed_time)
{
    
}

bool Timer::is_over() const
{
    return elapsed_time >= time_to_reach;
}

void Timer::update(long update_time)
{
    elapsed_time += update_time;
}

void Timer::reset()
{
    elapsed_time = 0;
}

void Timer::set_time_to_reach(long time_to_reach)
{
    if (time_to_reach <= 0) {
        return;
    }
    this->time_to_reach = time_to_reach;
}

long Timer::get_time_to_reach() const
{
    return time_to_reach;
}

long Timer::get_time_elapsed() const
{
    return elapsed_time;
}

long Timer::get_time_remaining() const
{
    return time_to_reach - elapsed_time;
}

ActionTimer::ActionTimer(long time_to_reach, long elapsed_time) : Timer(time_to_reach, elapsed_time)
{
}

void ActionTimer::add_action(std::string name, std::function<void()> func, float proportion_to_activate)
{
    if (proportion_to_activate > 1 || proportion_to_activate < 0) {
        return;
    }
    Action action {func, proportion_to_activate, false};
    actions.insert(std::pair<std::string, Action>(name, action));
}

void ActionTimer::remove_action(std::string name)
{
    actions.erase(name);
}

void ActionTimer::remove_action(float proportion_to_activate) 
{
    if (proportion_to_activate > 1 || proportion_to_activate < 0) {
        return;
    }
    std::string name;
    for (const std::pair<const std::string, Action>& action_pair : actions) {
        if (action_pair.second.proportion_to_activate == proportion_to_activate) {
            name = action_pair.first;
            break;
        }
    }
    actions.erase(name);
}

void ActionTimer::update(long frameTime)
{
    Timer::update(frameTime);

    for (std::pair<const std::string, Action>& action_pair : actions) {
        if (action_pair.second.activated) {
            continue;
        }
        if (action_pair.second.proportion_to_activate < ((double)get_time_elapsed() / get_time_to_reach())) {
            action_pair.second.func();
            action_pair.second.activated = true;
        }
    }
}

void ActionTimer::reset()
{
    Timer::reset();
    for (auto& action_pair : actions) { // I use auto here
        action_pair.second.activated = false;
    }
}

void ActionTimer::clear_actions()
{
    actions.clear();
}
