[English](README.md) | **中文版（当前页面）**
# Godot 4 EntityBody2D
为2D平台游戏而开发的适用于Godot 4的 GDExtension，向引擎中自动附加`EntityBody2D`节点类

# 安装
1. 打开你的游戏工程文件所在目录（即project.godot文件所在目录）
2. 右键空白处，点击"Open Git Bash Here"
3. 从下列两种情况中，根据自己实际情况，选择一种安装方法并对应输入命令
## 非Git Repo工程
```
git clone <the_link_of_this_repo> ./gdextension/entity_body_2d
```
## Git Repo工程
```
git submodule add <the_link_of_this_repo> ./gdextension/entity_body_2d
```

# 使用说明
本插件提供`EntityBody2D`节点类，继承自`CharacterBody2D`节点类。`EntityBody2D`内部新增多种属性与方法以适应开发2D平台游戏的开发者们的需求。

`EntityBody2D`类自带重力系统，其中，`gravity`, `gravity_direction` and `max_falling_speed`这三个属性可以让开发者快速调节物体的重力属性。同时，在该类中也新增了一个`motion`属性，会自动计算经由`global_rotation`旋转后得到的`velocity`并赋值给`velocity`。为了能够让重力系统真正发挥作用，本人把`move_and_slide()`这一父类方法进行了重定义，同时向其中新增了两个属性：`is_gravity_direction_rotated`和`use_real_velocity`，这两个属性会影响到该物体最终的运动结果。当然，`EntityBody2D`中还有许多新加入的方法，如`accelerate_*()`、`jump()`、`use_friction()`等方法，能够向开发者提供更加直观快捷的接口。

## 重力系统
重力系统是本插件的重中之重。下列属性均与该类的重力系统相关

* `gravity`实际上为**重力加速度**，单位为***像素每二次方秒***，该值越大，则物体下落速度会变化得越快
* `gravity_direction`确定该物体的下落方向。**注意：**`move_and_slide()`的参数`is_gravity_direction_rotated`会覆盖影响该下落方向。
* `max_falling_speed`确定该物体在下落方向上的最大速度，该值为0时则无此限制。单位为**像素每秒**

记住这些属性及其说明，对开发者使用本插件制作物体将会更加得心应手

## 上方向与真实上方向
`EntityBody2D`中的`up_direction`在参与`move_and_slide()`方法的调用时，并非绝对意义上的上方向，在此情形下，此“上方向”为该物体在`global_rotation`为0时的上方向，如果需要受`global_rotation`影响的“上方向”（以下称之为**真实上方向**），请调用`get_real_up_direction()`方法。  
举个例子：若`up_direction`为`Vector2(0, -1)`，`global_rotation`为PI/4(45°)，则该物体的真实上方向为`Vector2(0, -1).rotated(PI/4) => Vector2(√2/2, -√2/2)`

## 相对速度
实际上，2D平台运动游戏的开发者会经常与速度属性`velocity`打交道，但随着时间的推移，越来越多的开发者希望能快速操作受`global_rotation`影响的速度，于是`motion`这个属性便呼之欲出。通过该属性，开发者无需计算实际速度，只需要输入该物体在相对正常情况下的速度就行了。其setter getter方法会自动计算、转换这两种速度。  
同样举个例子：若`motion`为`Vector2(10, 0)`，`global_rotation`为PI/4(45°)，则实际速度为`Vector2(10, 0).rotated(PI/4) => Vector2(5√2, 5√2)`

## `EntityBody2D`中重定义的`move_and_slide()`方法
该节点类的另一个核心点之一便是**重定义**的`move_and_slide()`方法，虽然方法名字与父类相同，但多了两个参数，开发者了解这些参数后能够对自己的游戏开发有所帮助。

**注:** 括号内的数值为对应参数的对应默认值
* `bool is_gravity_direction_rotated (true)`覆盖并影响该物体最终的下落效果，若为`true`，则最终下落方向为真实上方向的反方向。若真实上方向为`Vector2(1/2, -√3/2)`，则最终下落方向为`-Vector2(1/2, -√3/2) = Vector2(-1/2, √3/2)`
* `bool use_real_velocity (false)`决定该物体最终的运动效果，若为`true`，则该物体将会以更加真实的物理效果进行运动

### 下落速度公式
`下落速度 = 重力加速度 * 最终下落方向(重力方向亦或是真实上方向的反方向)`

### `max_falling_speed`在该方法中的运作机理
若`max_falling_speed`大于0，则该物体的速度在重力方向上的投影向量会受到限制，若超过这个限制，则该物体的最终速度为超过限制时的速度减去该投影向量超出限制的那部分向量。这样，就使得该物体在下落时看起来像是受到了最大限制（空气阻力）一样。

## 其他方法
不仅如此，该类还提供了其他物理方法供开发者使用：
* `accelerate_*()`有三个版本，分别是`accelerate_x()`、`accelerate_y()`和`accelerate()`，其共同参数`acceleration`的单位均为***pixels/s²***，其另一个共同参数为`to`，单位均为***pixels/s***。这三个方法能够让物体的x速度/y速度/`velocity`属性加速到一定数值，使之不会超过该数值的限制。如果想要做无上限加速，请参考下列代码：
```GDScript
velocity.x += acceleration * delta # 加速度为 float 类型
velocity.y += acceleration * delta # 加速度为 float 类型
velocity += acceleration * delta # 加速度为 Vector2 类型
```
* `jump()`方法会让该物体沿真实上方向进行跳跃，跳跃速度的单位为***pixels/s***，还有一个布尔值参数，若为`true`，则该物体不会先截断下落速度然后赋予跳跃速度，而是直接累加到当前速度上
* `use_friction()`该方法利用`lerp()`方法来让该物体在地面上运动时产生摩擦效果

# 安装需求
## 前置
需安装[Mathorm](https://github.com/Lazy-Rabbit-2001/Godot-4-Mathorm)前置GDExtension扩展

## 支持的Godot版本
4.2及以上

## 支持的开发系统及平台
Windows 10, 11 x86-64

# 源码
## 在哪里？
在`src.zip`文件夹里，需要解压使用