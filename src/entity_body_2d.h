#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>

using namespace godot;

class EntityBody2D : public CharacterBody2D {
    GDCLASS(EntityBody2D, CharacterBody2D)

private:
    const double GRAVITY_DEFAULT = 980.0;
    
    // Exported properties
    Vector2 velocity;
    double damp_mode;
    double damp_min_speed_ratio;
    bool autobody;
    double speed;

    double gravity;
    double max_falling_speed;
    Dictionary falling_speed_factors;
    bool global_rotation_to_gravity_direction;

    Vector2 top_direction;

    // Hiden properties
    Vector2 _velocity;
    Vector2 _velocity_global;
    double _speed;

    double _init_speed = 0.0;
    double _init_velocity_global_axis_x = 0.0;
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
    void accelerate_speed(const double acceleration, const double to);
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

    void set_global_velocity(const Vector2 &p_global_velocity);
    Vector2 get_global_velocity() const;

    void set_damp_mode(const bool p_damp_mode);
    bool is_damp_mode() const;

    void set_damp_min_speed_ratio(const double p_damp_min_speed_ratio);
    double get_damp_min_speed_ratio() const;

    void set_autobody(const bool p_autobody);
    bool is_autobody() const;

    void set_speed(const double p_speed);
    double get_speed() const;

    void set_gravity(const double p_gravity);
    double get_gravity() const;

    void set_max_falling_speed(const double p_max_falling_speed);
    double get_max_falling_speed() const;

    void set_global_rotation_to_gravity_direction(const bool p_global_rotation_to_gravity_direction);
    bool is_global_rotation_to_gravity_direction() const;

    void set_top_direction(const Vector2 &p_top_direction);
    Vector2 get_top_direction() const;
};
