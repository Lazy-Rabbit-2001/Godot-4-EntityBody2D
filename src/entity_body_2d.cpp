#include "entity_body_2d.h"

#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void EntityBody2D::_bind_methods() {
    // Register signals
    ADD_SIGNAL(MethodInfo("collided_wall", PropertyInfo(Variant::OBJECT, "collision_info")));
    ADD_SIGNAL(MethodInfo("collided_ceiling", PropertyInfo(Variant::OBJECT, "collision_info")));
    ADD_SIGNAL(MethodInfo("collided_floor", PropertyInfo(Variant::OBJECT, "collision_info")));

    // Register properties
    // Vector2 velocity
    ADD_GROUP("Velocity", "");
    ClassDB::bind_method(D_METHOD("get_velocity"), &EntityBody2D::get_velocity);
    ClassDB::bind_method(D_METHOD("set_velocity", "p_velocity"), &EntityBody2D::set_velocity);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "velocity"),
        "set_velocity", "get_velocity"
    );
    // Vector2 velocity (in parent class)
    ClassDB::bind_method(D_METHOD("get_global_velocity"), &EntityBody2D::get_global_velocity);
    ClassDB::bind_method(D_METHOD("set_global_velocity", "p_global_velocity"), &EntityBody2D::set_global_velocity);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "global_velocity", PropertyHint::PROPERTY_HINT_NONE, "", PropertyUsageFlags::PROPERTY_USAGE_NO_EDITOR),
        "set_global_velocity", "get_global_velocity"
    );
    // bool speed_for_motion
    ClassDB::bind_method(D_METHOD("is_movement_local"), &EntityBody2D::is_movement_local);
    ClassDB::bind_method(D_METHOD("set_movement_local", "p_movement_local"), &EntityBody2D::set_movement_local);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "movement_local"),
        "set_movement_local", "is_movement_local"
    );
    ADD_GROUP("Gravity", "");
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
    ClassDB::bind_method(D_METHOD("get_gravity_direction"), &EntityBody2D::get_gravity_direction);
    ClassDB::bind_method(D_METHOD("get_previous_velocity"), &EntityBody2D::get_previous_velocity);
    ClassDB::bind_method(D_METHOD("get_previous_global_velocity"), &EntityBody2D::get_previous_global_velocity);
    ClassDB::bind_method(D_METHOD("is_leaving_ground"), &EntityBody2D::is_leaving_ground);
    ClassDB::bind_method(D_METHOD("is_falling"), &EntityBody2D::is_falling);
    ClassDB::bind_method(D_METHOD("move_and_slide", "use_real_velocity"), &EntityBody2D::move_and_slide, false);
    ClassDB::bind_method(D_METHOD("analyse_global_velocity"), &EntityBody2D::analyse_global_velocity);
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
    velocity = Vector2(0, 0);
    movement_local = true;
    gravity = 0.0;
    max_falling_speed = 1500;
    top_direction = Vector2(0, -1);
}

EntityBody2D::~EntityBody2D() {}

// Built-in
void EntityBody2D::_enter_tree() {
    if (get_global_velocity().is_zero_approx()) {
        set_velocity(velocity);
    }
}

