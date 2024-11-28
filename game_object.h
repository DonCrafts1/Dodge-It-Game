#pragma once
#include "util.h"

class GameSpace;
class GameObject;

class HitBox {
    private:
        GameObject* game_object;
        Rect rect;
    public:
        HitBox(GameObject* game_object) : game_object(game_object) {}
        GameObject* get_game_object() const;
        Rect get_rect() const;
        void update();
        bool intersects(const HitBox& hitbox);
};

class GameObject {
    protected:
        int size_x, size_y, mass;
        bool deletable = false;
        char representing_char;
        GameSpace* game_space;
        Position position;
        Pattern pattern;
        HitBox hitbox;
        Vector2 velocity;
        bool collidable = true;
        std::list<GameObjectFrameInfo> colliding_entities_frame_info;

        int get_fixed_size(int size, const Pattern& pattern);
        void update_hitbox();
        void update_velocity_from_collisions(long frameTime);
    public:
        GameObject(GameSpace* game_space, Position position = Position(0,0), int size_x = 1, int size_y = 1, Pattern pattern = Pattern::Cross, Vector2 velocity = Vector2(0, 0));
        virtual ~GameObject() = default;

        // Returns char that represents the GameObject type.
        char get_char() const;

        // Overriden by Player class to return true. By default, returns false.
        virtual bool is_player() const;

        // Overriden by objects to show if it is an enemy.
        virtual bool is_enemy() const = 0;

        // Returns the position of the GameObject.
        Position get_position() const;

        // Returns the velocity of the GameObject.
        virtual Vector2 get_velocity();

        // Sets the position of the GameObject.
        void set_position(const Position& pos); 

        // Sets the velocity of the GameObject.
        void set_velocity(const Vector2& v);

        // Returns the pattern ("image") of the GameObject (to be rendered on screen).
        Pattern get_pattern() const;

        // Returns the hitbox of the GameObject.
        HitBox get_hitbox() const;

        // Returns defined size_x (length).
        int get_size_x() const;

        // Returns defined size_y (height).
        int get_size_y() const;

        // Returns the mass of the GameObject.
        int get_mass() const;

        // Returns the bool 'deletable'. If true, will be removed from 'entities' List in GameSpace.
        bool is_deletable() const;

        // Sets the bool 'collidable'. Self explanatory. CollisionDetector & CollisionCells ignore not collidable GameObjects.
        void set_collidable(bool collidable);

        // Returns the bool 'collidable'.
        bool is_collidable() const;

        // Returns true if this hitbox intersects with another GameObject's hitbox.
        bool intersects(GameObject* entity);

        // Handle collision with another GameObject. Accepts GOFI, specifying collided with entity and relevant info (e.g. velocity).
        virtual void handle_collision(const GameObjectFrameInfo& gofi);

        // Update currently colliding entities with this GameObject. Removes no longer colliding entities.
        void update_existing_colliding_entities();

        // Pushes back GameObjectFrameInfo to colliding_entities_frame_info vector.
        void add_colliding_entity_info(const GameObjectFrameInfo& gofi);

        // Checks if colliding_entity is colliding with this GameObject.
        bool is_colliding_with(GameObject* colliding_entity);

        // Clears the colliding_entities vector.
        void clear_colliding_entities();

        // Abstract update function to be overriden by derived classes. Called by GameSpace update(), in turn called by game_loop.cpp
        virtual void update(long frameTime) = 0;
};


