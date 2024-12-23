#include "game_space.h"

GameSpace::GameSpace(Difficulty difficulty, bool test_mode) : difficulty(difficulty), player(new Player(test_mode)), entities(), game_timer(static_cast<long>(difficulty) * MILLION), test_mode(test_mode)
{
    entities.push_front(player);
}

GameSpace::~GameSpace() {
    for (GameObject*& entity : entities) {
        delete entity;
        entity = nullptr;
    }
    for (GameObject*& entity : entities_to_delete) {
        delete entity;
        entity = nullptr;
    }
    entities.clear(); entities_to_delete.clear();
    player = nullptr; // already deleted, inside entities vector
}

long GameSpace::get_next_object_spawn_time()
{
    double factor = (1.1 - 0.5 * game_timer.get_time_elapsed() / game_timer.get_time_to_reach()) 
        / static_cast<double>(difficulty) * static_cast<double>(Difficulty::Easy);
    return SPAWN_FALLING_OBJECT_COOLDOWN * factor;
}

Timer spawn_timer;
constexpr int refresh_physics_factor = REFRESH_PHYSICS_FACTOR <= 0 ? 1 : REFRESH_PHYSICS_FACTOR;
bool GameSpace::update(long frame_time) {
    bool game_over = false;
    frame_time /= refresh_physics_factor;
    for (int i = 0; i < refresh_physics_factor; i++) {
        game_timer.update(frame_time);
        spawn_timer.update(frame_time);
        if (game_timer.is_over()) {
            game_over = true;
            break;
        }
        if (spawn_timer.is_over()) { // time to spawn new object
            spawn_falling_obj_random();
            spawn_timer.set_time_to_reach(get_next_object_spawn_time());
            spawn_timer.reset();
        }
        
        for (GameObject* entity : entities) {
            entity->update(frame_time);
        }
        collision_detector.update(entities);
        for (GameObject* entity : entities) {
            if (entity == nullptr) {
                continue;
            }
            if (entity->is_deletable()) {
                entities_to_delete.push_front(entity);
                // If player delete (dies), game over!
                if (entity->is_player()) {
                    player = nullptr;
                    game_over = true;
                }
            }
        }
        for (GameObject*& deletable_entity : entities_to_delete) {
            entities.remove(deletable_entity);
            delete deletable_entity;
            deletable_entity = nullptr;
            num_deleted_entities++;
        }
        entities_to_delete.clear();
    }
    
    return game_over;
}

Player* GameSpace::get_player() {
    return player;
}

Player* GameSpace::get_player() const
{
    return player;
}


long GameSpace::get_time_elapsed() const
{
    return game_timer.get_time_elapsed();
}


GameResults GameSpace::get_game_results() const
{
    return GameResults {  
        difficulty, 
        get_time_elapsed(), 
        get_time_elapsed() >= game_timer.get_time_to_reach(),
        get_player() != nullptr ? get_player()->get_health() : 0
    };
}

void GameSpace::spawn_falling_obj_random()
{
    int posX = rand() % 100;
    int size_x = rand() % (int)(6 + 1.5 * (int)difficulty / (int)Difficulty::Easy) + 1;
    int size_y = rand() % (int)(4 + 1.5 * (int)difficulty / (int)Difficulty::Easy) + 1;
    double accelerationY = 0; // 9.81 * ((rand() / (double(RAND_MAX)) - 0.5) * static_cast<double>(difficulty) / static_cast<double>(Difficulty::Easy));
    double accelerationX = rand() % 2 == 1 ? -rand() % 10 / 10.0 : rand() % 10 / 10.0;
    double velocityY = 4 * ((rand() / (double(RAND_MAX)) - 0.5) * static_cast<double>(difficulty) / static_cast<double>(Difficulty::Easy));
    // AcceleratingObject* obj = new AcceleratingObject(this, Position(posX, 0), size_x, size_y, true, Vector2(accelerationX, accelerationY), Vector2(0, velocityY));
    // entities.push_back(obj);

    instantiate<AcceleratingObject>(Position(posX, 0), size_x, size_y, true, Vector2(accelerationX, accelerationY), Vector2(0, velocityY));
}