// Methods
bool EntityBody2D::move_and_slide(const bool use_real_velocity) {
    bool ret = false;
    
    // Previous velocity
    _velocity = velocity;
    _velocity_global = get_global_velocity();

    // Falling
    if (!is_on_floor()) {
        velocity.y += double(gravity * _get_delta());
    
        if (max_falling_speed > 0 && velocity.y > max_falling_speed) {
            velocity.y = max_falling_speed;
        }
    }
    else if (UtilityFunctions::is_zero_approx(velocity.y)){
        velocity.y = 1;
    }

    // Movement
    set_velocity(velocity);
    ret = CharacterBody2D::move_and_slide();
    if (use_real_velocity) {
        set_velocity(get_real_velocity());
    }
    analyse_global_velocity();

    // Fix abnormal grounding
    double mod = UtilityFunctions::fmod(get_up_direction().angle(), Math_PI/2);
    bool abnormal_gravity = !UtilityFunctions::is_zero_approx(mod);
    if (!is_on_floor() && abnormal_gravity && test_move(get_global_transform(), get_gravity_direction())) {
        Vector2 v = velocity;
        set_velocity(Vector2(0, 24));
        move_and_slide();
        set_velocity(v);
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

void EntityBody2D::analyse_global_velocity() {
    velocity = CharacterBody2D::get_velocity().rotated(movement_local ? -get_global_rotation() : 0.0);
}

void EntityBody2D::accelerate_x(const double acceleration, const double to) {
    velocity.x = UtilityFunctions::move_toward(velocity.x, to, acceleration * _get_delta());
    set_velocity(velocity);
}

void EntityBody2D::accelerate_y(const double acceleration, const double to) {
    velocity.y = UtilityFunctions::move_toward(velocity.x, to, acceleration * _get_delta());
    set_velocity(velocity);
}

void EntityBody2D::accelerate(const double acceleration, const Vector2 &to) {
    velocity = velocity.move_toward(to, acceleration * _get_delta());
    set_velocity(velocity);
}

void EntityBody2D::use_friction(const double miu) {
    if (!is_on_floor()) {
        return;
    }
    velocity.x = UtilityFunctions::lerpf(velocity.x, 0, miu * _get_delta());
    set_velocity(velocity);
}

void EntityBody2D::jump(const double jumping_speed, const bool is_accumulating_mode) {
    if (is_accumulating_mode) {
        velocity.y -= abs(jumping_speed);
    } else {
        velocity.y = -abs(jumping_speed);
    }
    set_velocity(velocity);
}

void EntityBody2D::correct_on_wall_corner(const int steps) {
    double dot = UtilityFunctions::snappedf(_velocity.dot(get_up_direction()), 0.01);
    
    // Only on ceiling or falling speed < 0 can skip this detection
    if (dot < 0.0 || UtilityFunctions::is_zero_approx(dot) || !is_on_ceiling() || is_on_floor()) {
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
            set_global_position(p);
            return;
        }
    }
    p_cur = p;
    // "Right" detection and correction
    for (int j = 0; j < steps; j++) {
        p_cur -= d;
        set_global_position(p_cur);
        bool cl = test_move(get_global_transform(), get_up_direction());

        if (!cl) {
            p = p_cur;
            set_velocity(_velocity);
            set_global_position(p);
            return;
        }
    }
    set_global_position(p);
}

void EntityBody2D::correct_onto_floor(const int steps) {
    double dot = UtilityFunctions::snappedf(_velocity.dot(get_gravity_direction()), 0.01);
    bool on_wall = is_on_wall();
    
    // Only on wall or falling speed > 0 can skip this detection
    if (dot < 0.0 || !on_wall || (on_wall && is_on_floor()) || is_on_ceiling()) {
        return;
    }

    // Initialization for current position, detect-in length and pushing length
    Vector2 p = get_global_position();
    Vector2 p_in = _velocity_global.normalized();
    Vector2 p_push = p_in.project(get_up_direction());
    Vector2 p_cur = p;

    // Looping for detection and correction
    for (int i = 0; i < steps; i++) {
        if (abs(p_in.dot(get_gravity_direction())) == 1.0) {
            break;
        }

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
Vector2 EntityBody2D::get_gravity_direction() const {
    return -get_up_direction();
}

Vector2 EntityBody2D::get_previous_velocity() const {
    return _velocity;
}

Vector2 EntityBody2D::get_previous_global_velocity() const {
    return _velocity_global;
}

bool EntityBody2D::is_leaving_ground() const {
    return get_velocity().dot(get_up_direction()) > 0.0;
}

bool EntityBody2D::is_falling() const {
    return get_velocity().dot(get_up_direction()) < 0.0;
}


// Get physics
double EntityBody2D::_get_delta() {
    return Engine::get_singleton()->is_in_physics_frame() ? get_physics_process_delta_time() : get_process_delta_time();
}


// Vector2 velocity
void EntityBody2D::set_velocity(const Vector2 &p_velocity) {
    velocity = p_velocity;
    set_global_velocity(p_velocity.rotated(movement_local ? get_global_rotation() : 0.0));
    set_up_direction(top_direction.rotated(get_global_rotation()));
}

Vector2 EntityBody2D::get_velocity() const {
    return velocity;
}

// Vector2 velocity (in parent class)
void EntityBody2D::set_global_velocity(const Vector2 &p_global_velocity) {
    CharacterBody2D::set_velocity(p_global_velocity);
}

Vector2 EntityBody2D::get_global_velocity() const {
    return CharacterBody2D::get_velocity();
}

// bool speed_for_motion
void EntityBody2D::set_movement_local(const bool p_movement_local) {
    movement_local = p_movement_local;

    if (p_movement_local) {
        set_velocity(velocity);
    }
    else {
        set_global_velocity(velocity);
    }
}

bool EntityBody2D::is_movement_local() const {
    return movement_local;
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