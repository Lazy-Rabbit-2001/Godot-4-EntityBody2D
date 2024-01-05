**English(Current)** | [中文版](zh_cn.md)
# Godot 4 EntityBody2D (v3.0)
A GDExtension for Godot 4 to provide an `EntityBody2D` for 2D platform games

# How to Install?
1. Clone the repo to any folders and copy the `gdextension` folder
2. Go into the directory of your project where a `project.godot` exists
3. Paste the folder you just copied to the directory

# How to Use?
This provides an extending class from `CharacterBody2D` named `EntityBody2D`, which offers extra properties and methods that developers can use when they are working on a 2D-platformer games. 

## Gravity
Gravity is the significance for this extension. Hereby a property list will be shown to explain how each property about gravity works:

* `gravity_scale` -- adjusts the gravity of a body, and the greater the value is, the faster the body will fall.
* `max_falling_speed` -- defines how great the falling speed is, and if the value is set to 0, there will be no limitation on the falling speed. The unit is ***pixels/s***.

For gravity in other directions, please use an `Area2D` with its `gravity_space_override` adjusted to a value other than "Disabled", on the body, and then change the popping settings related to gravity. And it will take effect once a body enters it.

Remembering these properties and their usage may help developers understand better how the gravity affects a body.

## Up Direction and Up Direction Angle
In `EntityBody2D`, `up_direction` is not an absolute up direction if a body is moving via `move_and_slide()`. In such situation, `up_direction` is **the reverse of *total gravity direction***, which is set to make sure everything will work in expected way.

Also, to make users able to do some modification on this property, `up_direction_angle` **(in degrees)** is introduced to rotate the `up_direction` during the procession of `move_and_slide()`. So the up direction in `move_and_slide()` will be:

> -gravity_direction.rotated(deg_to_rad(up_direction_angle))

```diff
! Don't worry if the up direction will rotate based on one that rotated previously, after the call of the function, up direction will be rotated back to what it should be
```

## Velocity and Velocality
To simplify the operation of velocity in some situations, the velocity of `EntityBody2D` is separated into two variants — `velocity` and `velocality`, among which the latter, blended by "velocity" and "local", means such a `velocity` rotated by `up_direction.angle()`, or the angle of gravity direction, and can be edited in the inspector.

## Threshold Speed
The moment a body enters an `Area2D` whose gravity override is enabled with non-standard gravity direction, the body keeps its `velocity` the same as the one before it gets into the area. This means that when, for example, from `EntityBody2D`, a monster created by a developer is expected to keep walking at a constant horizontal speed, and he/she thinks it easy because they can directly set the `velocity.x` in the function `_ready()`, an astonishing phenomenon happens: The body entering that area doesn't walk at that speed expected! Imagine you are thinking of a solution to this problem, and you get an interesting idea: why not keep the `velocity.x` constant every frame? Thus, you start working on it but end up with solving the conflict between the constant speed and the call of `accelerate_*()`. This is a great problem because, if you keep that value invariable, all methods related to acceleration will not work. However, you need acceleration. Then you come into a dilemma...  
To have the most extent possible to solve this confusing situation, `threshold_speed` system is introduced. When the `velocity.x` doesn't match the threshold speed, it will be accelerated/decelerated to the value of `threshold_speed` by `threshold_speed_correction_acceleration`. When the value of `threshold_speed_enabled` is `false`, no threshold limitation is required. 

## `move_and_slide()` in `EntityBody2D`
The core of the class is **redefined** `move_and_slide()` method, though its name maintains, there is added a new extra parameter that each developer needs to know to have a better understanding of its workflow.

**Note:** The value in "()" is default values of representative parameters
* `bool use_real_velocity (false)` -- this will determine the final performance of the body's movement. In case of being `true`, the body will act more like it does in realler physics environment.

