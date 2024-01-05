#include "entity_body_2d.h"

#include "mathorm.h"
#include "fast_syntax.hpp"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/physics_direct_body_state2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/tile_set.hpp>
#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/classes/static_body2d.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>
#include <godot_cpp/classes/physics_material.hpp>
#include <godot_cpp/classes/kinematic_collision2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace gde_fast_syntax;

void EntityBody2D::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("collided_wall"));
    ADD_SIGNAL(MethodInfo("collided_ceiling"));
    ADD_SIGNAL(MethodInfo("collided_floor"));
    
    ClassDB::bind_method(D_METHOD("set_velocality", "p_velocality"), &EntityBody2D::set_velocity);
    ClassDB::bind_method(D_METHOD("get_velocality"), &EntityBody2D::get_velocity);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::VECTOR2, "velocality"),"set_velocality", "get_velocality");
    ClassDB::bind_method(D_METHOD("set_velocity", "p_velocity"), &EntityBody2D::set_global_velocity);
    ClassDB::bind_method(D_METHOD("get_velocity"), &EntityBody2D::get_global_velocity);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::VECTOR2, "velocity", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR),"set_velocity", "get_velocity");
    ADD_GROUP("Threshold Speed", "threshold_speed_");
    ClassDB::bind_method(D_METHOD("set_threshold_speed_enabled", "p_threshold_speed_enabled"), &EntityBody2D::set_threshold_speed_enabled);
    ClassDB::bind_method(D_METHOD("is_threshold_speed_enabled"), &EntityBody2D::is_threshold_speed_enabled);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::BOOL, "threshold_speed_enabled"),"set_threshold_speed_enabled", "is_threshold_speed_enabled");
    ClassDB::bind_method(D_METHOD("set_threshold_speed", "p_threshold_speed"), &EntityBody2D::set_threshold_speed);
    ClassDB::bind_method(D_METHOD("get_threshold_speed"), &EntityBody2D::get_threshold_speed);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::FLOAT, "threshold_speed", PROPERTY_HINT_RANGE, "-0.001, 1, 0.001, or_greater, hide_slider, suffix:px/s"),"set_threshold_speed", "get_threshold_speed");
    ClassDB::bind_method(D_METHOD("set_threshold_speed_correction_acceleration", "p_threshold_speed_correction_acceleration"), &EntityBody2D::set_threshold_speed_correction_acceleration);
    ClassDB::bind_method(D_METHOD("get_threshold_speed_correction_acceleration"), &EntityBody2D::get_threshold_speed_correction_acceleration);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::FLOAT, "threshold_speed_correction_acceleration", PROPERTY_HINT_RANGE, U"0, 1, 0.001, or_greater, hide_slider, suffix:px/s\u00B2"),"set_threshold_speed_correction_acceleration", "get_threshold_speed_correction_acceleration");
    ADD_GROUP("Damp", "");
    ClassDB::bind_method(D_METHOD("set_damp_enabled", "p_damp_enabled"), &EntityBody2D::set_damp_enabled);
    ClassDB::bind_method(D_METHOD("is_damp_enabled"), &EntityBody2D::is_damp_enabled);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::BOOL, "damp_enabled"),"set_damp_enabled", "is_damp_enabled");
    ClassDB::bind_method(D_METHOD("set_damp_min_speed", "p_damp_min_speed"), &EntityBody2D::set_damp_min_speed);
    ClassDB::bind_method(D_METHOD("get_damp_min_speed"), &EntityBody2D::get_damp_min_speed);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::FLOAT, "damp_min_speed", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:px/s"),"set_damp_min_speed", "get_damp_min_speed");
    ADD_GROUP("Gravity", "");
    ClassDB::bind_method(D_METHOD("set_gravity_scale", "p_gravity_scale"), &EntityBody2D::set_gravity_scale);
    ClassDB::bind_method(D_METHOD("get_gravity_scale"), &EntityBody2D::get_gravity_scale);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::FLOAT, "gravity_scale", PROPERTY_HINT_RANGE, "-1, 1, 0.001, or_less, or_greater, hide_slider, suffix:x"),"set_gravity_scale", "get_gravity_scale");
    ClassDB::bind_method(D_METHOD("set_max_falling_speed", "p_max_falling_speed"), &EntityBody2D::set_max_falling_speed);
    ClassDB::bind_method(D_METHOD("get_max_falling_speed"), &EntityBody2D::get_max_falling_speed);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::FLOAT, "max_falling_speed", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:px/s"),"set_max_falling_speed", "get_max_falling_speed");
    ClassDB::bind_method(D_METHOD("set_global_rotation_to_gravity_direction", "p_global_rotation_to_gravity_direction"), &EntityBody2D::set_global_rotation_to_gravity_direction);
    ClassDB::bind_method(D_METHOD("is_global_rotation_to_gravity_direction"), &EntityBody2D::is_global_rotation_to_gravity_direction);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::BOOL, "global_rotation_to_gravity_direction"),"set_global_rotation_to_gravity_direction", "is_global_rotation_to_gravity_direction");
    ADD_GROUP("Up Direction", "");
    ClassDB::bind_method(D_METHOD("set_up_direction_angle", "p_up_direction_angle"), &EntityBody2D::set_up_direction_angle);
    ClassDB::bind_method(D_METHOD("get_up_direction_angle"), &EntityBody2D::get_up_direction_angle);
    ClassDB::add_property("EntityBody2D", PropertyInfo(Variant::FLOAT, "up_direction_angle", PROPERTY_HINT_RANGE, "-180, 180, 0.001, degrees"),"set_up_direction_angle", "get_up_direction_angle");

    ClassDB::bind_method(D_METHOD("move_and_slide", "use_real_velocity"), &EntityBody2D::move_and_slide, false);
    ClassDB::bind_method(D_METHOD("calculate_gravity"), &EntityBody2D::calculate_gravity);
    ClassDB::bind_method(D_METHOD("calculate_damp"), &EntityBody2D::calculate_damp);
    ClassDB::bind_method(D_METHOD("accelerate_local_x", "acceleration", "to"), &EntityBody2D::accelerate_local_x);
    ClassDB::bind_method(D_METHOD("accelerate_local_y", "acceleration", "to"), &EntityBody2D::accelerate_local_y);
    ClassDB::bind_method(D_METHOD("accelerate_local", "acceleration", "to"), &EntityBody2D::accelerate_local);
    ClassDB::bind_method(D_METHOD("accelerate_x", "acceleration", "to"), &EntityBody2D::accelerate_x);
    ClassDB::bind_method(D_METHOD("accelerate_y", "acceleration", "to"), &EntityBody2D::accelerate_y);
    ClassDB::bind_method(D_METHOD("accelerate", "acceleration", "to"), &EntityBody2D::accelerate);
    ClassDB::bind_method(D_METHOD("decelerate_with_friction", "deceleration"), &EntityBody2D::decelerate_with_friction);
    ClassDB::bind_method(D_METHOD("use_friction", "miu"), &EntityBody2D::use_friction);
    ClassDB::bind_method(D_METHOD("turn_wall"), &EntityBody2D::turn_wall);
    ClassDB::bind_method(D_METHOD("turn_ceiling_ground"), &EntityBody2D::turn_ceiling_ground);
    ClassDB::bind_method(D_METHOD("jump", "jumping_speed", "is_accumulating_mode"), &EntityBody2D::jump, false);
    ClassDB::bind_method(D_METHOD("correct_on_wall_corner", "steps"), &EntityBody2D::correct_on_wall_corner, 4);
    ClassDB::bind_method(D_METHOD("correct_onto_floor","steps"), &EntityBody2D::correct_onto_floor, 10);
    ClassDB::bind_method(D_METHOD("get_gravity_vector"), &EntityBody2D::get_gravity_vector);
    ClassDB::bind_method(D_METHOD("get_gravity_rotation_angle"), &EntityBody2D::get_gravity_rotation_angle);
    ClassDB::bind_method(D_METHOD("get_damp"), &EntityBody2D::get_damp);
    ClassDB::bind_method(D_METHOD("get_previous_velocity"), &EntityBody2D::get_previous_velocity);
    ClassDB::bind_method(D_METHOD("get_previous_global_velocity"), &EntityBody2D::get_previous_global_velocity);
    ClassDB::bind_method(D_METHOD("get_colliders"), &EntityBody2D::get_colliders);
    ClassDB::bind_method(D_METHOD("get_last_collider"), &EntityBody2D::get_last_collider);
    ClassDB::bind_method(D_METHOD("is_leaving_ground"), &EntityBody2D::is_leaving_ground);
    ClassDB::bind_method(D_METHOD("is_falling"), &EntityBody2D::is_falling);
}

