**English(Current)** | [中文版](zh_cn.md)
# Godot 4 EntityBody2D
A GDExtension for Godot 4 to provide an EntityBody2D for 2D platform games

# How to Install?
1. Go into the directory of your project where a `project.godot` exists
2. Right click and run **"Open Git Bash Here"**
3. According to the situation listed below, choose the matching one method to finish the installation
## Project NOT AS a Git Repo
```
git clone <the_link_of_this_repo> ./gdextension/entity_body_2d
```
## Project AS a Git Repo
```
git submodule add <the_link_of_this_repo> ./gdextension/entity_body_2d
```

# How to Use?
This provides an extending class from `CharacterBody2D` named `EntityBody2D`, which offers extra properties and methods that developers can use when they are working on a 2D-platformer games.  

`EntityBody2D` brings built-in gravity system, with `gravity`, `gravity_direction` and `max_falling_speed` presented to make designers get fast modification on gravity settings of a body inheriting from this class. In addition, another property `motion` is also provided to be edited so that a developer can fast set its velocity rotated by `global_rotation` of the `EntityBody2D` instance. Of course, if you want to edit `velocity`, you can directly edit it through `EntityBody2D`'s inspector.  

To make the gravity system totally implemented, `move_and_slide()` method has been hidden and redefined with two new parameters joining in: `is_gravity_direction_rotated` and `use_real_velocity`, which controls the acutal action that the body will perform when the method is being called.

Also, there are a bunch of methods provided as well, such as `accelerate_*()`, `jump()` and `use_friction()`, helping developers to make their development faster and clearer

## Gravity
Gravity is the significance for this extension. Hereby a property list will be shown to explain how each property about gravity works:

* `gravity` -- actually it refers to **gravity acceleration**, in ***pixels/s²***. The greater the value is, the faster the body will fall
* `gravity_direction` -- defines in which direction the body will fall. However, a parameter `is_gravity_direction_rotated` in `move_and_slide()` have an overriding effect on the final result of real falling direction of the body
* `max_falling_speed` -- defines how great the falling speed is, and if the value is set to 0, there will be no limitation on the falling speed. The unit is ***pixels/s***

Remembering these properties and their usage may help developers better understand how the gravity will affect a body.

## Up Direction & Real Up Direction
In `EntityBody2D`, `up_direction` is not an absolute one if a body is moving via `move_and_slide()`. In such situation, `up_direction` is one when the body's `global_rotation` is 0, and you need to call `get_real_up_direction()` (hereinafter referred to as **"real up direction"**) to get one rotated by the `global_rotation`.  

E.g: If the `up_direction` is `Vector2(0, -1)` with `global_rotation` PI/4(45°), the `up_direction` will keeps still while the real up direction will become `Vector2(0, -1).rotated(PI/4) => Vector2(√2/2, -√2/2)`

## Motion
In actual projects, developers will frequently get access to `velocity`; however, they seem to be thirsty for a better way to get such one rotated by `global_rotation` of the body. This is why `motion` is born, which helps them to ignore how the global rotation will affect the final velocity, and they only to input the velocity without the rotation and the system will automatically operate it. 

Let's have a example: If you have set `motion` of the body to `Vector2(10, 0)` with `global_rotation` PI/4(45°), the final velocity will be `Vector2(10, 0).rotated(PI/4) => Vector2(5√2, 5√2)`

**Note:** This would make `velocity` in the inspector affected as well, and if you edit the `velocity` in the dock, then the `motion` will be given with modification as well.

## `move_and_slide()` in `EntityBody2D`
The core of the class is **redefined** `move_and_slide()` method, though its name maintains, and there are two new extra parameters that each developer needs to know to have a better understanding of its workflow.

**Note:** Values in "()" are default values of representative parameters
* `bool is_gravity_direction_rotated (true)` -- this will greatly affect the final movement of falling. If `true`, then it means the gravity will be the reverse of real up direction. For example, if you have your real up direction becoming `Vector2(1/2, -√3/2)`, the final falling direction will be `-Vector2(1/2, -√3/2) = Vector2(-1/2, √3/2)`
* `bool use_real_velocity (false)` -- this will determine the final performance of the body's movement. In case of being `true`, the body will act more like it does in realler physics environment.

### The formula of falling velocity
`falling velocity = gravity * falling direction(gravity_direction or it rotated by global_rotation)`

### `max_falling_speed` Working in the method
If the `max_falling_speed` is greater than 0, the body will fall along the falling direction until the project of the falling velocity onto the `gravity_direction` is longer than this value, the `velocity` will be subtracted from this outranged section to make the projecter limited, so everthing will be viewed as the body falls with some maximum.


## Other Methods
### Basic extra methods
Not only can a body run with new `move_and_slide()`, but they also have ability to be performed by other functions offered:

* `accelerate_*()` -- with three versions: `accelerate_x()`, `accelerate_y()` and `accelerate()`, whose first params are `acceleration` in ***pixels/s²***, while the second ones are `to` in ***pixels/s***. They will move the x_speed/y_speed/velocity to the value of param `to` and they won't make the final value out of limitation. So if you want to accelerate a body without continuously, please code：
```GDScript
velocity.x += acceleration * delta # acceleration is of float type
velocity.y += acceleration * delta # acceleration is of float type
velocity += acceleration * delta # acceleration is of Vector2 type
```
* `jump()` -- this will make te body jumps along the real up direction. This methods allows two params, between which the former one is jumping speed in ***pixels/s***, while the latter one is a boolean, and if `true`, the final velocity will be add by /real up direction * abs(jumping speed)/ rather than making falling speed 0 and then set the jumping speed
* `use_friction()` -- this method, taking use of `lerp()`, to make the body slow down as if it has friction against the floor

### Correction methods
During the development, the developers may find their character moving with some issues, especially when they are working on some games like Super Mario Bros., in which Mario can walk through one-tile gaps and they have no idea how to make a character walk over the gap when they are planning to make such a character; or such a situation where he, visually, jumps through by the side of a block, but due to the information difference led by his collision box, he acutally hits the block and starts to fall.  
Thus, two methods `correct_on_wall_corner()` and `correct_onto_floor()` are implemented. The first method will solve the issue of the second situation mentioned above, making the body jump by the side of one block smoothly; while the second method deals with the problem of the body not being able to pass the one-tile gap.

# Requirements
## Dependency
[Mathorm](https://github.com/Lazy-Rabbit-2001/Godot-4-Mathorm) is needed to run the extension

## Supported Godot Versions
4.2 or above

## Supported developing platforms
Windows 10, 11 x86-64

# Source Code
## How to Get it?
It's in `src.zip` in the main page, and you can unpack it to get the source code
