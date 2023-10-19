#include "entity_body_2d.h"

#include "mathorm.h"
#include "fast_syntax.hpp"

#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace gde_fast_syntax;

void EntityBody2D::_bind_methods() {
    // Register signals
    ADD_SIGNAL(MethodInfo("collided_wall", PropertyInfo(Variant::OBJECT, "collision_info")));
    ADD_SIGNAL(MethodInfo("collided_ceiling", PropertyInfo(Variant::OBJECT, "collision_info")));
    ADD_SIGNAL(MethodInfo("collided_floor", PropertyInfo(Variant::OBJECT, "collision_info")));

    // Register properties
    // double speed
    ADD_GROUP("Speed", "");
    ClassDB::bind_method(D_METHOD("get_speed"), &EntityBody2D::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &EntityBody2D::set_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "speed", PropertyHint::PROPERTY_HINT_RANGE, "-1, 1, 0.001, or_less, or_greater, hide_slider, suffix:px/s"),
        "set_speed", "get_speed"
    );
    // Vector2 speed_direction
    ClassDB::bind_method(D_METHOD("get_speed_direction"), &EntityBody2D::get_speed_direction);
    ClassDB::bind_method(D_METHOD("set_speed_direction", "p_speed_direction"), &EntityBody2D::set_speed_direction);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "speed_direction"),
        "set_speed_direction", "get_speed_direction"
    );
    // bool speed_for_motion
    ClassDB::bind_method(D_METHOD("is_speed_for_motion"), &EntityBody2D::is_speed_for_motion);
    ClassDB::bind_method(D_METHOD("set_speed_for_motion", "p_speed_for_motion"), &EntityBody2D::set_speed_for_motion);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "speed_for_motion"),
        "set_speed_for_motion", "is_speed_for_motion"
    );
    // Vector2 motion
    ClassDB::bind_method(D_METHOD("get_motion"), &EntityBody2D::get_motion);
    ClassDB::bind_method(D_METHOD("set_motion", "p_motion"), &EntityBody2D::set_motion);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "motion", PropertyHint::PROPERTY_HINT_NONE, "", PropertyUsageFlags::PROPERTY_USAGE_NO_EDITOR), 
        "set_motion", "get_motion"
    );
    ADD_GROUP("Gravity", "");
    // Vector2 gravity_direction
    ClassDB::bind_method(D_METHOD("get_gravity_direction"), &EntityBody2D::get_gravity_direction);
    ClassDB::bind_method(D_METHOD("set_gravity_direction", "p_gravity_direction"), &EntityBody2D::set_gravity_direction);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "gravity_direction"), 
        "set_gravity_direction", "get_gravity_direction"
    );
    // double gravity
    ClassDB::bind_method(D_METHOD("get_gravity"), &EntityBody2D::get_gravity);
    ClassDB::bind_method(D_METHOD("set_gravity", "p_gravity"), &EntityBody2D::set_gravity);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "gravity", PROPERTY_HINT_RANGE, "-1, 1, 0.001, or_less, or_greater, hide_slider, suffix:px/s^2"),
        "set_gravity", "get_gravity"
    );
    // double max_falling_speed
    ClassDB::bind_method(D_METHOD("get_max_falling_speed"), &EntityBody2D::get_max_falling_speed);
    ClassDB::bind_method(D_METHOD("set_max_falling_speed", "p_max_falling_speed"), &EntityBody2D::set_max_falling_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "max_falling_speed", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:px/s"),
        "set_max_falling_speed", "get_max_falling_speed"
    );
    ADD_GROUP("Top", "");
    ClassDB::bind_method(D_METHOD("get_top_direction"), &EntityBody2D::get_top_direction);
    ClassDB::bind_method(D_METHOD("set_top_direction", "p_top_direction"), &EntityBody2D::set_top_direction);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "top_direction"),
        "set_top_direction", "get_top_direction"
    );

    // Register methods
    ClassDB::bind_method(D_METHOD("get_falling_velocity"), &EntityBody2D::get_falling_velocity);
    ClassDB::bind_method(D_METHOD("get_previous_velocity"), &EntityBody2D::get_previous_velocity);
    ClassDB::bind_method(D_METHOD("is_leaving_ground"), &EntityBody2D::is_leaving_ground);
    ClassDB::bind_method(D_METHOD("is_falling"), &EntityBody2D::is_falling);
    ClassDB::bind_method(D_METHOD("move_and_slide", "is_gravity_direction_rotated", "use_real_velocity"), &EntityBody2D::move_and_slide, true, false);
    ClassDB::bind_method(D_METHOD("accelerate_x", "acceleration", "to"), &EntityBody2D::accelerate_x);
    ClassDB::bind_method(D_METHOD("accelerate_y", "acceleration", "to"), &EntityBody2D::accelerate_y);
    ClassDB::bind_method(D_METHOD("accelerate", "acceleration", "to"), &EntityBody2D::accelerate);
    ClassDB::bind_method(D_METHOD("use_friction", "miu"), &EntityBody2D::use_friction);
    ClassDB::bind_method(D_METHOD("jump", "jumping_speed", "is_accumulating_mode"), &EntityBody2D::jump, false);
    ClassDB::bind_method(D_METHOD("correct_on_wall_corner", "steps"), &EntityBody2D::correct_on_wall_corner, 4);
    ClassDB::bind_method(D_METHOD("correct_onto_floor","steps"), &EntityBody2D::correct_onto_floor, 20);
}

