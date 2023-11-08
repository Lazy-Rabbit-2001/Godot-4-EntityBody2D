#pragma once

#include <godot_cpp/classes/area2d.hpp>

#include "entity_area_2d.h"

using namespace godot;

class EntityBody2D;
class EntityArea2D : public Area2D {
    GDCLASS(EntityArea2D, Area2D)

private:
    // Properties
    double max_falling_speed_ratio;

    void _on_entity_body_entered(Node2D *body);
    void _on_entity_body_exited(Node2D *body);
    

protected:
    static void _bind_methods();

public:
    // Constructor and Destructor
    EntityArea2D();
    ~EntityArea2D();

    // Built-in methods
    void _ready() override;

    void set_max_falling_speed_ratio(const double p_max_falling_speed_ratio);
    double get_max_falling_speed_ratio() const;
};
