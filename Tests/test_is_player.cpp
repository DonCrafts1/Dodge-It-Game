#include "../util.h"
#include "../game_object.h"
#include <iostream>
#include "../game_space.h"
using namespace std;

int main() {
    GameSpace gamespace;
    GameObject* fobj1 = gamespace.test_spawn_falling_obj(Position(0, 1));
    GameObject* player = gamespace.get_player();

    cout << "Is fobj1 player? " << fobj1->is_player() << endl;
    cout << "Is player player? " << player->is_player() << endl;
    
}
