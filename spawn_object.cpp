#include "spawn_object.h"

// SpawnObject::SpawnObject() {}

SpawnObject::SpawnObject(Position position, int size_x, int size_y, Pattern pattern, Vector2 velocity)
 : GameObject(position, size_x, size_y, pattern, velocity) {
    
}

AcceleratingObject::AcceleratingObject(Position position, int size_x, int size_y, bool affected_by_gravity, Vector2 acceleration, Vector2 velocity) 
    : SpawnObject(position, size_x, size_y, Pattern::Square, velocity), affected_by_gravity(affected_by_gravity), acceleration(acceleration)
{
    representing_char = 'v';
}

bool AcceleratingObject::is_enemy() const
{
    return true;
}

void AcceleratingObject::update(long frameTime) {
    double time = frameTime / MILLION;
    velocity += (affected_by_gravity ? acceleration + GRAVITY : acceleration) * time;
    
    update_velocity_from_collisions(frameTime);

    position += velocity * time;
    update_hitbox();
    
    if (is_in_bounds(hitbox.get_rect())) {
        return;
    }
    deletable = true;
}

void AcceleratingObject::handle_collision(const GameObjectFrameInfo& gofi)
{
    if (gofi.entity->is_player()) {
        Player* player = dynamic_cast<Player*>(gofi.entity);
        player->take_damage(1, this);
        deletable = true;
        return;
    }
    
    GameObject::handle_collision(gofi);
}