[English](README.md) | **中文版（当前页面）**
# Godot 4 EntityBody2D (v3.0版本)
为2D平台游戏而开发的适用于Godot 4的 GDExtension，向引擎中自动附加`EntityBody2D`节点类

# 安装
1. 将本工程克隆到任意文件夹下，复制其中的`gdextension`文件夹
2. 打开你的游戏工程文件所在目录（即project.godot文件所在目录）
3. 将已复制好的`gdextension`文件夹粘贴在该目录下

# 使用说明
本插件提供`EntityBody2D`节点类，继承自`CharacterBody2D`节点类。`EntityBody2D`内部新增多种属性与方法以适应开发2D平台游戏的开发者们的需求。

## 重力系统
重力系统是本插件的重中之重。下列属性均与该类的重力系统相关：

* `gravity_scale`为该物体受重力加速度的影响倍率，，该值越大，则物体下落速度变化得越快。
* `max_falling_speed`确定该物体在下落方向上的最大速度，该值为0时则无此限制。单位为**像素每秒**。

如需对物体应用其他方向的重力，请使用`Area2D`节点，将其`gravity_space_override`调整为非"Disabled"的值，然后调整与重力相关的属性值即可。当物体进入该节点范围内时即可生效。  

记住这些属性及其说明，对开发者了解本插件物体的重力机制会更有所益。

## 上方向与上方向偏角
`EntityBody2D`中的`up_direction`在参与`move_and_slide()`方法的调用时，并非绝对意义上的上方向，在此情形下，此“上方向”为该物体的***合重力方向*的反方向**，这样设计也是希望能让插件的一些行为能以合适的方式运作。

同时，为了能让开发者能够调整该属性，特此引入了`up_direction_angle`属性 **（单位：°）**，该属性会让`up_direction`旋转一定的角度。因此，上方向在调用`move_and_slide()`时实际上会变成：

> -gravity_direction.rotated(deg_to_rad(up_direction_angle))

```diff
! 开发者无需担心`up_direction`属性会累加旋转。在`move_and_slide()`函数执行完毕后，该属性就会转回原先的方向
```

## （相对）速度与全局速度
To simplify the operation of velocity in some situations, the velocity of `EntityBody2D` is separated into two variants — `velocity` and `global_velocity`. The `velocity` here is not equal to `CharacterBody2D.velocity` which is `global_velocity` in this extension; instead, it means such a "velocity" rotated by the angle of `up_direction`, or the angle of gravity direction.

## `EntityBody2D`中重定义的`move_and_slide()`方法
该节点类的另一个核心点便是**重定义后**的`move_and_slide()`方法，虽然方法名字与父类相同，但多了一个参数，开发者了解该参数后将能对自己的游戏开发有所帮助。

**注：** 括号内的数值为对应参数的默认值
* `bool use_real_velocity (false)`决定该物体最终的运动效果，若为`true`，则该物体将会以更加真实的物理效果进行运动

### `max_falling_speed`在该方法中的运作机理
若`max_falling_speed`大于0，则该物体的下落速度在超过该数值时会被限制在该数值以内，**默认为1500px/s²**。  

## 其他方法
### 其他基本方法
不仅如此，该类还提供了其他物理方法供开发者使用：
* `accelerate_*()`有七个版本，分别是`accelerate_speed()`, `accelerate_local_x()`, `accelerate_local_y()`, `accelerate_local()`, `accelerate_x()`, `accelerate_y()`和`accelerate()`，其共同参数`acceleration`的单位均为***pixels/s²***，其另一个共同参数为`to`，单位均为***pixels/s***。这三个方法能够让物体的`speed`/`velocity.x`/`velocity.y`/`velocity`/`global_velocity.x`/`global_velocity.y`/`global_velocity`属性加速到一定数值，使之不会超过该数值的限制。如果想要做无上限加速，请参考下列代码：
```GDScript
speed += acceleration * delta # 加速度为 float 类型

velocity.x += acceleration * delta # 加速度为 float 类型
velocity.y += acceleration * delta # 加速度为 float 类型
velocity += acceleration * delta # 加速度为 Vector2 类型

global_velocity.x += acceleration * delta # 加速度为 float 类型
global_velocity.y += acceleration * delta # 加速度为 float 类型
global_velocity += acceleration * delta # 加速度为 Vector2 类型
```
* `jump()`方法会让该物体`up_direction`进行跳跃，跳跃速度的单位为***pixels/s***，还有一个布尔值参数，若为`true`，则该物体不会先截断下落速度然后赋予跳跃速度，而是直接累加到当前速度上，适合用于增强跳跃高度。
* `use_friction()`方法利用`lerp()`方法来让该物体**在地面上运动时**产生摩擦效果。
* `decelerate_with_friction(float deceleration)`方法会根据传入的`deceleration`及所在地面的光滑程度来对该物体进行减速。如果地面为`TileMap`、`StaticBody2D`（`AnimatableBody2D`）和`RigidBody2D`，请为其提供 `PhysicsMaterial`资源，并开启`roughness`属性，将`friction`属性调为小于1.0的数值即可使地面光滑；若地面为`CharacterBody2D`，请新建一个元数据，命名为`friction`，将值设定为`float`类型，介于0.0~1.0之间即可。 **注意:** 由于该方法采用了`test_move()`，计算量较大，因此会比较吃性能，故在使用时需格外谨慎，控制好调用该方法的物体的数量，非必要不要调用该方法来进行减速操作。

### 校正方法
在开发过程中，开发者也会在开发可移动角色时遇到一些问题，尤其是对于制作类马里奥游戏的开发者来说更是如此。马里奥可以走过一格宽的空隙，但实际开发时却因为自己的角色无法通过这样的空隙对此头疼不已。而马里奥顶到东西时，由于使用了矩形判定箱，导致一部分视觉上会让马里奥在顶到方块侧边时会顺势滑过跳上去的情况，变成了马里奥被判定为顶头而下落，为此实装了两个方法：`correct_on_wall_corner()`和`correct_onto_floor()`。前者用来解决前面提到的“视觉欺骗”问题，而后者则用来让物体能够通过一格宽的缝隙。  

# 已知问题
## `velocity`的设置与获取
* 由于Godot自带的2D物理引擎GodotPhysics2D的精度尚且不足，当`up_direction`的角度不为90°的整数倍时，其物理行为就容易会出现问题。该问题虽已被修复，但修复方法的性能开销较大，请注意！

# 安装需求
## 支持的Godot版本
4.2及以上

## 开发源码前置
需要下载[Fast Syntax](https://github.com/Lazy-Rabbit-2001/GDExtension-Fast-Syntax/)和[Mathorm](https://github.com/Lazy-Rabbit-2001/Godot-4-Mathorm)这两个源码作为前置

## 支持的开发系统及平台
Windows 10, 11 x86-64
