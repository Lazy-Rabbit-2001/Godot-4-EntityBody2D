#include "entity_body_2d.h"

#include "mathorm.h"
#include "fast_syntax.hpp"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/physics_direct_body_state2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/kinematic_collision2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace gde_fast_syntax;

void EntityBody2D::_bind_methods()
{
    // Register signals
    ADD_SIGNAL(MethodInfo("collided_wall"));
    ADD_SIGNAL(MethodInfo("collided_ceiling"));
    ADD_SIGNAL(MethodInfo("collided_floor"));

    // Register properties
    // Vector2 velocity
    // bool autobody
    // Vector2 velocity (in current class)
    ClassDB::bind_method(D_METHOD("get_velocity"), &EntityBody2D::get_velocity);
    ClassDB::bind_method(D_METHOD("set_velocity", "p_velocity"), &EntityBody2D::set_velocity);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "velocity"),
        "set_velocity", "get_velocity"
    );
    ADD_GROUP("Autobody", "");
    ClassDB::bind_method(D_METHOD("is_autobody"), &EntityBody2D::is_autobody);
    ClassDB::bind_method(D_METHOD("set_autobody", "p_autobody"), &EntityBody2D::set_autobody);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "autobody"),
        "set_autobody", "is_autobody"
    );
    ClassDB::bind_method(D_METHOD("get_max_speed"), &EntityBody2D::get_max_speed);
    ClassDB::bind_method(D_METHOD("set_max_speed", "p_max_speed"), &EntityBody2D::set_max_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "max_speed", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:px/s"),
        "set_max_speed", "get_max_speed"
    );
    ClassDB::bind_method(D_METHOD("get_max_speed_scale"), &EntityBody2D::get_max_speed_scale);
    ClassDB::bind_method(D_METHOD("set_max_speed_scale", "p_max_speed_scale"), &EntityBody2D::set_max_speed_scale);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "max_speed_scale", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:x", PROPERTY_USAGE_NO_EDITOR),
        "set_max_speed_scale", "get_max_speed_scale"
    );
    ClassDB::bind_method(D_METHOD("is_speed_set_to_max_speed"), &EntityBody2D::is_speed_set_to_max_speed);
    ClassDB::bind_method(D_METHOD("set_speed_is_max_speed", "p_speed_is_max_speed"), &EntityBody2D::set_speed_is_max_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "speed_is_max_speed"),
        "set_speed_is_max_speed", "is_speed_set_to_max_speed"
    );
    // Vector2 velocity (in parent class)
    ClassDB::bind_method(D_METHOD("get_global_velocity"), &EntityBody2D::get_global_velocity);
    ClassDB::bind_method(D_METHOD("set_global_velocity", "p_global_velocity"), &EntityBody2D::set_global_velocity);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::VECTOR2, "global_velocity", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR),
        "set_global_velocity", "get_global_velocity"
    );
    ADD_GROUP("Rigid", "");
    // bool damp_mode
    ClassDB::bind_method(D_METHOD("is_damp_enabled"), &EntityBody2D::is_damp_enabled);
    ClassDB::bind_method(D_METHOD("set_damp_enabled", "p_damp_enabled"), &EntityBody2D::set_damp_enabled);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "damp_enabled"),
        "set_damp_enabled", "is_damp_enabled"
    );
    // bool damp_min_speed_ratio
    ClassDB::bind_method(D_METHOD("get_damp_min_speed"), &EntityBody2D::get_damp_min_speed);
    ClassDB::bind_method(D_METHOD("set_damp_min_speed", "p_damp_min_speed"), &EntityBody2D::set_damp_min_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "damp_min_speed", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:px/s"),
        "set_damp_min_speed", "get_damp_min_speed"
    );
    ADD_GROUP("Gravity", "");
    // double gravity
    ClassDB::bind_method(D_METHOD("get_gravity_scale"), &EntityBody2D::get_gravity_scale);
    ClassDB::bind_method(D_METHOD("set_gravity_scale", "p_gravity_scale"), &EntityBody2D::set_gravity_scale);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "gravity_scale", PROPERTY_HINT_RANGE, "-1, 1, 0.001, or_less, or_greater, hide_slider, suffix:x"),
        "set_gravity_scale", "get_gravity_scale"
    );
    // double max_falling_speed
    ClassDB::bind_method(D_METHOD("get_max_falling_speed"), &EntityBody2D::get_max_falling_speed);
    ClassDB::bind_method(D_METHOD("set_max_falling_speed", "p_max_falling_speed"), &EntityBody2D::set_max_falling_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "max_falling_speed", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:px/s"),
        "set_max_falling_speed", "get_max_falling_speed"
    );
    ClassDB::bind_method(D_METHOD("get_max_falling_speed_scale"), &EntityBody2D::get_max_falling_speed_scale);
    ClassDB::bind_method(D_METHOD("set_max_falling_speed_scale", "p_max_falling_speed_scale"), &EntityBody2D::set_max_falling_speed_scale);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "max_falling_speed_scale", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:x", PROPERTY_USAGE_NO_EDITOR),
        "set_max_falling_speed_scale", "get_max_falling_speed_scale"
    );
    // bool global_rotation_to_gravity_direction
    ClassDB::bind_method(D_METHOD("is_global_rotation_to_gravity_direction"), &EntityBody2D::is_global_rotation_to_gravity_direction);
    ClassDB::bind_method(D_METHOD("set_global_rotation_to_gravity_direction", "p_global_rotation_to_gravity_direction"), &EntityBody2D::set_global_rotation_to_gravity_direction);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "global_rotation_to_gravity_direction"),
        "set_global_rotation_to_gravity_direction", "is_global_rotation_to_gravity_direction"
    );
    ADD_GROUP("Up Direction", "");
    ClassDB::bind_method(D_METHOD("get_up_direction_angle"), &EntityBody2D::get_up_direction_angle);
    ClassDB::bind_method(D_METHOD("set_up_direction_angle", "p_up_direction_angle"), &EntityBody2D::set_up_direction_angle);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "up_direction_angle", PROPERTY_HINT_RANGE, "-180, 180, 0.001, degrees"),
        "set_up_direction_angle", "get_up_direction_angle"
    );

    // Register methods
    ClassDB::bind_method(D_METHOD("get_gravity_vector"), &EntityBody2D::get_gravity_vector);
    ClassDB::bind_method(D_METHOD("get_gravity_rotation_angle"), &EntityBody2D::get_gravity_rotation_angle);
    ClassDB::bind_method(D_METHOD("get_damp"), &EntityBody2D::get_damp);
    ClassDB::bind_method(D_METHOD("get_previous_velocity"), &EntityBody2D::get_previous_velocity);
    ClassDB::bind_method(D_METHOD("get_previous_global_velocity"), &EntityBody2D::get_previous_global_velocity);
    ClassDB::bind_method(D_METHOD("get_colliders"), &EntityBody2D::get_colliders);
    ClassDB::bind_method(D_METHOD("get_last_collider"), &EntityBody2D::get_last_collider);
    ClassDB::bind_method(D_METHOD("is_leaving_ground"), &EntityBody2D::is_leaving_ground);
    ClassDB::bind_method(D_METHOD("is_falling"), &EntityBody2D::is_falling);
    ClassDB::bind_method(D_METHOD("move_and_slide", "use_real_velocity"), &EntityBody2D::move_and_slide, false);
    ClassDB::bind_method(D_METHOD("accelerate_to_max_speed", "acceleration", "direction_override"), &EntityBody2D::accelerate_to_max_speed, 0);
    ClassDB::bind_method(D_METHOD("accelerate_local_x", "acceleration", "to"), &EntityBody2D::accelerate_local_x);
    ClassDB::bind_method(D_METHOD("accelerate_local_y", "acceleration", "to"), &EntityBody2D::accelerate_local_y);
    ClassDB::bind_method(D_METHOD("accelerate_local", "acceleration", "to"), &EntityBody2D::accelerate_local);
    ClassDB::bind_method(D_METHOD("accelerate_x", "acceleration", "to"), &EntityBody2D::accelerate_x);
    ClassDB::bind_method(D_METHOD("accelerate_y", "acceleration", "to"), &EntityBody2D::accelerate_y);
    ClassDB::bind_method(D_METHOD("accelerate", "acceleration", "to"), &EntityBody2D::accelerate);
    ClassDB::bind_method(D_METHOD("turn_wall"), &EntityBody2D::turn_wall);
    ClassDB::bind_method(D_METHOD("turn_ceiling_ground"), &EntityBody2D::turn_ceiling_ground);
    ClassDB::bind_method(D_METHOD("use_friction", "miu"), &EntityBody2D::use_friction);
    ClassDB::bind_method(D_METHOD("jump", "jumping_speed", "is_accumulating_mode"), &EntityBody2D::jump, false);
    ClassDB::bind_method(D_METHOD("correct_on_wall_corner", "steps"), &EntityBody2D::correct_on_wall_corner, 4);
    ClassDB::bind_method(D_METHOD("correct_onto_floor","steps"), &EntityBody2D::correct_onto_floor, 20);
}

