#pragma once
#include "util.h"
#include "timer.h"
#include <iostream>
#include "game_object.h"

const double PLAYER_MOVE_SPEED_INCREASE_X = 25;
const double PLAYER_MOVE_SPEED_INCREASE_Y = 18;
const double PLAYER_MAX_SPEED = 70;

class Player : public GameObject {
    private:
        int health;
        Vector2 move_velocity;
        bool player_movement_disabled = false;
        bool immune = false;
        ActionTimer hit_immunity_timer;
    public:
        Player(bool test_mode);
        void move(const std::vector<Direction> &directions);
        void move(Direction direction);
        void attack(Direction direction);
        void heal(int value);
        void take_damage(int damage, GameObject* attacking_entity);
        void set_player_movement_disabled(bool disabled);
        bool get_player_movement_disabled() const;
        void set_immune(bool immune);
        bool is_immune() const;
        int get_health() const;
        virtual bool is_player() const override;
        virtual bool is_enemy() const override;
        virtual Vector2 get_velocity() override;
        virtual void update(long frameTime) override;
        virtual void handle_collision(const GameObjectFrameInfo& gofi) override;
};