### Calculation of gravity and damp
Before v3.0, the gravity is calculated within `move_and_slide()`; since the version, the calculation of it has been extracted from the method and become an independent `calculate_gravity()`, which makes it more couplingless to code the movement of an instance of `EntityBody2D`.
Meanwhile, the simulation of being in dampful area is also what we are seeking for; thus, `calculate_damp()` may help with it when an `EntityBody2D` is in an `Area2D` whose `linear_damp` is enabled and greater than 0, which will affect the body's movement and makes it decelerate.

### `max_falling_speed` Working in the method
If the `max_falling_speed` is greater than 0, the falling speed will be limited within this maximum. **1500 px/s²** is default.

## Accelerational Methods
An `EntityBody2D` not only can run with new `move_and_slide()`, but also has ability to perform with other functions offered:

* `accelerate_*()` -- with seven versions: `accelerate_speed()`, `accelerate_local_x()`, `accelerate_local_y()`, `accelerate_local()`, `accelerate_x()`, `accelerate_y()` and `accelerate()`, whose first params are `acceleration` in ***pixels/s²***, while the second ones are `to` in ***pixels/s***. They will move the `speed`/`velocity.x`/`velocity.y`/`velocity`/`global_velocity.x`/`global_velocity.y`/`global_velocity` to the value of param `to` and they won't make the final value out of limitation. So if you want to accelerate a body continuously without any limitation, please code：
```GDScript
speed += acceleration * delta # acceleration is of float type

velocity.x += acceleration * delta # acceleration is of float type
velocity.y += acceleration * delta # acceleration is of float type
velocity += acceleration * delta # acceleration is of Vector2 type

global_velocity.x += acceleration * delta # acceleration is of float type
global_velocity.y += acceleration * delta # acceleration is of float type
global_velocity += acceleration * delta # acceleration is of Vector2 type
```
* `jump()` -- this will make a body jump along the `up_direction`. This method allows two params, between which the former one is jumping speed in ***pixels/s***, while the latter one is a boolean, and if `true`, the final velocity will be add by the jumping speed rather than making falling speed 0 and then set the jumping speed
* `use_friction()` -- this method, taking use of `lerp()`, makes the body slow down **when it is on the floor**, as if it has friction against the ground.
* `decelerate_with_friction(float deceleration)` -- this method will make the body decelerate by given `deceleration` according to the roughness of the ground it is standing on. For `TileMap`, `StaticBody2D`(`AnimatableBody2D`) and `RigidBody2D`, a `PhysicsMaterial` with `roughness` on and `friction` less than 1.0 to make the ground slippery; for `CharacterBody2D`, use metadata named `friction` with a value typed `float` within [0.0, 1.0]. **Note:** This method is costy because it is accomplished through `test_move()` which is calculation-consuming. So be careful of the amount of bodies calling this method and call this method only if they need it.
**Note:** To ensure the stability and accuracy of the result of `move_and_slide()`, please call them before `move_and_slide()` gets called.

## Correction methods
During the development, the developers may find their characters moving with some issues, especially when they are working on some games like Super Mario Bros., in which Mario can walk through one-tile gaps and they have no idea how to make a character walk over the gap when they are planning to make such a feature; or such a situation where he, visually, should be jumping by the side of a block, but due to the information difference led by his collision box, he acutally hits the block and starts to fall.  
Thus, two methods `correct_on_wall_corner()` and `correct_onto_floor()` are implemented. The first method will solve the issue of the second situation mentioned above, making the body jump by the side of one block smoothly; while the second method deals with the problem of the body not being able to pass the one-tile gap.


# Known Issues
## About setting and getting `velocality`
* Due to the midium accuracy of GodotPhysics2D, when the angle of `up_direction` cannot be divided completely by PI/2, the body will, in great chance, behave unexpectedly. This issue has got repaired in a tricky way that has increased the performance cost.

# Requirements
## Supported Godot Versions
4.2 or above

## For developing
[Fast Syntax](https://github.com/Lazy-Rabbit-2001/GDExtension-Fast-Syntax/) and [Mathorm](https://github.com/Lazy-Rabbit-2001/Godot-4-Mathorm) are required for developing from the source code

## Supported developing platforms
Windows 10, 11 x86-64