// Constructor and Destructor
EntityBody2D::EntityBody2D() {
    // Properties' intialization
    autobody = true;
    velocity = Vector2(0, 0);
    max_speed = 0.0;
    max_speed_scale = 1.0;
    speed_is_max_speed = false;
    damp_enabled = false;
    damp_min_speed = 0.0;
    gravity_scale = 1.0;
    max_falling_speed = 1500.0;
    max_falling_speed_scale = 1.0;
    global_rotation_to_gravity_direction = true;
    up_direction_angle = 0.0;
}

EntityBody2D::~EntityBody2D() {}

// Built-in
void EntityBody2D::_enter_tree() {
    if (get_global_velocity().is_zero_approx() && !velocity.is_zero_approx()) {
        set_velocity(velocity);
    }
}


// Private methods
Vector2 EntityBody2D::_get_gravity() const {
    ProjectSettings *prjs = ProjectSettings::get_singleton();
    return double(prjs->get_setting("physics/2d/default_gravity", 980.0)) * Vector2(prjs->get_setting("physics/2d/default_gravity_vector", Vector2(0.0, 1.0)));
}


// Methods
bool EntityBody2D::move_and_slide(const bool use_real_velocity) {
    bool ret = false;

    // Previous data
    _velocity = velocity;
    _velocity_global = get_global_velocity();

    // Velocity
    Vector2 gv = _velocity_global;
    Vector2 grvec = get_gravity_vector();
    Vector2 grdir = grvec != Vector2() ? grvec.normalized() : Vector2();
    
    // Rotation
    double gangl = get_gravity_rotation_angle();
    if (global_rotation_to_gravity_direction && !UtilityFunctions::is_equal_approx(get_global_rotation(), gangl)) {
        set_global_rotation(UtilityFunctions::lerp_angle(get_global_rotation(), gangl, 22.5 * get_delta(this)));
    }
    set_up_direction(grdir != Vector2() ? -grdir.rotated(UtilityFunctions::deg_to_rad(up_direction_angle)) : get_up_direction());

    // Speed (Autobody only)
    double rot = get_up_direction().angle() + Math_PI/2.0;
    if (autobody) {
        if (max_speed * max_speed_scale > 0.0 && velocity.x != 0.0) { // Speed limitation enabled if max speed is greater than 0.0
            double mvx = UtilityFunctions::signf(velocity.x) * max_speed * max_speed_scale;
            if (speed_is_max_speed) { // Autobody mode: speed assgined to max speed
                velocity.x = mvx;
            }
            else { 
                 // If oversped, decelerate to the max speed
                 if (abs(velocity.x) > abs(mvx)) {
                    velocity.x = UtilityFunctions::move_toward(velocity.x, mvx, abs(velocity.x) * 0.1);
            }
            set_velocity(velocity);
            gv = get_global_velocity(); // Update the global velocity
            }
        }
    }
    // Damp (Rigidmode only)
    else if (damp_enabled) {
        double damp = get_damp();
        if (damp > 0.0) {
            Vector2 gvly = gv.project(grdir);
            Vector2 gvlx = gv - gvly;
            gvlx = gvlx.move_toward(gvlx.normalized() * damp_min_speed, damp);
            gv = gvlx + gvly;
        }
    }

    // Falling
    if (!is_on_floor()) {
        double mfs = max_falling_speed * max_falling_speed_scale;
        gv += grvec * gravity_scale * get_delta(this);
        bool on_falling = gv.dot(grdir) > 0.0;
        if (mfs > 0.0 && on_falling) {
            gv = Transform2DAlgo::get_projection_limit(gv, grdir, mfs);
        }
    }
    else {
        gv += grdir; // To ensure the body will correctly on the floor, and this will cause signal collided_on_floor emitted continuously on the floor
    }

    // Set global velocity
    set_global_velocity(gv);

    // Movement
    ret = CharacterBody2D::move_and_slide();
    if (use_real_velocity) {
        set_global_velocity(get_real_velocity());
        if (autobody) {
            velocity.x = get_global_velocity().rotated(-rot).x;
        }
    }
    gv = get_global_velocity();
    set_global_velocity(gv); // Update the velocity in the final frame

    // Signals emission;
    if (is_on_wall()) {
        emit_signal("collided_wall");
    }
    if (is_on_ceiling()) {
        emit_signal("collided_ceiling");
    }
    if (is_on_floor()) {
        emit_signal("collided_floor");
    }

    /* // Fix abnormal grounding
    double mod = UtilityFunctions::fmod(get_up_direction().angle(), Math_PI/2.0);
    bool abnormal_gravity = !UtilityFunctions::is_zero_approx(mod);
    if (!is_on_floor() && abnormal_gravity && test_move(get_global_transform(), grvec.normalized())) {
        set_global_velocity(gv + grdir * 12.0);
        move_and_slide();
    } */

    return ret;
}

