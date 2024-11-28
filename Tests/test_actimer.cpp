#include <iostream>
#include <thread>
#include <chrono>
#include "../timer.h"
using namespace std;


int main() {
    const double frameTime = 1000000/60.0;
    ActionTimer action_timer(1000000, 0);

    action_timer.add_action("hi", [&action_timer]() {cout << "hi! I executed at: " << action_timer.get_time_elapsed() << endl; }, 0.25);
    action_timer.add_action("hehe", [&action_timer]() {cout << "hehe! I executed at: " << action_timer.get_time_elapsed() << endl;}, 0.75);

    cout << action_timer.get_time_to_reach() << endl;

    while (true) {
        action_timer.update(frameTime);
        if (action_timer.is_over()) {
            action_timer.reset();
        }
        this_thread::sleep_for(chrono::microseconds((long)frameTime));
    }
    
}
