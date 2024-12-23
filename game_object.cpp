#include "game_object.h"
#include "util.h"

int GameObject::get_fixed_size(int size, const Pattern &pattern)
{
    switch (pattern) {
        case Pattern::Square:
            if (size % 2 == 0) {
                size = size + 1;
            }
            break;
        case Pattern::Cross:
            break;
    }
    return size;
}

void GameObject::update_hitbox()
{
    hitbox.update();
}

void GameObject::update_velocity_from_collisions(long frameTime)
{
    if (!is_collidable()) {
        return;
    }
    // Push colliding objects out, depending on proportion of area overlap
    for (GameObjectFrameInfo& gofi : colliding_entities_frame_info) {
        float proportion_intersected = get_hitbox().get_rect().proportion_intersected(gofi.entity->get_hitbox().get_rect());
        velocity += (gofi.entity->get_position() - get_position()).normalise() * proportion_intersected;
    }

}

GameObject::GameObject(Position position, int size_x, int size_y, Pattern pattern, Vector2 velocity) 
    : position(position), pattern(pattern), hitbox(this), velocity(velocity)
{
    this->size_x = get_fixed_size(size_x, pattern);
    this->size_y = get_fixed_size(size_y, pattern);
    this->mass = this->size_x * this->size_y;
    this->hitbox.update();
}

char GameObject::get_char() const
{
    return representing_char;
}

Position GameObject::get_position() const
{
    return position;
}

bool GameObject::is_player() const
{
    return false;
}

Pattern GameObject::get_pattern() const
{
    return pattern;
}

HitBox GameObject::get_hitbox() const
{
    return hitbox;
}

int GameObject::get_size_x() const {
    return size_x;
}

int GameObject::get_size_y() const {
    return size_x;
}

int GameObject::get_mass() const
{
    return mass;
}

bool GameObject::is_deletable() const
{
    return deletable;
}

void GameObject::set_collidable(bool collidable)
{
    this->collidable = collidable;
}

bool GameObject::is_collidable() const
{
    return collidable;
}

void GameObject::set_position(const Position &pos)
{
    position = pos;
}

void GameObject::set_velocity(const Vector2 &v)
{
    velocity = v;
}

Vector2 GameObject::get_velocity()
{
    return velocity;
}


bool GameObject::intersects(GameObject *entity)
{
    if (this == entity) {
        return false;
    }
    return hitbox.intersects(entity->hitbox);
}

void GameObject::handle_collision(const GameObjectFrameInfo &gofi)
{
    // if (!collidable) {
    //     return;
    // }

    // Elastic collision, 2D vector form
    // v1' = v1 - 2m2/(m1 + m2) * ((v1 - v2) . (x1 - x2))/(norm(x1 - x2)^2) * (x1 - x2)
    double mass_factor = 2 * gofi.entity->get_mass() / (get_mass() + gofi.entity->get_mass());
    Vector2 position_difference = get_position() - gofi.position;
    double velocity_factor = (get_velocity() - gofi.velocity).dot(position_difference) / pow(position_difference.get_magnitude(), 2);
    
    velocity -= position_difference * mass_factor * velocity_factor; // * pow(0.8, ++frames_since)

    add_colliding_entity_info(gofi);
}

void GameObject::update_existing_colliding_entities()
{
    if (!is_collidable()) {
        clear_colliding_entities();
        return;
    }
    std::list<GameObjectFrameInfo> remove_colliding_entities_info;
    for (const GameObjectFrameInfo& gofi : colliding_entities_frame_info) {
        if (gofi.entity->is_deletable() || !gofi.entity->is_collidable()) {
            remove_colliding_entities_info.push_front(gofi);
            continue;
        }
        if (intersects(gofi.entity)) {
            continue;
        }
        remove_colliding_entities_info.push_front(gofi);
    }
    for (const GameObjectFrameInfo& gofi : remove_colliding_entities_info) {
        colliding_entities_frame_info.remove(gofi);
    }
}

void GameObject::add_colliding_entity_info(const GameObjectFrameInfo& gofi)
{
    if (gofi.entity == this || is_colliding_with(gofi.entity)) {
        return;
    }
    colliding_entities_frame_info.push_front(gofi);
}

bool GameObject::is_colliding_with(GameObject *colliding_entity)
{
    return std::find_if(colliding_entities_frame_info.begin(), 
        colliding_entities_frame_info.end(), 
        [&colliding_entity](GameObjectFrameInfo& gofi){ return colliding_entity == gofi.entity; }) != colliding_entities_frame_info.end();
    // return std::find(colliding_entities.begin(), colliding_entities.end(), colliding_entity) != colliding_entities.end();
}

void GameObject::clear_colliding_entities()
{
    colliding_entities_frame_info.clear();
}

bool HitBox::intersects(const HitBox& hitbox)
{
    if (this == &hitbox) {
        return false;
    }
    std::array<Vector2, 4> edge_points = rect.get_edge_points();
    std::array<Vector2, 4> other_edge_points = hitbox.rect.get_edge_points();
    // std::cout << this->rect.to_string() << std::endl;
    // std::cout << hitbox.rect.to_string() << std::endl;

    // std::cout << edge_points[0].getX() << ' ' << other_edge_points[1].getX() << std::endl;
    // std::cout << (edge_points[0].getX() <= other_edge_points[1].getX()) << std::endl; // check if currbox left is to the left of otherbox right
    // std::cout << edge_points[1].getX() << ' ' << other_edge_points[0].getX() << std::endl;
    // std::cout << (edge_points[1].getX() >= other_edge_points[0].getX()) << std::endl; // currbox right is to the right of otherbox left
    // std::cout << edge_points[0].getY() << ' ' << other_edge_points[2].getY() << std::endl;
    // std::cout << (edge_points[0].getY() >= other_edge_points[2].getY()) << std::endl; // currbox top is above otherbox bottom
    // std::cout << edge_points[2].getY() << ' ' << other_edge_points[0].getY() << std::endl;
    // std::cout << (edge_points[2].getY() <= other_edge_points[0].getY()) << std::endl;
    return edge_points[0].getX() <= other_edge_points[1].getX() && // check if currbox left is to the left of otherbox right
        edge_points[1].getX() >= other_edge_points[0].getX() && // currbox right is to the right of otherbox left
        edge_points[0].getY() >= other_edge_points[2].getY() && // currbox top is above otherbox bottom
        edge_points[2].getY() <= other_edge_points[0].getY(); // currbox bottom is below otherbox top
}

GameObject *HitBox::get_game_object() const
{
    return game_object;
}

Rect HitBox::get_rect() const
{
    return rect;
}

void HitBox::update()
{
    Position pos = game_object->get_position();
    double size_x = game_object->get_size_x();
    double size_y = game_object->get_size_y();
    Vector2 topL = pos + Vector2(-size_x/2, size_y/2);
    Vector2 topR = pos + Vector2(size_x/2, size_y/2);
    Vector2 bottomL = pos + Vector2(-size_x/2, -size_y/2);
    Vector2 bottomR = pos + Vector2(size_x/2,-size_y/2);
    rect.set(topL, topR, bottomL, bottomR);
}