void EntityBody2D::accelerate_to_max_speed(const double acceleration, const int direction_override) {
    if (!autobody) {
        return;
    }
    accelerate_local_x(acceleration, max_speed * max_speed_scale * (direction_override == 0 ? UtilityFunctions::signf(velocity.x) : direction_override));
}

void EntityBody2D::accelerate_local_x(const double acceleration, const double to) {
    velocity.x = UtilityFunctions::move_toward(velocity.x, to, acceleration * get_delta(this));
    set_velocity(velocity);
}

void EntityBody2D::accelerate_local_y(const double acceleration, const double to) {
    velocity.y = UtilityFunctions::move_toward(velocity.x, to, acceleration * get_delta(this));
    set_velocity(velocity);
}

void EntityBody2D::accelerate_local(const double acceleration, const Vector2 &to) {
    set_velocity(velocity.move_toward(to, acceleration * get_delta(this)));
}

void EntityBody2D::accelerate_x(const double acceleration, const double to) {
    Vector2 gv = get_global_velocity();
    gv.x = UtilityFunctions::move_toward(velocity.x, to, acceleration * get_delta(this));
    set_velocity(gv);
}

void EntityBody2D::accelerate_y(const double acceleration, const double to) {
    Vector2 gv = get_global_velocity();
    gv.y = UtilityFunctions::move_toward(velocity.x, to, acceleration * get_delta(this));
    set_velocity(gv);
}