EntityBody2D::EntityBody2D() 
{
    velocity = Vector2();
    threshold_speed_enabled = false;
    threshold_speed = 0.0;
    threshold_speed_correction_acceleration = 500.0;
    damp_enabled = false;
    damp_min_speed = 0.0;
    gravity_scale = 0.0;
    max_falling_speed = 1500.0;
    global_rotation_to_gravity_direction = true;
    up_direction_angle = 0.0;
}

EntityBody2D::~EntityBody2D() {}

Vector2 EntityBody2D::_get_gravity() const
{
    ProjectSettings *prjs = ProjectSettings::get_singleton();
    return Vector2(prjs->get_setting("physics/2d/default_gravity_vector", Vector2(0.0, 1.0))) * double(prjs->get_setting("physics/2d/default_gravity", 980.0)); // Gets gravity from project settings
}

bool EntityBody2D::move_and_slide(const bool use_real_velocity) 
{
    // Stores data in previous call of this method
    _velocity = velocity;
    _velocity_global = get_global_velocity();

    // Store some velocity-related values
    Vector2 grvec = get_gravity_vector(); // Gravity vector
    Vector2 grdir = grvec != Vector2() ? grvec.normalized() : Vector2(); // Gravity direction, a normalized Vector2
    
    // Rotation
    double gangl = get_gravity_rotation_angle(); // Gravity rotation angle
    if (global_rotation_to_gravity_direction && !UtilityFunctions::is_equal_approx(get_global_rotation(), gangl)) {
        set_global_rotation(UtilityFunctions::rotate_toward(get_global_rotation(), gangl, UtilityFunctions::deg_to_rad(900.0) * get_delta(this))); // Eases the global rotation to the gravity angle
    }
    set_up_direction(grdir != Vector2() ? -grdir.rotated(UtilityFunctions::deg_to_rad(up_direction_angle)) : get_up_direction());

    // Threshold speed
    if (threshold_speed_enabled && !_threshold_speed_affected) {
        double dir = UtilityFunctions::signf(velocity.x);
        velocity.x = UtilityFunctions::move_toward(velocity.x, dir * threshold_speed, threshold_speed_correction_acceleration * get_delta(this));
        set_velocity(velocity);
    }
    _threshold_speed_affected = false; // Restore threshold affection

    // Makes the body move
    bool result = CharacterBody2D::move_and_slide();
    Vector2 gv = get_global_velocity();
    if (use_real_velocity) { // Rigid mode
        gv = get_real_velocity();
    }
    set_global_velocity(gv);

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

    return result;
}

