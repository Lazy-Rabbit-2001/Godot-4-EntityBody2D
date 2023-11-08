#include "entity_body_2d.h"

#include "mathorm.h"
#include "fast_syntax.hpp"

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
    // bool damp_mode
    ClassDB::bind_method(D_METHOD("is_damp_mode"), &EntityBody2D::is_damp_mode);
    ClassDB::bind_method(D_METHOD("set_damp_mode", "p_damp_mode"), &EntityBody2D::set_damp_mode);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "damp_mode"),
        "set_damp_mode", "is_damp_mode"
    );
    // bool damp_min_speed_ratio
    ClassDB::bind_method(D_METHOD("get_damp_min_speed_ratio"), &EntityBody2D::get_damp_min_speed_ratio);
    ClassDB::bind_method(D_METHOD("set_damp_min_speed_ratio", "p_damp_min_speed_ratio"), &EntityBody2D::set_damp_min_speed_ratio);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "damp_min_speed_ratio", PROPERTY_HINT_RANGE, "0.1, 1.0, 0.0001, suffix:x"),
        "set_damp_min_speed_ratio", "get_damp_min_speed_ratio"
    );
    // bool autobody
    ClassDB::bind_method(D_METHOD("is_autobody"), &EntityBody2D::is_autobody);
    ClassDB::bind_method(D_METHOD("set_autobody", "p_autobody"), &EntityBody2D::set_autobody);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::BOOL, "autobody"),
        "set_autobody", "is_autobody"
    );
    // double speed
    ClassDB::bind_method(D_METHOD("get_speed"), &EntityBody2D::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &EntityBody2D::set_speed);
    ClassDB::add_property(
        "EntityBody2D", PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "-1, 1, 0.001, or_less, or_greater, hide_slider, suffix:px/s"),
        "set_speed", "get_speed"
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
    ClassDB::bind_method(D_METHOD("add_entity_area_to_list", "entity_area"), &EntityBody2D::add_entity_area_to_list);
    ClassDB::bind_method(D_METHOD("remove_entity_area_from_list", "entity_area"), &EntityBody2D::remove_entity_area_from_list);
    ClassDB::bind_method(D_METHOD("clear_entity_areas_list"), &EntityBody2D::clear_entity_areas_list);
    ClassDB::bind_method(D_METHOD("is_in_entity_areas_list", "entity_area"), &EntityBody2D::is_in_entity_areas_list);
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
    ClassDB::bind_method(D_METHOD("accelerate_speed", "acceleration", "to"), &EntityBody2D::accelerate_speed);
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
    velocity = Vector2(0, 0);
    damp_mode = false;
    damp_min_speed_ratio = 0.4;
    autobody = true;
    speed = 0.0;
    gravity = 0.0;
    max_falling_speed = 1500.0;
    global_rotation_to_gravity_direction = true;
    up_direction_angle = 0.0;
}

EntityBody2D::~EntityBody2D() {}

// Built-in
void EntityBody2D::_enter_tree() {
    if (!is_running_mode(this)) {
        return;
    }
    if (get_global_velocity().is_zero_approx() && !velocity.is_zero_approx()) {
        set_velocity(velocity);
    }
}