void EntityBody2D::accelerate(const double acceleration, const Vector2 &to) {
    set_global_velocity(get_global_velocity().move_toward(to, acceleration * get_delta(this)));
}

void EntityBody2D::turn_wall() {
    // Autobody mode
    if (autobody) {
        if (_velocity.x != 0.0){
            velocity.x = -_velocity.x;
        }
        else {
            velocity.x *= -1.0;
        }
    }
    // Rigid mode
    else if (_velocity_global != Vector2()) {
        set_global_velocity(_velocity_global.reflect(get_up_direction()));
    }
    else {
        set_global_velocity(get_global_velocity().reflect(get_up_direction()));
    }
}

void EntityBody2D::turn_ceiling_ground() {
     if (_velocity_global != Vector2()) {
        set_global_velocity(_velocity_global.bounce(get_up_direction()));
    }
    else {
        set_global_velocity(get_global_velocity().bounce(get_up_direction()));
    }
}

void EntityBody2D::use_friction(const double miu) {
    if (!is_on_floor()) {
        return;
    }
    // Autobody mode
    if (autobody) {
        velocity.x = UtilityFunctions::lerpf(velocity.x, 0, miu * get_delta(this));
    }
    // Rigid mode
    else {
        Vector2 gv = get_global_velocity();
        Vector2 gn = gv.slide(get_floor_normal());
        gv = gn.lerp(gn *0.0, miu * get_delta(this));
        set_global_velocity(gv);
    }
}

