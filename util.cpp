#include "util.h"

Vector2::Vector2(double x, double y): x(x), y(y) {}

double Vector2::getX() const {
    return x;
}

double Vector2::getY() const {
    return y;
}

int Vector2::get_rounded_x() const
{
    if (x - int(x) >= 0.5) {
        return int(x) + 1;
    }
    return int(x);
}

int Vector2::get_rounded_y() const
{
    if (y - int(y) >= 0.5) {
        return int(y);
    }
    return int(y) + 1;
}

double Vector2::get_magnitude() const
{
    return sqrt(x * x + y * y);
}

void Vector2::setX(double x) {
    this->x = x;
}
void Vector2::setY(double y) {
    this->y = y;
}
Vector2 Vector2::operator+(const Vector2& v) {
    Vector2 temp = *this;
    return temp += v;
}
double Vector2::dot(const Vector2 &v)
{
    return getX() * v.getX() + getY() * v.getY();
}
Vector2 Vector2::normalise()
{
    Vector2 temp(*this);
    double magnitude = get_magnitude();
    temp.setX(x / magnitude);
    temp.setY(y / magnitude);
    return temp;
}
Vector2 Vector2::operator-(const Vector2 &v)
{
    Vector2 temp = *this;
    return temp -= v;
}
Vector2 Vector2::operator-()
{
    return Vector2(-x, -y);
}
Vector2 Vector2::operator*(const double &d)
{
    Vector2 temp = *this;
    return temp *= d;
}
Vector2 Vector2::operator/(const double &d)
{
    Vector2 temp = *this;
    return temp /= d;
}
Vector2 &Vector2::operator+=(const Vector2 &v)
{
    x += v.x;
    y += v.y;
    return *this;
}
Vector2& Vector2::operator-=(const Vector2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2 &Vector2::operator*=(const double &d)
{
    x *= d;
    y *= d;
    return *this;
}

Vector2 &Vector2::operator/=(const double &d)
{
    x /= d;
    y /= d;
    return *this;
}

std::string Vector2::to_string() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

Position::Position(double x, double y): Vector2(x, y) {
    add_vector2_to_position_bound(Vector2(0,0)); // make sure within bounds
}

Vector2 &Position::add_vector2_to_position_bound(const Vector2 &v)
{
    *this += v;
    if (getX() >= MAX_X) {
        setX(MAX_X);
    } else if (getX() <= 0) {
        setX(0);
    }
    if (getY() >= MAX_Y) {
        setY(MAX_Y);
    } else if (getY() <= 0) {
        setY(0);
    }
    return *this;
}

bool operator==(const Vector2 &v1, const Vector2 &v2)
{
    return abs(v1.getX() - v2.getX()) <= std::numeric_limits<double>::epsilon() && abs(v1.getY() - v2.getY()) <= std::numeric_limits<double>::epsilon();
}

bool operator!=(const Vector2 &v1, const Vector2 &v2)
{
    return !(v1 == v2);
}

std::ostream &operator<<(std::ostream &os, const Vector2 &v)
{
    return os << '(' << v.getX() << ", " << v.getY() << ')';
}

Rect::Rect(Vector2 topL, Vector2 topR, Vector2 bottomL, Vector2 bottomR)
{
    set(topL, topR, bottomL, bottomR);
}

void Rect::set(const Vector2& topL, const Vector2& topR, const Vector2& bottomL, const Vector2& bottomR)
{
    // keep it axis aligned (for now)
    if ((topL.getY() != topR.getY()) || (topL.getX() != bottomL.getX()) || (bottomL.getY() != bottomR.getY()) || (topR.getX() != bottomR.getX())) {
        return;
    }
    edge_points[0] = topL;
    edge_points[1] = topR;
    edge_points[2] = bottomL;
    edge_points[3] = bottomR;
    area = (edge_points[1].getX() - edge_points[0].getX()) * (edge_points[1].getY() - edge_points[2].getY());
}

float Rect::proportion_intersected(const Rect &rect) const
{
    // max of left for this and other rect - min of right for this and other rect
    float x_overlap = std::max(edge_points[0].getX(), rect.edge_points[0].getX()) - std::min(edge_points[1].getX(), rect.edge_points[1].getX());

    // max of top for this and other rect - min of bottom for this and other rect
    float y_overlap = std::max(edge_points[0].getY(), rect.edge_points[0].getY()) - std::min(edge_points[2].getY(), rect.edge_points[2].getY());
    float area_overlap = x_overlap * y_overlap;
    
    return area_overlap / area;
}

std::array<Vector2, 4> Rect::get_edge_points() const
{
    return edge_points;
}

std::string Rect::to_string() const
{
    std::string s = "[";
    for (Vector2 point : edge_points) {
        s += point.to_string();
        s += ",";
    }
    s.pop_back();
    s += "]";
    return s;
}

bool is_in_bounds(int x, int y)
{
    return (x >= 0 && x <= MAX_X) && (y >= 0 && y <= MAX_Y);
}

bool is_in_bounds(const Vector2& pos) {
    return is_in_bounds(pos.getX(), pos.getY());
}

bool is_in_bounds(Rect rect)
{
    std::array<Vector2, 4> edge_points = rect.get_edge_points();
    return is_in_bounds(edge_points[0]) || is_in_bounds(edge_points[1]) ||
        is_in_bounds(edge_points[2]) || is_in_bounds(edge_points[3]);
}

bool is_touching_space_bottom_side(const Rect &rect)
{
    static Rect bottom_rect({0, MAX_Y}, {MAX_X, MAX_Y}, {0, MAX_Y + 5}, {MAX_X, MAX_Y + 5});
    return bottom_rect.proportion_intersected(rect) == 0;
}

bool is_touching_space_left_side(const Rect &rect)
{
    static Rect left_rect({-5, 0}, {0, 0}, {-5, MAX_Y}, {0, MAX_Y});
    return left_rect.proportion_intersected(rect) == 0;
}

bool is_touching_space_right_side(const Rect &rect)
{
    static Rect right_rect({MAX_X, 0}, {MAX_X + 5, 0}, {MAX_X, MAX_Y}, {MAX_X + 5, MAX_Y});
    return right_rect.proportion_intersected(rect) == 0;
}

bool is_touching_space_top_side(const Rect &rect)
{
    static Rect top_rect({0, -5}, {MAX_X, -5}, {0, 0}, {MAX_X, 0});
    return top_rect.proportion_intersected(rect) == 0;
}

Direction get_final_direction(const std::vector<Direction>& directions) {
    Direction finalDirection = Direction::Unassigned;
    for (const Direction& dir : directions) {
        if (static_cast<int>(finalDirection) == -static_cast<int>(dir)) { // if opposite direction
            finalDirection = Direction::Unassigned;
            continue;
        }
        switch (finalDirection) {
            case Direction::Unassigned:
                finalDirection = dir;
                break;
            case Direction::Up: case Direction::Down:
                finalDirection = static_cast<Direction>(static_cast<int>(finalDirection) - static_cast<int>(dir));
                break;
            case Direction::Left: case Direction::Right:
                finalDirection = static_cast<Direction>(-(static_cast<int>(finalDirection) - static_cast<int>(dir)));
                break;
            case Direction::UpLeft:
                if (dir == Direction::Right) {
                    finalDirection = Direction::Up;
                } else if (dir == Direction::Down) {
                    finalDirection = Direction::Left;
                }
                break;
            case Direction::DownLeft:
                if (dir == Direction::Right) {
                    finalDirection = Direction::Up;
                } else if (dir == Direction::Up) {
                    finalDirection = Direction::Left;
                }
                break;
            case Direction::UpRight:
                if (dir == Direction::Left) {
                    finalDirection = Direction::Up;
                } else if (dir == Direction::Down) {
                    finalDirection = Direction::Right;
                }
                break;
            case Direction::DownRight:
                if (dir == Direction::Left) {
                    finalDirection = Direction::Down;
                } else if (dir == Direction::Up) {
                    finalDirection = Direction::Right;
                }
                break;
        }
    }
    return finalDirection;
}

std::ostream &operator<<(std::ostream &os, const Rect &r)
{
    return os << r.to_string();
}

bool operator==(const GameObjectFrameInfo &gofi1, const GameObjectFrameInfo &gofi2)
{
    return gofi1.entity == gofi2.entity && gofi1.velocity == gofi2.velocity;
}

GameObjectFrameInfo::GameObjectFrameInfo(GameObject *entity, Position position, Vector2 velocity) : entity(entity), position(position), velocity(velocity) //, frames_since(1)
{
}
