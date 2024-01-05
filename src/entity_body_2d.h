#ifndef GD_ENTITY_BODY2D
#define GD_ENTITY_BODY2D

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>

using namespace godot;

class EntityBody2D : public CharacterBody2D {
    GDCLASS(EntityBody2D, CharacterBody2D)

private:
    // Properties
    Vector2 velocity; // Local velocity, or `velocality` in GDScript
    bool threshold_speed_enabled;
    double threshold_speed;
    double threshold_speed_correction_acceleration;
    double damp_enabled;
    double damp_min_speed;
    double gravity_scale;
    double max_falling_speed;
    bool global_rotation_to_gravity_direction;
    double up_direction_angle;

    Vector2 _velocity;
    Vector2 _velocity_global;
    bool _threshold_speed_initialized_for_velocity_x;
    bool _threshold_speed_affected; // Wherever velocity.x gets changed, this should be reassigned to it except in set_velocity() and set_global_velocity();
    double _damped_min_speed;
    double _damped_min_velocity_global_axis_x;

    // Methods
    Vector2 _get_gravity() const;

protected:
    static void _bind_methods();

public:
    EntityBody2D();
    ~EntityBody2D();

    bool move_and_slide(const bool use_real_velocity = false);
    void calculate_gravity();
    void calculate_damp();
    void accelerate_local_x(const double acceleration, const double to); // For velocality
    void accelerate_local_y(const double acceleration, const double to); // For velocality
    void accelerate_local(const double acceleration, const Vector2 &to); // For velocality
    void accelerate_x(const double acceleration, const double to); // For velocity
    void accelerate_y(const double acceleration, const double to); // For velocity
    void accelerate(const double acceleration, const Vector2 &to); // For velocity
    void decelerate_with_friction(const double deceleration);
    void use_friction(const double miu);
    void turn_wall();
    void turn_ceiling_ground();
    void jump(const double jumping_speed, const bool is_accumulating_mode = false);
    void correct_on_wall_corner(const int steps = 4);
    void correct_onto_floor(const int steps = 10);
    Vector2 get_previous_velocity() const;
    Vector2 get_previous_global_velocity() const;
    Vector2 get_gravity_vector() const;
    double get_gravity_rotation_angle() const;
    double get_damp() const;
    TypedArray<Node2D> get_colliders();
    Node2D* get_last_collider();
    bool is_leaving_ground() const;
    bool is_falling() const;

    void set_velocity(const Vector2 &p_velocity); // = `set_velocality()` in GDScript
    Vector2 get_velocity() const; // = `get_velocality()` in GDScript
    void set_global_velocity(const Vector2 &p_global_velocity);
    Vector2 get_global_velocity() const;
    void set_threshold_speed(const double p_threshold_speed);
    double get_threshold_speed() const;
    void set_threshold_speed_enabled(const bool p_threshold_speed_enabled);
    bool is_threshold_speed_enabled() const;
    void set_threshold_speed_correction_acceleration(const double p_threshold_speed_correction_acceleration);
    double get_threshold_speed_correction_acceleration() const;
    void set_damp_enabled(const bool p_damp_enabled);
    bool is_damp_enabled() const;
    void set_damp_min_speed(const double p_damp_min_speed);
    double get_damp_min_speed() const;
    void set_gravity_scale(const double p_gravity_scale);
    double get_gravity_scale() const;
    void set_max_falling_speed(const double p_max_falling_speed);
    double get_max_falling_speed() const;
    void set_global_rotation_to_gravity_direction(const bool p_global_rotation_to_gravity_direction);
    bool is_global_rotation_to_gravity_direction() const;
    void set_up_direction_angle(const double p_up_direction_angle);
    double get_up_direction_angle() const;
};

#endif