void EntityBody2D::jump(const double jumping_speed, const bool is_accumulating_mode) {
    Vector2 gv = get_global_velocity();
    
    if (!is_accumulating_mode) {
        gv -= gv.project(get_gravity_vector().normalized());
    }
    gv += get_up_direction() * abs(jumping_speed);
    
    set_global_velocity(gv);
}

void EntityBody2D::correct_on_wall_corner(const int steps) {
    // Only on ceiling or falling speed < 0 can skip this detection
    if (!is_on_ceiling() || is_on_floor() || is_falling()) {
        return;
    }

    // Prevent from sliding up along reversed slope
    Ref<KinematicCollision2D> &k = get_last_slide_collision();
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
    // Only on wall or falling speed > 0 can skip this detection
    if (is_on_floor() || !is_on_wall_only() || is_leaving_ground()) {
        return;
    }

    // Initialization for current position, detect-in length and pushing length
    Vector2 v = _velocity_global * get_delta(this);
    Vector2 p_push = get_up_direction();
    
    if (UtilityFunctions::is_zero_approx(v.project(p_push.orthogonal()).length_squared())) {
        return;
    }

    Vector2 p = get_global_position();
    Vector2 p_cur = p;

    // Looping for detection and correction
    for (int i = 0; i < steps; i++) {
        p_cur += p_push;
        set_global_position(p_cur);

        if (!test_move(get_global_transform(), v)) {
            p = p_cur;
            velocity.x = _velocity.x;
            set_global_velocity(_velocity_global);
            break;
        }
    }

    // Finally, move the body back to original position, or to one after correction
    set_global_position(p);
}


// Properties' Setters and Getters
// Getters only
Vector2 EntityBody2D::get_previous_velocity() const {
    return _velocity;
}

Vector2 EntityBody2D::get_previous_global_velocity() const {
    return _velocity_global;
}

Vector2 EntityBody2D::get_gravity_vector() const {
    PhysicsDirectBodyState2D *pdbs = PhysicsServer2D::get_singleton()->body_get_direct_state(get_rid());
    
    if (pdbs == nullptr) {
        return _get_gravity();
    }

    Vector2 gv = pdbs->get_total_gravity();
    return gv;
}

double EntityBody2D::get_gravity_rotation_angle() const {
    return get_gravity_vector().angle() - Math_PI / 2;
}

double EntityBody2D::get_damp() const {
    PhysicsDirectBodyState2D *pdbs = PhysicsServer2D::get_singleton()->body_get_direct_state(get_rid());
    
    if (pdbs == nullptr) {
        return 0.0;
    }

    return pdbs->get_total_linear_damp();
}

TypedArray<Node2D> EntityBody2D::get_colliders() {
    TypedArray<Node2D> objs = TypedArray<Node2D>::make();

    for(int i = 0; i < get_slide_collision_count(); i++) {
        Ref<KinematicCollision2D> &kc = get_slide_collision(i);
        if (!UtilityFunctions::is_instance_valid(kc)) {
            continue;
        }

        Object *o = Object::cast_to<Node2D>(kc->get_collider());
        if (o == nullptr) {
            continue;
        }

        objs.append(o);
    }

    return objs;
}