// Constructor and Destructor
EntityBody2D::EntityBody2D() {
    // Properties' intialization
    speed = 0.0;
    speed_direction = Vector2(1, 0);
    speed_for_motion = true;
    gravity_direction = Vector2(0, 1);
    gravity = 0.0;
    max_falling_speed = 1500;
    top_direction = Vector2(0, -1);
}

EntityBody2D::~EntityBody2D() {}

// Built-in
void EntityBody2D::_enter_tree() {
    Vector2 init_speed = speed * speed_direction;
    if (get_velocity().is_zero_approx() && init_speed.is_zero_approx()) {
        if (speed_for_motion) {
            set_motion(init_speed);
        }
        else {
            set_velocity(init_speed);
        }
    }
}

// Methods
bool EntityBody2D::move_and_slide(const bool is_gravity_direction_rotated, const bool use_real_velocity) {
    bool ret = false;
    
    // Previous velocity
    Vector2 velocity = get_velocity();
    _velocity = velocity;

    // Up direction
    set_up_direction(top_direction.rotated(get_global_rotation()));
    
    // Override falling velocity
    if (is_gravity_direction_rotated) {
        gravity_direction = -get_up_direction();
    }

     // Falling
    bool on_floor = is_on_floor();
    bool is_slopable = !is_floor_stop_on_slope_enabled();
    bool is_slope_steel = !UtilityFunctions::is_zero_approx(get_floor_angle(get_up_direction()));
    bool is_able_to_slope_down = on_floor && is_slopable && is_slope_steel;

    if (!on_floor || is_able_to_slope_down) {
        _falling_velocity = gravity_direction * gravity * float(get_physics_process_delta_time());
        Vector2 velocity_falling = velocity + _falling_velocity;
        
        if (max_falling_speed > 0) {
            velocity_falling = Vec2D::get_projection_limit(velocity_falling, gravity_direction, max_falling_speed);
        }

        if (!is_floor_constant_speed_enabled()) {
            velocity_falling = velocity_falling.slide(get_floor_normal());
        }

        set_velocity(velocity_falling);
    }
        
    ret = CharacterBody2D::move_and_slide();
    if (use_real_velocity) {
        set_velocity(get_real_velocity());
    }

    // Signals emission
    if (is_on_wall()) {
        emit_signal("collided_wall", get_last_slide_collision());
    }
    if (is_on_ceiling()) {
        emit_signal("collided_ceiling", get_last_slide_collision());
    }
    if (is_on_floor()) {
        emit_signal("collided_floor", get_last_slide_collision());
    }

    return ret;
}

void EntityBody2D::accelerate_x(const double acceleration, const double to) {
    Vector2 velocity = get_velocity();
    set_velocity(velocity + Vector2(float(godot::Math::move_toward(double(velocity.x), to, godot::Math::abs(acceleration) * get_physics_process_delta_time())), 0));
}

void EntityBody2D::accelerate_y(const double acceleration, const double to) {
    Vector2 velocity = get_velocity();
    set_velocity(velocity + Vector2(0, float(godot::Math::move_toward(double(velocity.y), to, godot::Math::abs(acceleration) * get_physics_process_delta_time()))));
}

void EntityBody2D::accelerate(const double acceleration, const Vector2 &to) {
    set_velocity(get_velocity().move_toward(to, float(acceleration * get_physics_process_delta_time())));
}

void EntityBody2D::use_friction(const double miu) {
    Vector2 velocity = get_velocity();
    Vector2 slide_vel = velocity.slide(get_up_direction());
    Vector2 friction_affected_vel = slide_vel.lerp(Vector2(0, 0), float(miu * get_physics_process_delta_time()));
    Vector2 result = velocity - (slide_vel - friction_affected_vel);
    set_velocity(result);
}

void EntityBody2D::jump(const double jumping_speed, const bool is_accumulating_mode) {
    Vector2 velocity = get_velocity();
    Vector2 up_direction = get_up_direction();
    Vector2 falling_direction = get_falling_direction();
    float j_speed = UtilityFunctions::absf(jumping_speed);
    
    if (is_accumulating_mode) {
        set_velocity(velocity + up_direction * j_speed);
    } else {
        Vector2 jumping_velocity = velocity - velocity.project(gravity_direction);
        set_velocity(jumping_velocity + up_direction * j_speed);
    }
    
}

