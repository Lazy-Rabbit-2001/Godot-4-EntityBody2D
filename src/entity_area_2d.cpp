#include "entity_body_2d.h"
#include "entity_area_2d.h"

#include <godot_cpp/classes/engine.hpp>

void EntityArea2D::_bind_methods()
{
    // double max_falling_speed_ratio
    ADD_GROUP("Gravity", "");
    ADD_SUBGROUP("For Entity Body", "");
    ClassDB::bind_method(D_METHOD("get_max_falling_speed_ratio"), &EntityArea2D::get_max_falling_speed_ratio);
    ClassDB::bind_method(D_METHOD("set_max_falling_speed_ratio", "p_max_falling_speed_ratio"), &EntityArea2D::set_max_falling_speed_ratio);
    ClassDB::add_property(
        "EntityArea2D", PropertyInfo(Variant::FLOAT, "max_falling_speed_ratio", PROPERTY_HINT_RANGE, "0, 1, 0.001, or_greater, hide_slider, suffix:x"),
        "set_max_falling_speed_ratio", "get_max_falling_speed_ratio"
    );
}


EntityArea2D::EntityArea2D() {
    max_falling_speed_ratio = 1.0;
}

EntityArea2D::~EntityArea2D() {}


// Built-in
void EntityArea2D::_ready() {
    connect("body_entered", create_custom_callable_function_pointer<EntityArea2D, Node2D*>(this, &EntityArea2D::_on_entity_body_entered));
    connect("body_exited", create_custom_callable_function_pointer<EntityArea2D, Node2D*>(this, &EntityArea2D::_on_entity_body_exited));
}


// Private Methods
void EntityArea2D::_on_entity_body_entered(Node2D *body) {
    EntityBody2D *eb2d = Object::cast_to<EntityBody2D>(body);
    if (eb2d == nullptr) {
        return;
    }
    eb2d->add_entity_area_to_list(this);
}

void EntityArea2D::_on_entity_body_exited(Node2D *body) {
    EntityBody2D *eb2d = Object::cast_to<EntityBody2D>(body);
    if (eb2d == nullptr) {
        return;
    }
    eb2d->remove_entity_area_from_list(this);
}


// Setters & Getters
void EntityArea2D::set_max_falling_speed_ratio(const double p_max_falling_speed_ratio) {
    max_falling_speed_ratio = p_max_falling_speed_ratio;
}

double EntityArea2D::get_max_falling_speed_ratio() const {
    return max_falling_speed_ratio;
}