// Methods
bool EntityBody2D::move_and_slide(const bool use_real_velocity) {
    bool ret = false;

    // Previous data
    _speed = speed;
    _velocity = velocity;
    _velocity_global = get_global_velocity();

    // Velocity
    Vector2 gv = _velocity_global;
    Vector2 grvec = get_gravity_vector();
    Vector2 grdir = grvec.normalized();
    Vector2 gunit = grvec / GRAVITY_DEFAULT;
    
    // Rotation
    double gangl = get_gravity_rotation_angle();
    if (global_rotation_to_gravity_direction && !UtilityFunctions::is_equal_approx(get_global_rotation(), gangl)) {
        set_global_rotation(UtilityFunctions::lerp_angle(get_global_rotation(), gangl, 22.5 * get_delta(this)));
    }
    set_up_direction(-grdir.rotated(UtilityFunctions::deg_to_rad(up_direction_angle)));

    // Damp & speed
    double rot = get_up_direction().angle() + Math_PI/2.0;
    if (autobody) {
        if (_init_speed == 0.0) {
            _init_speed = speed;
        }
        speed = UtilityFunctions::move_toward(speed, _init_speed * damp_min_speed_ratio, damp_mode ? get_damp() : 0.0);
        Vector2 vrot = gv.rotated(-rot);
        vrot.x = speed;
        gv = vrot.rotated(rot);
    }
    else {
        Vector2 gvly = gv.project(grdir);
        Vector2 gvlx = gv - gvly;
        if (_init_velocity_global_axis_x == 0.0) {
            _init_velocity_global_axis_x = gvlx.length();
        }
        gvlx = gvlx.move_toward(gvlx.normalized() * _init_velocity_global_axis_x * damp_min_speed_ratio, damp_mode ? get_damp() : 0.0);
        gv = gvlx + gvly;
    }

    // Falling
    calculate_final_max_falling_speed_ratio();
    double mfs = _final_max_falling_speed_ratio > 0.0 ? max_falling_speed * _final_max_falling_speed_ratio : max_falling_speed;
    if (!is_on_floor()) {
        gv += gunit * gravity * get_delta(this);
        bool on_falling = gv.dot(grdir) > 0.0;
        if (mfs > 0.0 && on_falling) {
            gv = Transform2DAlgo::get_projection_limit(gv, grdir, mfs);
        }
    }
    else {
        gv += grdir;
    }

    // Set global velocity
    set_global_velocity(gv);

    // Movement
    ret = CharacterBody2D::move_and_slide();
    if (use_real_velocity) {
        set_global_velocity(get_real_velocity());
        speed = get_global_velocity().rotated(-rot).x;
    }
    gv = get_global_velocity();

    // Signals emission;
    if (is_on_wall()) {
        speed = 0.0;
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

void EntityBody2D::accelerate_speed(const double acceleration, const double to) {
    speed = UtilityFunctions::move_toward(speed, to, acceleration * get_delta(this));
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
        if (_speed != 0.0){
            speed = -_speed;
        }
        else {
            speed *= -1.0;
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
        speed = UtilityFunctions::lerpf(speed, 0, miu * get_delta(this));
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
            speed = _speed;
            set_global_velocity(_velocity_global);
            break;
        }
    }

    // Finally, move the body back to original position, or to one after correction
    set_global_position(p);
}


void EntityBody2D::add_entity_area_to_list(EntityArea2D *entity_area) {
    if (is_in_entity_areas_list(entity_area)) {
        return;
    }
    _entity_areas.append(entity_area);
}

bool EntityBody2D::is_in_entity_areas_list(EntityArea2D *entity_area) {
    return _entity_areas.has(entity_area);
}

void EntityBody2D::remove_entity_area_from_list(EntityArea2D *entity_area) {
    if (!is_in_entity_areas_list(entity_area)) {
        return;
    }
    _entity_areas.erase(entity_area);
}

TypedArray<Area2D> EntityBody2D::get_entity_areas_from_list() const {
    return _entity_areas;
}

void EntityBody2D::clear_entity_areas_list()
{
    _entity_areas.clear();
}


void EntityBody2D::calculate_final_max_falling_speed_ratio() {
    _final_max_falling_speed_ratio = 0.0;
    int eas = _entity_areas.size();
    for (int i = 0; i < eas; i++) {
        _final_max_falling_speed_ratio += Object::cast_to<EntityArea2D>(_entity_areas[i])->get_max_falling_speed_ratio();
    }
    _final_max_falling_speed_ratio /= double(eas);
}


// Properties' Setters and Getters
// Getters only
Vector2 EntityBody2D::get_previous_velocity() const {
    return _velocity;
}

Vector2 EntityBody2D::get_previous_global_velocity() const {
    return _velocity_global;
}

double EntityBody2D::get_previous_speed() const {
    return _speed;
}

Vector2 EntityBody2D::get_gravity_vector() const {
    PhysicsDirectBodyState2D *pdbs = PhysicsServer2D::get_singleton()->body_get_direct_state(get_rid());
    
    if (pdbs == nullptr) {
        return Vector2(0, GRAVITY_DEFAULT);
    }

    Vector2 gv = pdbs->get_total_gravity();
    return gv != Vector2() ? gv : Vector2(0, GRAVITY_DEFAULT);
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
    CharacterBody2D::set_velocity(velocity.rotated(get_global_rotation()));
}

Vector2 EntityBody2D::get_velocity() const {
    return velocity;
}

// Vector2 velocity (in parent class)
void EntityBody2D::set_global_velocity(const Vector2 &p_global_velocity) {
    CharacterBody2D::set_velocity(p_global_velocity);
    velocity = CharacterBody2D::get_velocity().rotated(-get_global_rotation());
}

Vector2 EntityBody2D::get_global_velocity() const {
    return CharacterBody2D::get_velocity();
}

void EntityBody2D::set_damp_mode(const bool p_damp_mode) {
    damp_mode = p_damp_mode;
}

bool EntityBody2D::is_damp_mode() const {
    return damp_mode;
}

void EntityBody2D::set_damp_min_speed_ratio(const double p_damp_min_speed_ratio) {
    damp_min_speed_ratio = p_damp_min_speed_ratio;
}

double EntityBody2D::get_damp_min_speed_ratio() const {
    return damp_min_speed_ratio;
}

void EntityBody2D::set_autobody(const bool p_autobody) {
    autobody = p_autobody;
}

bool EntityBody2D::is_autobody() const {
    return autobody;
}

void EntityBody2D::set_speed(const double p_speed) {
    speed = p_speed;
}

double EntityBody2D::get_speed() const {
    return speed;
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