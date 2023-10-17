#pragma once

#include <godot_cpp/classes/character_body2d.hpp>

using namespace godot;

class EntityBody2D : public CharacterBody2D {
    GDCLASS(EntityBody2D, CharacterBody2D)

private:
    // Exported properties
    Vector2 motion;
    Vector2 gravity_direction;
    double gravity;
    double max_falling_speed;

    // Hiden properties
    Vector2 _velocity;
    Vector2 _falling_velocity;
    Vector2 _real_up_direction;

    // Method use
    bool _motion_changed;

protected:
    static void _bind_methods();

public:
    // Constructor and Destructor
    EntityBody2D();
    ~EntityBody2D();

    // Built-in
    void _enter_tree() override;

    // Methods
    bool move_and_slide(const bool is_gravity_direction_rotated = true, const bool use_real_velocity = false);
    void accelerate_x(const double acceleration, const double to);
    void accelerate_y(const double acceleration, const double to);
    void accelerate(const double acceleration, const Vector2 &to);
    void use_friction(const double miu);
    void jump(const double jumping_speed, const bool is_accumulating_mode = false);
    void correct_on_wall_corner(const int steps = 4);
    void correct_onto_floor(const int steps = 20);

    void set_body_motion(const Vector2 &motion);
    Vector2 get_body_motion() const;


    // Properties Setters and Getters
    Vector2 get_real_up_direction() const;
    Vector2 get_falling_velocity() const;
    Vector2 get_falling_direction() const;
    Vector2 get_previous_velocity() const;

    bool is_leaving_ground() const;
    bool is_falling() const;

    void set_motion(const Vector2 &p_motion);
    Vector2 get_motion();

    void set_inspector_velocity(const Vector2 &p_inspector_velocity);
    Vector2 get_inspector_velocity() const;

    void set_gravity_direction(const Vector2 &p_gravity_direction);
    Vector2 get_gravity_direction() const;

    void set_gravity(const double p_gravity);
    double get_gravity() const;

    void set_max_falling_speed(const double p_max_falling_speed);
    double get_max_falling_speed() const;
};