void EntityBody2D::calculate_gravity()
{
    if (!is_on_floor()) {
        Vector2 grvec = get_gravity_vector();
        Vector2 grdir = grvec.normalized(); // Gets gravity direction
        Vector2 result = get_global_velocity() + grvec * gravity_scale * get_delta(this); // Calculates global velocity affected by the gravity
        bool on_falling = result.dot(grdir) > 0.0;
        if (max_falling_speed > 0.0 && on_falling) {
            UtilityFunctions::print(result);
            result = Transform2DAlgo::get_projection_limit(result, grdir, max_falling_speed); // Limits the maximum of falling speed
        }
        set_global_velocity(result);
    } else {
        set_global_velocity(get_global_velocity() + get_gravity_vector().normalized()); // To ensure the body will correctly on the floor, and this will cause signal collided_on_floor emitted continuously on the floor
    }
}

void EntityBody2D::calculate_damp()
{
    if (!damp_enabled) {
        return;
    }
    
    double damp = get_damp() * get_delta(this);
    if (damp <= 0.0) {
        return;
    }
    
    double dir = UtilityFunctions::signf(velocity.x);
    velocity.x = UtilityFunctions::move_toward(velocity.x, damp_min_speed * dir, damp);
    set_velocity(velocity);
    _threshold_speed_affected = true;
}

