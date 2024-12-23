#include "player.h"

Player::Player(bool test_mode) : GameObject(Position(50, 35), 2, 2, Pattern::Cross), health(4), hit_immunity_timer(1500000) {
    mass = 4;
    if (test_mode) {
        health = 9999;
    }

    representing_char = '*';

    hit_immunity_timer.add_action("enable_player_movement", [this](){ this->set_player_movement_disabled(false); }, 0.15);
    for (int i = 0; i <= 5; i++) {
        hit_immunity_timer.add_action("player_char_" + std::to_string(i), [this, i]() { representing_char = i % 2 == 0 ? '!' : '*'; }, i / 5.0);
    }
}

void Player::move(const std::vector<Direction>& directions) {
    Direction finalDir = get_final_direction(directions);
    move(finalDir);
}

void Player::move(Direction direction) {
    if (get_player_movement_disabled()) {
        return;
    }

    Vector2 future_move_velocity = move_velocity;
    switch (direction) {
        case Direction::Down: // Position +y => going down!
            future_move_velocity += Vector2(0, PLAYER_MOVE_SPEED_INCREASE_Y);
            break;
        case Direction::Up:
            future_move_velocity += Vector2(0, -PLAYER_MOVE_SPEED_INCREASE_Y);
            break;
        case Direction::Left:
            future_move_velocity += Vector2(-PLAYER_MOVE_SPEED_INCREASE_X, 0);
            break;
        case Direction::Right:
            future_move_velocity += Vector2(PLAYER_MOVE_SPEED_INCREASE_X, 0);
            break;
        case Direction::DownLeft:
            future_move_velocity += Vector2(-ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_X, ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_Y);
            break;
        case Direction::UpLeft:
            future_move_velocity += Vector2(-ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_X, -ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_Y);
            break;
        case Direction::DownRight:
            future_move_velocity += Vector2(ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_X, ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_Y);
            break;
        case Direction::UpRight:
            future_move_velocity += Vector2(ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_X, -ONE_OVER_SQRT2 * PLAYER_MOVE_SPEED_INCREASE_Y);
            break;
        default:
            break;
    }
    if (future_move_velocity.get_magnitude() > PLAYER_MAX_SPEED) {
        move_velocity = future_move_velocity.normalise() * PLAYER_MAX_SPEED;
        return;
    }
    move_velocity = future_move_velocity;
}

void Player::heal(int value) {
    if (value < 0) {
        return;
    }
    health += value;
}

void Player::take_damage(int damage, GameObject* attacking_entity)
{
    if (!attacking_entity->is_enemy()) {
        return;
    }
    if (damage < 0) {
        return;
    }
    if (is_immune()) {
        return;
    }
    
    // Turn on temporary immunity (immunity timer logic in Player() and update(long frameTime))
    move_velocity = 0;
    set_immune(true);
    set_player_movement_disabled(true);
    set_collidable(false);

    health -= damage;
    if (health <= 0) {
        // GAMEOVER!
        deletable = true;
    }
}

void Player::set_player_movement_disabled(bool disabled)
{
    player_movement_disabled = disabled;
}

bool Player::get_player_movement_disabled() const
{
    return player_movement_disabled;
}

void Player::set_immune(bool immune)
{
    this->immune = immune;
}

bool Player::is_immune() const
{
    return immune;
}

int Player::get_health() const {
    return health;
}

bool Player::is_player() const
{
    return true;
}

bool Player::is_enemy() const
{
    return false;
}

Vector2 Player::get_velocity()
{
    return velocity + move_velocity;
}

void Player::update(long frameTime)
{
    float time = frameTime / MILLION;
    if (velocity.get_magnitude() > 0.0001) {
        Rect rect = get_hitbox().get_rect();

        // if touching borders stop moving in that direction
        if (is_touching_space_bottom_side(rect) || is_touching_space_top_side(rect)) {
            velocity.setY(0);
        }
        if (is_touching_space_left_side(rect) || is_touching_space_right_side(rect)) {
            velocity.setX(0);
        }
        // slow velocity over time
        velocity *= (1 - 1.75 * time);
    }
    if (move_velocity.get_magnitude() > 0.0001) {
        // slow player controlled velocity over time
        move_velocity *= (1 - 4 * time);
    }
    position.add_vector2_to_position_bound((velocity + move_velocity) * time);
    update_hitbox();

    if (is_immune()) {
        set_collidable(false);
        hit_immunity_timer.update(frameTime);
        
        if (!hit_immunity_timer.is_over()) {
            return;
        }

        hit_immunity_timer.reset();
        set_player_movement_disabled(false); // just in case
        set_immune(false);
        set_collidable(true);
    }
}

void Player::handle_collision(const GameObjectFrameInfo& gofi)
{
    GameObject::handle_collision(gofi);
}


