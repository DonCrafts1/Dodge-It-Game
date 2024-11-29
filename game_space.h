#pragma once
#include <iostream>

#ifdef _WIN32
#include <ncurses/ncurses.h>
#elif __APPLE__ || defined(LINUX)
#include "ncurses.h"
#else
# error "Unknown compiler"
#endif

#include <set>
#include <list>
#include "spawn_object.h"
#include "player.h"
#include "timer.h"
#include "util.h"

enum class Difficulty {
    NotSet,
    Easy = 30,
    Medium = 50,
    Hard = 75
};

std::ostream& operator<<(std::ostream& os, const Difficulty& difficulty);

class CollisionCell {
    int x;
    int y;
    std::set<GameObject*> entities;
    // std::vector<GameObject*> entities;

    public:
        CollisionCell(int x = 0, int y = 0);
        void setX(int x);
        void setY(int y);
        int getX() const;
        int getY() const;
        void clear_entities();
        void add_entity(GameObject* entity);
        void check_collision();

        int get_num_of_entities() const;
};

constexpr size_t COLLISION_DIVISION = 25;
constexpr size_t COLLISION_GRID_X = static_cast<size_t>(MAX_X) / COLLISION_DIVISION + (static_cast<size_t>(MAX_X) % COLLISION_DIVISION > 0 ? 2 : 1);
constexpr size_t COLLISION_GRID_Y = static_cast<size_t>(MAX_Y) / COLLISION_DIVISION + (static_cast<size_t>(MAX_Y) % COLLISION_DIVISION > 0 ? 2 : 1);
class CollisionDetection {
    private:
        std::array<std::array<CollisionCell, COLLISION_GRID_X>, COLLISION_GRID_Y> cells;
        void clear_cells();
        void check_cell_collisions();

    public:
        CollisionDetection();
        void update(const std::list<GameObject*>& entities);
        void print(WINDOW* window);
        
};

struct GameResults {
    Difficulty difficulty;
    long time_elapsed;
    bool won;
    int lives_remaining;
};

constexpr long SPAWN_FALLING_OBJECT_COOLDOWN = 500000;
constexpr int REFRESH_PHYSICS_FACTOR = 1;
class GameSpace {
    Difficulty difficulty;
    Player* player;
    std::list<GameObject*> entities;
    std::list<GameObject*> entities_to_delete;
    Timer game_timer;
    bool test_mode;
    int num_deleted_entities = 0;

    CollisionDetection collision_detector;
    long get_next_object_spawn_time();
    
    // void update_screen();
    public:
        GameSpace(Difficulty difficulty = Difficulty::Easy, bool test_mode = false);
        ~GameSpace();
        bool update(long frame_time);
        Player* get_player();
        Player* get_player() const;
        long get_time_elapsed() const;
        GameResults get_game_results() const;

        void spawn_falling_obj_random();
        AcceleratingObject* test_spawn_falling_obj(Position position);
        void set_difficulty(Difficulty difficulty);
        void print(WINDOW* window);
        void reset(Difficulty difficulty, bool test_mode);
};
