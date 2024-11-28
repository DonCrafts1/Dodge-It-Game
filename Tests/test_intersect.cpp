#include "../util.h"
#include "../game_object.h"
#include <iostream>
#include "../game_space.h"
using namespace std;

int main() {
    GameSpace gamespace;
    AcceleratingObject* fobj1 = gamespace.test_spawn_falling_obj(Position(0, 1));
    AcceleratingObject* fobj2 = gamespace.test_spawn_falling_obj(Position(0.5, 1.5));
    cout << "fobj1 intersect fobj2" << endl;
    cout << boolalpha << fobj1->intersects(fobj2) << endl;

    cout << endl << "fobj1 intersect fobj3" << endl;
    AcceleratingObject* fobj3 = gamespace.test_spawn_falling_obj(Position(5, 5));
    cout << boolalpha << fobj1->intersects(fobj3) << endl;
    
}