AcceleratingObject* GameSpace::test_spawn_falling_obj(Position position)
{
    AcceleratingObject* fallingObject = new AcceleratingObject(position, 3, 3);
    entities.push_back(fallingObject);
    return fallingObject;
}

void GameSpace::set_difficulty(Difficulty difficulty)
{
    this->difficulty = difficulty;
    game_timer.set_time_to_reach(static_cast<long>(difficulty) * MILLION);
}

void GameSpace::print(WINDOW* window)
{
    for (GameObject* entity : entities) {
        if (entity == nullptr) continue;
        Position pos = entity->get_position();
        if (entity->is_player()) {
            std::string player_str = std::to_string(get_player()->get_health());
            if (test_mode) {
                player_str += ", " + get_player()->get_velocity().to_string() + ", ";
                // player_str += (get_player()->is_collidable() ? "collidable" : "not collidable");
                player_str += (get_player()->is_immune() ? "immune" : "not immune");
            }
            mvwaddstr(window, pos.getY() + entity->get_size_y() + 1, pos.getX() + entity->get_size_x() + 1, player_str.c_str());
        }
        // mvwaddstr(window, pos.getY() + entity->get_size_y() + 1, pos.getX() + entity->get_size_x() + 1
        //     , entity->is_player() ? std::to_string(dynamic_cast<Player*>(entity)->get_health()).c_str() : pos.to_string().c_str());
        int x = pos.getX(), y = pos.getY();
        int size_x = entity->get_size_x(), size_y = entity->get_size_y();
        char entity_char = entity->get_char();

        Pattern entity_pattern = entity->get_pattern();
        switch (entity_pattern) {
            case Pattern::Cross:
                for (int i = 0; i < size_x; i++) {              
                    if (is_in_bounds(x + i - 1, y)) {
                        mvwaddch(window, y, x + i, entity_char);
                    }
                    if (is_in_bounds(x - i - 1, y)) {
                        mvwaddch(window, y, x - i, entity_char);
                    }
                }
                for (int i = 0; i < size_y; i++) {
                    if (is_in_bounds(x, y + i - 1)) {
                        mvwaddch(window, y + i, x, entity_char);
                    }
                    if (is_in_bounds(x, y - i - 1)) {
                        mvwaddch(window, y - i, x, entity_char);
                    }
                }
                break;
            case Pattern::Square: {
                int topX = x - size_x / 2 + 1;
                int topY = y - size_y / 2 + 1;
                for (int tempY = topY; tempY < topY + size_y; tempY++) {
                    for (int tempX = topX; tempX < topX + size_x; tempX++) {
                        mvwaddch(window, tempY, tempX, entity_char);
                    }
                }
                break;
            }
            default:
                mvwaddch(window, y, x, entity_char);
                break;
        }
    }

    std::string time_remaining_str;
    if (test_mode) {
        time_remaining_str = "Entities: " + std::to_string(entities.size()) + ". Time remaining: " + std::to_string((game_timer.get_time_remaining()) / MILLION) + "s";

        collision_detector.print(window);

        std::string deleted_entities_str = "Deleted entities: " + std::to_string(num_deleted_entities);
        mvwaddstr(window, MAX_Y - 1, MAX_X - deleted_entities_str.length() - 1, deleted_entities_str.c_str());
    } else {
        // everything not in test mode
        time_remaining_str = "Time remaining: " + std::to_string((game_timer.get_time_remaining()) / MILLION) + "s";
    }
    mvwaddstr(window, 3, MAX_X - time_remaining_str.length() - 1, time_remaining_str.c_str());

    

}

void GameSpace::reset(Difficulty difficulty, bool test_mode)
{
    for (GameObject*& entity : entities) {
        delete entity;
        entity = nullptr;
    }
    for (GameObject*& entity : entities_to_delete) {
        delete entity;
        entity = nullptr;
    }
    entities.clear(); entities_to_delete.clear();

    set_difficulty(difficulty);
    this->test_mode = test_mode;
    player = new Player(test_mode);
    entities.push_back(player);
    collision_detector.update(entities);
    game_timer.reset();
}

GameSpace *GameSpace::get_instance()
{
    static GameSpace gamespace;
    return &gamespace;
}

CollisionCell::CollisionCell(int x, int y): x(x), y(y) {
    // entities.reserve(8);
}