void EntityBody2D::correct_on_wall_corner(const int steps) {
    double dot = UtilityFunctions::snappedf(_velocity.dot(get_up_direction()), 0.01);
    bool on_ceiling = is_on_ceiling();
    
    // Only on ceiling or falling speed < 0 can skip this detection
    if (dot < 0.0 || UtilityFunctions::is_zero_approx(dot) || !on_ceiling) {
        return;
    }

    // Prevent from sliding up along reversed slope
    Ref<KinematicCollision2D> k = get_last_slide_collision();
    if (k == nullptr) {
        return;
    }
    else {
        double rslope = k->get_angle(get_up_direction());
        if (!UtilityFunctions::is_equal_approx(rslope, Math_PI)) {
            return;
        }
    }

    Vector2 p = get_global_position();
    double r = get_global_rotation();
    Vector2 d = Vector2(1, 0).rotated(r);
    Vector2 p_cur = p;

    // "Left" detection and correction
    for (int i = 0; i < steps; i++) {
        p_cur += d;
        set_global_position(p_cur);
        bool cl = test_move(get_global_transform(), get_up_direction());

        if (!cl) {
            p = p_cur;
            set_velocity(_velocity);
            break;
        }
    }
    // "Right" detection and correction
    for (int j = 0; j < steps; j++) {
        p_cur -= d;
        set_global_position(p_cur);
        bool cl = test_move(get_global_transform(), get_up_direction());

        if (!cl) {
            p = p_cur;
            set_velocity(_velocity);
            break;
        }
    }
    set_global_position(p);
}

void EntityBody2D::correct_onto_floor(const int steps) {
    Vector2 falling_direction = get_falling_direction();
    double dot = UtilityFunctions::snappedf(_velocity.dot(falling_direction), 0.01);
    bool on_wall = is_on_wall();
    
    // Only on wall or falling speed > 0 can skip this detection
    if (dot < 0.0 || UtilityFunctions::is_zero_approx(dot) || !on_wall || (on_wall && is_on_floor())) {
        return;
    }

    // Initialization for current position, detect-in length and pushing length
    Vector2 p = get_global_position();
    Vector2 p_in = _velocity.normalized();
    Vector2 p_push = p_in.project(falling_direction);
    Vector2 p_cur = p;

    // Looping for detection and correction
    for (int i = 0; i < steps; i++) {
        // Everytime the detection is executed, the body is to be moved upwards and detect if there is any obstacle overlapping the body
        p_cur -= p_push;
        set_global_position(p_cur);
        bool cl = test_move(get_global_transform(), p_in);

        // If no any obstacle overlapping, then the looping is to be stopped and the velocity is to inherit the previous velocity
        if (!cl) {
            p = p_cur;
            set_velocity(_velocity);
            break;
        }
    }

    // Finally, move the body back to original position, or to one after correction
    set_global_position(p);
}


// Properties' Setters and Getters
// Getters only
Vector2 EntityBody2D::get_falling_velocity() const {
    return _falling_velocity; 
}

Vector2 EntityBody2D::get_falling_direction() const {
    return _falling_velocity.normalized();
}

Vector2 EntityBody2D::get_previous_velocity() const {
    return _velocity;
}

bool EntityBody2D::is_leaving_ground() const {
    return get_velocity().dot(gravity_direction) < 0;
}

bool EntityBody2D::is_falling() const {
    return get_velocity().dot(gravity_direction) > 0;
}


// double speed
void EntityBody2D::set_speed(const double p_speed) {
    speed = p_speed;
}

double EntityBody2D::get_speed() const {
    return speed;
}

// Vector2 speed_direction
void EntityBody2D::set_speed_direction(const Vector2 &p_speed_direction) {
    speed_direction = p_speed_direction.normalized();
}

Vector2 EntityBody2D::get_speed_direction() const {
    return speed_direction.normalized();
}

// bool speed_for_motion
void EntityBody2D::set_speed_for_motion(const bool p_speed_for_motion) {
    speed_for_motion = p_speed_for_motion;
}

bool EntityBody2D::is_speed_for_motion() const {
    return speed_for_motion;
}

// Vector2 motion
void EntityBody2D::set_motion(const Vector2 &p_motion) {
    set_velocity(p_motion.rotated(get_global_rotation()));
}

Vector2 EntityBody2D::get_motion() {
    return get_velocity().rotated(-get_global_rotation());
}

// Vecotr2 gravity_direction
void EntityBody2D::set_gravity_direction(const Vector2 &p_gravity_direction) {
    gravity_direction = p_gravity_direction.normalized();
}

Vector2 EntityBody2D::get_gravity_direction() const {
    return gravity_direction;
}

// double gravity
void EntityBody2D::set_gravity(const double p_gravity) {
    gravity = p_gravity;
}

double EntityBody2D::get_gravity() const {
    return gravity;
}

// double max_falling_speed
void EntityBody2D::set_max_falling_speed(const double p_max_falling_speed) {
    max_falling_speed = p_max_falling_speed;
}

double EntityBody2D::get_max_falling_speed() const {
    return max_falling_speed;
}

// Vector2 top_direction
void EntityBody2D::set_top_direction(const Vector2 &p_top_direction) {
    top_direction = p_top_direction.normalized();
}

Vector2 EntityBody2D::get_top_direction() const {
    return top_direction.normalized();
}