Node2D* EntityBody2D::get_last_collider() {
    Ref<KinematicCollision2D> &kc = get_last_slide_collision();
    if (!UtilityFunctions::is_instance_valid(kc)) {
        return nullptr;
    }
    return Object::cast_to<Node2D>(kc->get_collider());
}

bool EntityBody2D::is_leaving_ground() const {
    return get_velocity().dot(get_gravity_vector().normalized()) < 0.0;
}

bool EntityBody2D::is_falling() const {
    return get_velocity().dot(get_gravity_vector().normalized()) > 0.0;
}

// Vector2 velocity
void EntityBody2D::set_velocity(const Vector2 &p_velocity) {
    velocity = p_velocity;
    CharacterBody2D::set_velocity(velocity.rotated(get_up_direction().angle() + Math_PI/2.0));
}

Vector2 EntityBody2D::get_velocity() const {
    return velocity;
}

void EntityBody2D::set_max_speed(const double p_max_speed) {
    max_speed = p_max_speed;
}

double EntityBody2D::get_max_speed() const {
    return max_speed;
}

void EntityBody2D::set_max_speed_scale(const double p_max_speed_scale) {
    max_speed_scale = p_max_speed_scale;
}

double EntityBody2D::get_max_speed_scale() const {
    return max_speed_scale;
}

void EntityBody2D::set_speed_is_max_speed(const bool p_speed_is_max_speed) {
    speed_is_max_speed = p_speed_is_max_speed;
}

bool EntityBody2D::is_speed_set_to_max_speed() const {
    return speed_is_max_speed;
}

// Vector2 velocity (in parent class)
void EntityBody2D::set_global_velocity(const Vector2 &p_global_velocity) {
    CharacterBody2D::set_velocity(p_global_velocity);
    velocity = CharacterBody2D::get_velocity().rotated(-get_up_direction().angle() - Math_PI/2.0);
}

Vector2 EntityBody2D::get_global_velocity() const {
    return CharacterBody2D::get_velocity();
}

void EntityBody2D::set_damp_enabled(const bool p_damp_enabled) {
    damp_enabled = p_damp_enabled;
}

bool EntityBody2D::is_damp_enabled() const {
    return damp_enabled;
}

void EntityBody2D::set_damp_min_speed(const double p_damp_min_speed) {
    damp_min_speed = p_damp_min_speed;
}

double EntityBody2D::get_damp_min_speed() const {
    return damp_min_speed;
}

void EntityBody2D::set_autobody(const bool p_autobody) {
    autobody = p_autobody;
}

bool EntityBody2D::is_autobody() const {
    return autobody;
}

// double gravity
void EntityBody2D::set_gravity_scale(const double p_gravity_scale) {
    gravity_scale = p_gravity_scale;
}

double EntityBody2D::get_gravity_scale() const {
    return gravity_scale;
}

// double max_falling_speed
void EntityBody2D::set_max_falling_speed(const double p_max_falling_speed) {
    max_falling_speed = p_max_falling_speed;
}

double EntityBody2D::get_max_falling_speed() const {
    return max_falling_speed;
}

void EntityBody2D::set_max_falling_speed_scale(const double p_max_falling_speed_scale) {
    max_falling_speed_scale = p_max_falling_speed_scale;
}

double EntityBody2D::get_max_falling_speed_scale() const {
    return max_falling_speed_scale;
}

void EntityBody2D::set_global_rotation_to_gravity_direction(const bool p_global_rotation_to_gravity_direction) {
    global_rotation_to_gravity_direction = p_global_rotation_to_gravity_direction;
}

bool EntityBody2D::is_global_rotation_to_gravity_direction() const {
    return global_rotation_to_gravity_direction;
}

// Vector2 top_direction
void EntityBody2D::set_up_direction_angle(const double p_up_direction_angle) {
    up_direction_angle = p_up_direction_angle;
}

double EntityBody2D::get_up_direction_angle() const {
    return up_direction_angle;
}