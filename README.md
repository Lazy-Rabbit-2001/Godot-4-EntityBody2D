**English(Current)** | [中文版](zh_cn.md)
# Godot 4 EntityBody2D (v2.0)
A GDExtension for Godot 4 to provide an EntityBody2D for 2D platform games

# How to Install?
1. Clone the repo to any folders and copy the `gdextension` folder
2. Go into the directory of your project where a `project.godot` exists
3. Paste the folder you just copied to the directory

# How to Use?
This provides an extending class from `CharacterBody2D` named `EntityBody2D`, which offers extra properties and methods that developers can use when they are working on a 2D-platformer games.  

`EntityBody2D` brings built-in gravity system, with `gravity` and `max_falling_speed` presented to make designers get fast modification on gravity settings of a body inheriting from this class. `velocity` of `EntityBody2D` is exposed in the inspector, so every developer can modify it directly to assign the body's initial velocity.  
For multi-gravity games, the instances of `EntityBody2D` are allowed to behave more physically and able to interact with `Area2D`s with `gravity_space_override` mode on and related properties adjusted. In addition, damp is also effective to `EntityBody2D`'s instances whose `damp_mode` is on when they enters an `Area2D` with `linear_damp_space_override`.  
In most cases, a body with `velocity` changed will perform weirdly during the progress of entering a gravity-space-overridden `Area2D`, which also changes the velocity of the body to an unexpected and unsatisfying value. To make the body act, especially walking on the "ground", as they should be expected to do, `speed` is introduced to solve this problem. This property forces the body to walk with the speed of its value so that the body will not move awkward.  

To make the gravity system totally implemented, `move_and_slide()` method has been hidden and redefined, with a new parameter `use_real_velocity`, which controls the acutal action that the body will perform when the method is being called.

Also, there are a bunch of methods provided as well, such as `accelerate_*()`, `jump()` and `use_friction()`, helping developers to make their development faster and clearer

## Gravity
Gravity is the significance for this extension. Hereby a property list will be shown to explain how each property about gravity works:

* `gravity` -- actually it refers to **gravity acceleration**, in ***pixels/s²***. The greater the value is, the faster the body will fall
* `max_falling_speed` -- defines how great the falling speed is, and if the value is set to 0, there will be no limitation on the falling speed. The unit is ***pixels/s***

For gravity in other directions, please use an `Area2D` and adjust its `gravity_space_override` to a value rather than "Disabled", and then change the settings popping right after the mode gets changed.
**(WARNING: If `autobody` mode is on while `global_rotation_to_gravity_direction` is off, the gravity in other direction will work abnormally!)**

Remembering these properties and their usage may help developers better understand how the gravity will affect a body.

## Up Direction and Top Direction
In `EntityBody2D`, `up_direction` is not an absolute up direction if a body is moving via `move_and_slide()`. In such situation, `up_direction` is one affected by the body's `global_rotation`, and you need to get access to `top_direction` to get an up direction without any rotation.  

E.g: If the `top_direction` is `Vector2(0, -1)` with `global_rotation` PI/4(45°), the `up_direction` will become `Vector2(0, -1).rotated(PI/4) => Vector2(√2/2, -√2/2)`

Before v1.6, there is a property `gravity_direction` that helps developers to decide to which direction the gravity effects the body. However, for more convenience, the property has been discarded and its functionality is now replaced with **`up_direction` in reverse** to be the gravity direction, which can be got via `get_gravity_direction()` method.
From v2.0, gravity direction is controlled by an `Area2D`, and please see [Gravity](#gravity).

## Velocity and Speed
In actual projects, the developers hope to edit the velocity in the inspector of a body so that they can assign the initial velocity of the body. Therefore, `velocity` is exposed for this purpose. However, this `velocity` is one relative to the body's `global_rotation`, and if you want to get access to the `velocity` in `CharacterBody2D`(global velocity), please use `global_velocity` instead of `velocity`.

Let's have an example: If you have set `velocity` of a body to `Vector2(10, 0)` with `global_rotation` PI/4(45°), the final velocity will be `Vector2(10, 0).rotated(PI/4) => Vector2(5√2, 5√2)`  

In most situations, some objects, like players and enemies, will behave weirdly during the change of gravity field, especially when they are walking. If there is only velocity modified, they will walk unstably with incorrect velocity being set. To solve this problem, it is recommended and required to use `speed` for this situation to take the place of `velocity` setting. This property will force the velocity to fit for walking to make the behavior more smooth and stable. **(The property`autobody` should be turned on first!)**  

## `move_and_slide()` in `EntityBody2D`
The core of the class is **redefined** `move_and_slide()` method, though its name maintains, there is added a new extra parameters that each developer needs to know to have a better understanding of its workflow.

**Note:** Values in "()" are default values of representative parameters
* `bool use_real_velocity (false)` -- this will determine the final performance of the body's movement. In case of being `true`, the body will act more like it does in realler physics environment.

### `max_falling_speed` Working in the method
If the `max_falling_speed` is greater than 0, the falling speed will be limited within this maximum. **1500 px/s²** is default.

## Other Methods
### Basic extra methods
An `EntityBody2D` not only can run with new `move_and_slide()`, but also has ability to be performed by other functions offered:

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

### Correction methods
During the development, the developers may find their characters moving with some issues, especially when they are working on some games like Super Mario Bros., in which Mario can walk through one-tile gaps and they have no idea how to make a character walk over the gap when they are planning to make such a feature; or such a situation where he, visually, should be jumping by the side of a block, but due to the information difference led by his collision box, he acutally hits the block and starts to fall.  
Thus, two methods `correct_on_wall_corner()` and `correct_onto_floor()` are implemented. The first method will solve the issue of the second situation mentioned above, making the body jump by the side of one block smoothly; while the second method deals with the problem of the body not being able to pass the one-tile gap.

# Known Issues
* Due to the midium accuracy of GodotPhysics2D, when the angle of `up_direction` cannot be divided completely by PI/2, the body will, in great chance, behave unexpectedly. This issue has got repaired in a tricky way that has increased the performance cost, which could get optimizd when [Jolt by Mikael Hermansson](https://github.com/godot-jolt/godot-jolt) is installed and enabled.

# Requirements
## Supported Godot Versions
4.2 or above

## For developing
[Fast Syntax](https://github.com/Lazy-Rabbit-2001/GDExtension-Fast-Syntax/) and [Mathorm](https://github.com/Lazy-Rabbit-2001/Godot-4-Mathorm) are required for developing from the source code

## Supported developing platforms
Windows 10, 11 x86-64
