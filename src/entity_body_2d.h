#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>

using namespace godot;

class EntityBody2D : public CharacterBody2D {
    GDCLASS(EntityBody2D, CharacterBody2D)

private:
    const double GRAVITY_DEFAULT = 980.0;

    // Exported properties
    Vector2 velocity;
    double max_velocity_x;
    double max_velocity_x_scale;
    double damp_enabled;
    double damp_min_speed;
    bool autobody;

    double gravity;
    double gravity_x;
    double max_falling_speed;
    double max_falling_speed_x;
    bool global_rotation_to_gravity_direction;

    double up_direction_angle;

    // Hiden properties
    Vector2 _velocity;
    Vector2 _velocity_global;
    double _max_velocity_x;

    double _damped_min_speed;
    double _damped_min_velocity_global_axis_x;

protected:
    static void _bind_methods();

public:
    // Constructor and Destructor
    EntityBody2D();
    ~EntityBody2D();

    // Built-in
    void _enter_tree() override;

    // Methods
    bool move_and_slide(const bool use_real_velocity = false);
    void accelerate_autobody_velocity_x(const double acceleration, const int direction_override = 0);
    void accelerate_local_x(const double acceleration, const double to);
    void accelerate_local_y(const double acceleration, const double to);
    void accelerate_local(const double acceleration, const Vector2 &to);
    void accelerate_x(const double acceleration, const double to);
    void accelerate_y(const double acceleration, const double to);
    void accelerate(const double acceleration, const Vector2 &to);
    void turn_wall();
    void turn_ceiling_ground();
    void use_friction(const double miu);
    void jump(const double jumping_speed, const bool is_accumulating_mode = false);
    void correct_on_wall_corner(const int steps = 4);
    void correct_onto_floor(const int steps = 20);

    // Properties Setters and Getters
    Vector2 get_previous_velocity() const;
    Vector2 get_previous_global_velocity() const;
    double get_previous_speed() const;
    Vector2 get_gravity_vector() const;
    double get_gravity_rotation_angle() const;
    double get_damp() const;
    TypedArray<Node2D> get_colliders();
    Node2D* get_last_collider();

    bool is_leaving_ground() const;
    bool is_falling() const;

    void set_velocity(const Vector2 &p_velocity);
    Vector2 get_velocity() const;

    void set_max_velocity_x(const double p_max_velocity_x);
    double get_max_velocity_x() const;

    void set_max_velocity_x_scale(const double p_max_velocity_x_scale);
    double get_max_velocity_x_scale() const;

    void set_global_velocity(const Vector2 &p_global_velocity);
    Vector2 get_global_velocity() const;

    void set_damp_enabled(const bool p_damp_enabled);
    bool is_damp_enabled() const;

    void set_damp_min_speed(const double p_damp_min_speed);
    double get_damp_min_speed() const;

    void set_autobody(const bool p_autobody);
    bool is_autobody() const;

    void set_gravity(const double p_gravity);
    double get_gravity() const;

    void set_gravity_x(const double p_gravity_x);
    double get_gravity_x() const;

    void set_max_falling_speed(const double p_max_falling_speed);
    double get_max_falling_speed() const;

    void set_max_falling_speed_x(const double p_max_falling_speed_x);
    double get_max_falling_speed_x() const;

    void set_global_rotation_to_gravity_direction(const bool p_global_rotation_to_gravity_direction);
    bool is_global_rotation_to_gravity_direction() const;

    void set_up_direction_angle(const double p_up_direction_angle);
    double get_up_direction_angle() const;
};