void EntityBody2D::accelerate_local_x(const double acceleration, const double to) 
{
    velocity.x = UtilityFunctions::move_toward(velocity.x, to, acceleration * get_delta(this));
    set_velocity(velocity);
    _threshold_speed_affected = true;
}

void EntityBody2D::accelerate_local_y(const double acceleration, const double to) 
{
    velocity.y = UtilityFunctions::move_toward(velocity.y, to, acceleration * get_delta(this));
    set_velocity(velocity);
}

void EntityBody2D::accelerate_local(const double acceleration, const Vector2 &to) 
{
    set_velocity(velocity.move_toward(to, acceleration * get_delta(this)));
    _threshold_speed_affected = true;
}

void EntityBody2D::accelerate_x(const double acceleration, const double to) 
{
    Vector2 gv = get_global_velocity();
    gv.x = UtilityFunctions::move_toward(gv.x, to, acceleration * get_delta(this));
    set_global_velocity(gv);
    _threshold_speed_affected = true;
}

void EntityBody2D::accelerate_y(const double acceleration, const double to) 
{
    Vector2 gv = get_global_velocity();
    gv.y = UtilityFunctions::move_toward(gv.y, to, acceleration * get_delta(this));
    set_global_velocity(gv);
}

void EntityBody2D::accelerate(const double acceleration, const Vector2 &to) 
{
    set_global_velocity(get_global_velocity().move_toward(to, acceleration * get_delta(this)));
    _threshold_speed_affected = true;
}

void EntityBody2D::decelerate_with_friction(const double deceleration) 
{
    if (is_on_floor()) {
        Ref<KinematicCollision2D> kc = memnew(KinematicCollision2D); // Gets kinematic collision information
        test_move(get_global_transform(), get_gravity_vector().normalized(), kc); // Test move to fill the kc with collision information
        Node2D *collider = Object::cast_to<Node2D>(kc->get_collider()); // Gets collided body
        if (collider != nullptr) {
            RID collider_rid = kc->get_collider_rid(); // Stores the collider's rid for TileMap's get_layer_for_body_rid()
            union { // Uses union to save more memory
                TileMap *tilemap;
                StaticBody2D *block;
                RigidBody2D *rigid;
            } body;
            // For StaticBody2D and RigidBody2D and tiles of TileMap, uses their physics_material(_override).
            // StaticBody2D
            body.block = Object::cast_to<StaticBody2D>(collider);
            if (body.block != nullptr) {
                Ref<PhysicsMaterial> phymt = body.block->get_physics_material_override();
                if (phymt != nullptr && phymt->is_rough()) {
                    accelerate_local_x(deceleration * phymt->get_friction(), 0.0);
                    return;
                }
            }
            // RigidBody2D
            body.rigid = Object::cast_to<RigidBody2D>(collider);
            if (body.rigid != nullptr) {
                Ref<PhysicsMaterial> phymt = body.rigid->get_physics_material_override();
                if (phymt != nullptr && phymt->is_rough()) {
                    accelerate_local_x(deceleration * phymt->get_friction(), 0.0);
                    return;
                }
            }
            // TileMap
            body.tilemap = Object::cast_to<TileMap>(collider);
            if (body.tilemap != nullptr) {
                Ref<TileSet> tileset = body.tilemap->get_tileset();
                if (tileset != nullptr) {
                    int32_t layer = body.tilemap->get_layer_for_body_rid(collider_rid);
                    Ref<PhysicsMaterial> phymt = tileset->get_physics_layer_physics_material(layer);
                    if (phymt != nullptr && phymt->is_rough()) {
                        accelerate_local_x(deceleration * phymt->get_friction(), 0.0);
                        return;
                    }
                }
            }
            // For other bodies, uses "friction" metadata
            if (collider->has_meta("friction")) {
                double friction = double(collider->get_meta("friction"));
                if (friction != Math_NAN) {
                    accelerate_local_x(deceleration * UtilityFunctions::clampf(friction, 0.0, 1.0), 0.0);
                    return;
                }
            }
        }
    }
    accelerate_local_x(deceleration, 0); // Deceleration
    _threshold_speed_affected = true;
}

