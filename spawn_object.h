#pragma once
#include "util.h"
#include "game_object.h"
#include "player.h"

class GameSpace;
enum class SpawnObjectType {

};

class SpawnObject : public GameObject {
    protected:
        SpawnObjectType type;
    public:
        SpawnObject(Position position, int size_x, int size_y, Pattern pattern, 
            Vector2 velocity = Vector2(0,0));
};

class AcceleratingObject : public SpawnObject {
    private:
        bool affected_by_gravity;
        Vector2 acceleration;
    public:
        AcceleratingObject(Position position = Position(50,0), int size_x = 3, int size_y = 3, bool affected_by_gravity = false, Vector2 acceleration = Vector2(0, 0), Vector2 velocity = Vector2(0,0));
        virtual bool is_enemy() const override;
        virtual void update(long frameTime) override;
        virtual void handle_collision(const GameObjectFrameInfo& gofi) override;
};