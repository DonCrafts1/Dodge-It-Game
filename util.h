#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <set>
#include <algorithm>
#include <limits>
#include "math.h"

constexpr double MAX_X = 100.0;
constexpr double MAX_Y = 50.0;
constexpr double MILLION = 1000000.0;
constexpr double SQRT2 = 1.4142135;
constexpr double ONE_OVER_SQRT2 = 1/SQRT2;

class GameObject;
class Vector2 {
    private:
        double x;
        double y;
    
    public:
        Vector2(double x = 0, double y = 0);
        virtual double getX() const;
        virtual double getY() const;
        int get_rounded_x() const;
        int get_rounded_y() const;
        double get_magnitude() const;
        void setX(double x);
        void setY(double y);

        double dot(const Vector2& v);
        Vector2 normalise();

        Vector2 operator+(const Vector2& v);
        Vector2 operator-();
        Vector2 operator-(const Vector2& v);
        Vector2 operator*(const double& d);
        Vector2 operator/(const double& d);
        Vector2& operator+=(const Vector2& v);
        Vector2& operator-=(const Vector2& v);
        Vector2& operator*=(const double& d);
        Vector2& operator/=(const double& d);

        std::string to_string() const;
};

bool operator==(const Vector2& v1, const Vector2& v2);
bool operator!=(const Vector2& v1, const Vector2& v2);


const Vector2 GRAVITY(0, 9.81);

class Position : public Vector2 {
    public:
        Position(double x = 0, double y = 0);
        Vector2& add_vector2_to_position_bound(const Vector2& v);
};

std::ostream& operator<<(std::ostream& os, const Vector2& v);

class Rect {
    std::array<Vector2, 4> edge_points;
    float area;
    public:
        Rect(Vector2 topL = Vector2(0,0), Vector2 topR = Vector2(0,0), Vector2 bottomL = Vector2(0,0), Vector2 bottomR = Vector2(0,0));
        void set(const Vector2& topL, const Vector2& topR, const Vector2& bottomL, const Vector2& bottomR);
        float proportion_intersected(const Rect& rect) const;
        std::array<Vector2, 4> get_edge_points() const;
        std::string to_string() const;
};

std::ostream& operator<<(std::ostream& os, const Rect& r);

bool is_in_bounds(int x, int y);

bool is_in_bounds(const Vector2& pos);

bool is_in_bounds(Rect rect);

// Referring to sides of GameSpace
bool is_touching_space_bottom_side(const Rect& rect);
bool is_touching_space_left_side(const Rect& rect);
bool is_touching_space_right_side(const Rect& rect);
bool is_touching_space_top_side(const Rect& rect);

enum class Direction {
    Up = -3,
    Down = 3,
    Left = -1,
    Right = 1,
    UpLeft = -2,
    UpRight = -4,
    DownLeft = 4,
    DownRight = 2,

    Unassigned = -999
};

Direction get_final_direction(const std::vector<Direction>& directions);

enum class Pattern {
    Cross,
    Square,
};

// Information on an entity at a given time 
struct GameObjectFrameInfo {
    GameObjectFrameInfo(GameObject* entity, Position position, Vector2 velocity);

    GameObject* entity;
    Position position;
    Vector2 velocity;

    // int frames_since = 0;
};

bool operator==(const GameObjectFrameInfo& gofi1, const GameObjectFrameInfo& gofi2);
