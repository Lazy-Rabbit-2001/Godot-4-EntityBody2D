#pragma once

#include <godot_cpp/classes/character_body2d.hpp>

using namespace godot;

class EntityBody2D : public CharacterBody2D {
    GDCLASS(EntityBody2D, CharacterBody2D)

private:
    // Exported properties
    Vector2 velocity;
    bool movement_local;

    double gravity;
    double max_falling_speed;

    Vector2 top_direction;

    // Hiden properties
    Vector2 _velocity;
    Vector2 _velocity_global;
    Vector2 _falling_velocity;

    // Method use
    bool _motion_changed;

protected:
    static void _bind_methods();

    double _get_delta();

public:
    // Constructor and Destructor
    EntityBody2D();
    ~EntityBody2D();

    // Built-in
    void _enter_tree() override;

    // Methods
    bool move_and_slide(const bool use_real_velocity = false);
    void analyse_global_velocity();
    void accelerate_x(const double acceleration, const double to);
    void accelerate_y(const double acceleration, const double to);
    void accelerate(const double acceleration, const Vector2 &to);
    void use_friction(const double miu);
    void jump(const double jumping_speed, const bool is_accumulating_mode = false);
    void correct_on_wall_corner(const int steps = 4);
    void correct_onto_floor(const int steps = 20);

    // Properties Setters and Getters
    Vector2 get_gravity_direction() const;
    Vector2 get_previous_velocity() const;
    Vector2 get_previous_global_velocity() const;

    bool is_leaving_ground() const;
    bool is_falling() const;

    void set_velocity(const Vector2 &p_velocity);
    Vector2 get_velocity() const;

    void set_global_velocity(const Vector2 &p_global_velocity);
    Vector2 get_global_velocity() const;

    void set_movement_local(const bool p_movement_local);
    bool is_movement_local() const;

    void set_gravity(const double p_gravity);
    double get_gravity() const;

    void set_max_falling_speed(const double p_max_falling_speed);
    double get_max_falling_speed() const;

    void set_top_direction(const Vector2 &p_top_direction);
    Vector2 get_top_direction() const;
};