void EntityBody2D::use_friction(const double miu) 
{
    if (!is_on_floor()) {
        return;
    }
    Vector2 gv = get_global_velocity();
    Vector2 gn = gv.slide(get_floor_normal());
    gv = gn.lerp(gn *0.0, miu * get_delta(this));
    set_global_velocity(gv);
    _threshold_speed_affected = true;
}

void EntityBody2D::turn_wall() 
{
    if (_velocity_global != Vector2()) {
        set_global_velocity(_velocity_global.reflect(get_up_direction()));
    }
    else {
        set_global_velocity(get_global_velocity().reflect(get_up_direction()));
    }
    _threshold_speed_affected = true;
}

void EntityBody2D::turn_ceiling_ground() 
{
    if (_velocity_global != Vector2()) {
        set_global_velocity(_velocity_global.bounce(get_up_direction()));
    }
    else {
        set_global_velocity(get_global_velocity().bounce(get_up_direction()));
    }
}

void EntityBody2D::jump(const double jumping_speed, const bool is_accumulating_mode) 
{
    Vector2 gv = get_global_velocity();
    
    if (!is_accumulating_mode) {
        gv -= gv.project(get_gravity_vector().normalized()); // If not on accumulating mode, then zero the falling speed
    }
    gv += get_up_direction() * abs(jumping_speed);
    
    set_global_velocity(gv);
}

void EntityBody2D::correct_on_wall_corner(const int steps) 
{
    // Only on ceiling or falling speed < 0 can skip this detection
    if (!is_on_ceiling() || is_on_floor() || is_falling()) {
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
            _threshold_speed_affected = true;
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
            _threshold_speed_affected = true;
            set_global_position(p);
            return;
        }
    }
    set_global_position(p);
}

void EntityBody2D::correct_onto_floor(const int steps) 
{
    // Only on wall or falling speed > 0 can skip this detection
    if (is_on_floor() || !is_on_wall_only() || is_leaving_ground()) {
        return;
    }

    // Initialization for current position, detect-in length and pushing length
    Vector2 p_push = get_up_direction();
    Vector2 v = _velocity_global.project(p_push.orthogonal()) * get_delta(this);
    
    if (UtilityFunctions::is_zero_approx(v.length_squared())) {
        return;
    }

    Vector2 p = get_global_position();
    Vector2 p_cur = p;

    // Looping for detection and correction
    for (int i = 0; i < steps; i++) {
        p_cur += p_push;
        set_global_position(p_cur);

        if (!test_move(get_global_transform(), v.normalized())) {
            p = p_cur;
            set_global_velocity(_velocity_global);
            _threshold_speed_affected = true;
            break;
        }
    }

    // Finally, move the body back to original position, or to one after correction
    set_global_position(p);
}

Vector2 EntityBody2D::get_previous_velocity() const 
{
    return _velocity;
}

Vector2 EntityBody2D::get_previous_global_velocity() const 
{
    return _velocity_global;
}

Vector2 EntityBody2D::get_gravity_vector() const 
{
    PhysicsDirectBodyState2D *pdbs = PhysicsServer2D::get_singleton()->body_get_direct_state(get_rid());
    
    if (pdbs == nullptr) {
        return _get_gravity();
    }

    Vector2 gv = pdbs->get_total_gravity();
    return gv;
}

double EntityBody2D::get_gravity_rotation_angle() const 
{
    return get_gravity_vector().orthogonal().angle();
}