void CollisionCell::setX(int x) {
    this->x = x;
}
void CollisionCell::setY(int y) {
    this->y = y;
}
int CollisionCell::getX() const {
    return x;
}
int CollisionCell::getY() const {
    return y;
}

void CollisionCell::clear_entities()
{
    // for (GameObject* entity : entities) {
    //     entity->clear_colliding_entities();
    // }
    entities.clear();
}

void CollisionCell::add_entity(GameObject *entity)
{

    // Now using set
    if (!entity) {
        return;
    }
    // if (std::find(entities.begin(), entities.end(), entity) != entities.end()) { // if found, don't push_back
    //     return;
    // }
    entities.insert(entity);
    // entities.push_back(entity);
}

void CollisionCell::check_collision()
{
    if (entities.size() == 0) {
        return;
    }

    std::vector<GameObjectFrameInfo> entities_info_in_frame;

    for (GameObject* entity : entities) {
        entity->update_existing_colliding_entities();
        if (!entity->is_collidable()) {
            continue;
        }
        entities_info_in_frame.push_back(GameObjectFrameInfo(entity, entity->get_position(), entity->get_velocity()));
    }

    if (entities.size() == 1) {
        return;
    }

    for (GameObjectFrameInfo entity_info : entities_info_in_frame) {
        GameObject*& entity = entity_info.entity;
        if (entity->is_deletable() || !entity->is_collidable()) {
            continue;
        }
        for (GameObjectFrameInfo other_entity_info : entities_info_in_frame) {
            GameObject*& other_entity = other_entity_info.entity;
            if (entity == other_entity || other_entity->is_deletable() || !entity->is_collidable()) {
                continue;
            }
            if (entity->is_colliding_with(other_entity) || other_entity->is_colliding_with(entity)) {
                continue;
            }
            if (entity->intersects(other_entity)) {
                entity->handle_collision(other_entity_info);
                other_entity->handle_collision(entity_info);
            }
        }
    }

    for (GameObject* entity : entities) {
        entity->update_existing_colliding_entities();
    }
}

int CollisionCell::get_num_of_entities() const
{
    return entities.size();
}

CollisionDetection::CollisionDetection()
{
    for (size_t i = 0; i < COLLISION_GRID_Y; i++) { // i = y = rows
        for (size_t j = 0; j < COLLISION_GRID_X; j++) { // j = x = cols
            cells[i][j].setX(j);
            cells[i][j].setY(i);
        }
    }
}

void CollisionDetection::clear_cells()
{
    for (size_t i = 0; i < COLLISION_GRID_Y; i++) { // i = y = rows
        for (size_t j = 0; j < COLLISION_GRID_X; j++) { // j = x = cols
            cells[i][j].clear_entities();
        }
    }
}

void CollisionDetection::check_cell_collisions()
{
    for (size_t i = 0; i < COLLISION_GRID_Y; i++) { // i = y = rows
        for (size_t j = 0; j < COLLISION_GRID_X; j++) { // j = x = cols
            cells[i][j].check_collision();
        }
    }
}

void CollisionDetection::update(const std::list<GameObject *>& entities)
{
    clear_cells();
    for (GameObject* entity : entities) {
        for (Vector2& point : entity->get_hitbox().get_rect().get_edge_points()) {
            int cell_x = point.getX() / COLLISION_DIVISION;
            int cell_y = point.getY() / COLLISION_DIVISION;
            cells[cell_y][cell_x].add_entity(entity);
        }
    }
    check_cell_collisions();
}

void CollisionDetection::print(WINDOW *window)
{
    for (size_t i = 0; i < COLLISION_GRID_Y; i++) {
        for (size_t j = 0; j < COLLISION_GRID_X; j++) {
            mvwaddstr(window, i*COLLISION_DIVISION, j*COLLISION_DIVISION, std::to_string(cells[i][j].get_num_of_entities()).c_str());
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Difficulty& difficulty) {
    switch (difficulty) {
        case Difficulty::NotSet:
            os << "NotSet";
            break;
        case Difficulty::Easy:
            os << "Easy";
            break;
        case Difficulty::Medium:
            os << "Medium";
            break;
        case Difficulty::Hard:
            os << "Hard";
            break;
    }
    return os;
}