double EntityBody2D::get_damp() const 
{
    PhysicsDirectBodyState2D *pdbs = PhysicsServer2D::get_singleton()->body_get_direct_state(get_rid());
    
    if (pdbs == nullptr) {
        return 0.0;
    }

    return pdbs->get_total_linear_damp();
}

TypedArray<Node2D> EntityBody2D::get_colliders() 
{
    TypedArray<Node2D> objs = TypedArray<Node2D>::make();

    for(int i = 0; i < get_slide_collision_count(); i++) {
        Ref<KinematicCollision2D> kc = get_slide_collision(i);
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

Node2D* EntityBody2D::get_last_collider() 
{
    Ref<KinematicCollision2D> kc = get_last_slide_collision();
    if (!UtilityFunctions::is_instance_valid(kc)) {
        return nullptr;
    }
    return Object::cast_to<Node2D>(kc->get_collider());
}

bool EntityBody2D::is_leaving_ground() const 
{
    return get_velocity().dot(get_gravity_vector().normalized()) < 0.0;
}

bool EntityBody2D::is_falling() const 
{
    return get_velocity().dot(get_gravity_vector().normalized()) > 0.0;
}



void EntityBody2D::set_velocity(const Vector2 &p_velocity) 
{
    velocity = p_velocity;
    CharacterBody2D::set_velocity(velocity.rotated(get_up_direction().angle() + Math_PI/2.0));
}

Vector2 EntityBody2D::get_velocity() const 
{
    return velocity;
}

void EntityBody2D::set_global_velocity(const Vector2 &p_global_velocity) 
{
    CharacterBody2D::set_velocity(p_global_velocity);
    velocity = get_global_velocity().rotated(-get_up_direction().angle() - Math_PI/2.0);
}

Vector2 EntityBody2D::get_global_velocity() const 
{
    return CharacterBody2D::get_velocity();
}

void EntityBody2D::set_threshold_speed(const double p_threshold_speed)
{
    threshold_speed = p_threshold_speed;
}

double EntityBody2D::get_threshold_speed() const
{
    return threshold_speed;
}

void EntityBody2D::set_threshold_speed_enabled(const bool p_threshold_speed_enabled)
{
    threshold_speed_enabled = p_threshold_speed_enabled;
}

bool EntityBody2D::is_threshold_speed_enabled() const
{
    return threshold_speed_enabled;
}

void EntityBody2D::set_threshold_speed_correction_acceleration(const double p_threshold_speed_correction_acceleration)
{
    threshold_speed_correction_acceleration = p_threshold_speed_correction_acceleration;
}

double EntityBody2D::get_threshold_speed_correction_acceleration() const
{
    return threshold_speed_correction_acceleration;
}

void EntityBody2D::set_damp_enabled(const bool p_damp_enabled) 
{
    damp_enabled = p_damp_enabled;
}

bool EntityBody2D::is_damp_enabled() const 
{
    return damp_enabled;
}

void EntityBody2D::set_damp_min_speed(const double p_damp_min_speed) 
{
    damp_min_speed = p_damp_min_speed;
}

double EntityBody2D::get_damp_min_speed() const 
{
    return damp_min_speed;
}

void EntityBody2D::set_gravity_scale(const double p_gravity_scale) 
{
    gravity_scale = p_gravity_scale;
}

double EntityBody2D::get_gravity_scale() const 
{
    return gravity_scale;
}

void EntityBody2D::set_max_falling_speed(const double p_max_falling_speed) 
{
    max_falling_speed = p_max_falling_speed;
}

double EntityBody2D::get_max_falling_speed() const 
{
    return max_falling_speed;
}

void EntityBody2D::set_global_rotation_to_gravity_direction(const bool p_global_rotation_to_gravity_direction) 
{
    global_rotation_to_gravity_direction = p_global_rotation_to_gravity_direction;
}

bool EntityBody2D::is_global_rotation_to_gravity_direction() const 
{
    return global_rotation_to_gravity_direction;
}

void EntityBody2D::set_up_direction_angle(const double p_up_direction_angle) 
{
    up_direction_angle = p_up_direction_angle;
}

double EntityBody2D::get_up_direction_angle() const 
{
    return up_direction_angle;
}