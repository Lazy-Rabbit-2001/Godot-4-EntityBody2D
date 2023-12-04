[English](README.md) | **中文版（当前页面）**
# Godot 4 EntityBody2D (2.2版本)
为2D平台游戏而开发的适用于Godot 4的 GDExtension，向引擎中自动附加`EntityBody2D`节点类

# 安装
1. 将本工程克隆到任意文件夹下，复制其中的`gdextension`文件夹
2. 打开你的游戏工程文件所在目录（即project.godot文件所在目录）
3. 将已复制好的`gdextension`文件夹粘贴在该目录下

# 使用说明
本插件提供`EntityBody2D`节点类，继承自`CharacterBody2D`节点类。`EntityBody2D`内部新增多种属性与方法以适应开发2D平台游戏的开发者们的需求。

`EntityBody2D`类自带重力系统，其中，`gravity`, 和`max_falling_speed`这两个属性可以让开发者快速调节物体的重力属性。同时，`EntityBody2D`的`velocity`属性暴露在节点检查器中，方便开发者修改以设置该物体的初速度。  
对于多向重力游戏，本插件的`EntityBody2D`还可以与`Area2D`发生互动来实现更加逼真的重力效果，只需调整该`Area2D`的`gravity_space_override`属性及其相关属性即可。当然，本插件也实现了物体阻尼，只要物体进入了启用`linear_damp_space_override`的`Area2D`区域，该物体就会受到该区域的阻尼影响而减速。  
大部分情况下，如果只调整了`EntityBody2D`的`velocity`属性，则该物体在进入修改了重力环境的`Area2D`的过程中，其运动结果会出现问题，尤其是行走这一行为，同时还会导致`velocity`的精确度严重降低。为此，本插件引入了`autobody模式，专门为这类物体打造，该属性会强制固定该物体的`velocity`.x，使其行走不受更改重力环境的影响。为了能将 `velocity`.x限制在一定范围内，于是引入了`max_speed`属性，该属性会将`velocity`.x限制在[-max_speed, max_speed]这个区间内。若需要让`max_speed`起效，须开启`autobody`模式。  

从2.2版本起，开发者将无法直接修改重力加速度的数值，而是跟`RigidBody2D`一样需要通过访问`gravity_scale`来修改重力倍率。对于想要通过物体外节点(如`Area2D`)来快速影响物体物理属性的开发者，本插件新增了`max_speed_scale`和`max_falling_speed_scale`这两个属性以实现此效果。如果物体进入了`Area2D`，只需修改这两个属性即可，如果离开了该节点所管辖区域，就把这些数值直接调回1.0就行，是不是非常简单呐？  

为了能够让重力系统真正发挥作用，本人把`move_and_slide()`这一父类方法进行了重定义，同时向其中新增了个`use_real_velocity`参数，会影响到该物体最终的运动结果。  
当然，`EntityBody2D`中还有许多新加入的方法，如`accelerate_*()`、`jump()`、`use_friction()`等方法，能够向开发者提供更加直观快捷的接口。

## 重力系统
重力系统是本插件的重中之重。下列属性均与该类的重力系统相关：

* `gravity_scale`为该物体受重力加速度的影响倍率，，该值越大，则物体下落速度变化得越快。
* `max_falling_speed`确定该物体在下落方向上的最大速度，该值为0时则无此限制。单位为**像素每秒**。
* `max_falling_speed_scale`影响`max_falling_speed`的作用倍率。

如需对物体应用多向重力，请使用`Area2D`节点，并将其`gravity_space_override`调整为非"Disabled"的值，然后调整相关属性的数值即可。  

记住这些属性及其说明，对开发者使用本插件制作物体将会更加得心应手。

## 上方向与上方向偏角
`EntityBody2D`中的`up_direction`在参与`move_and_slide()`方法的调用时，并非绝对意义上的上方向，在此情形下，此“上方向”为该物体的***合重力方向*的反方向**，这样设计也是希望能让插件的一些行为能以合适的方式运作。

同时，为了能让开发者能够调整该属性，特此引入了`up_direction_angle`属性 **（单位：°）**，该属性会让`up_direction`旋转一定的角度。因此，上方向在调用`move_and_slide()`时实际上会变成：

> -gravity_direction.rotated(deg_to_rad(up_direction_angle))

```diff
! 开发者无需担心`up_direction`属性会累加旋转。在`move_and_slide()`函数执行完毕后，该属性就会转回原先的方向
```

从2.0版本起，重力方向由`Area2D`节点控制，见[重力系统](#重力系统)条目。  

## 速度与自动体模式
实际上，一些开发者希望在开发某些2D平台运动游戏时，能够直接在节点检视器里给一个`EntityBody2D`实例赋予初始速度，为此，`EntityBody2D`特地将`velocity`属性暴露在检视器内。然而该`velocity`是相对于物体的上方向全局角度，即`up_direction.angle()`而言的，如需使用父类`CharacterBody2D`的`velocity`（全局速度），请访问`global_velocity`属性。  

同样举个例子：若`velocity`为`Vector2(10, 0)`，上方向全局角`up_direction.angle()`为PI/4(45°)，则实际速度为`Vector2(10, 0).rotated(PI/4) => Vector2(5√2, 5√2)` 。

大部分情况下，诸如玩家、敌人这类物体，其运动会在切换重力场的过程中发生问题，尤其是其行走运动，仅修改`velocity`属性会导致其行走不正常。为解决该问题，强烈建议开启`autobody`属性来完成行走的属性设置，该属性通过强制设置行走速度来保证物体的行走行为平稳如履。同时，本插件还增加了`max_speed`属性，允许开发者限制该物体的`velocity`.x的值，在制作角色加速效果时极其有用。当然，同`max_falling_speed_scale`一样，本插件也为`max_speed`引入了`max_speed_scale`来设置`max_speed`的生效倍率。  
有时候，物体本应在进入水中的时候会走得很慢，出水之后其走路速度又会恢复原状，但`velocity`.x在进入水中之后就已经被削减过了，出了水还要把速度加速回原状，就比较麻烦。为了简化这种效果的实现，引入了`speed_is_max_speed`这一属性，开发者将其打开后，`velocity`.x就会自动设为`max_speed`，开发者此时只需要设置`max_speed`就可以顺带设置该物体`velocity`.x的初始值和实时值了
```diff
! 使用`max_speed`与`speed_is_max_speed`这两个属性前需先开启`autobody`属性！
! 只有当`max_speed` * `max_speed_scale` 大于0时才能限制物体的velocity.x！
```

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
* 由于Godot自带的2D物理引擎GodotPhysics2D的精度尚且不足，当`up_direction`的角度不为90°的整数倍时，其物理行为就容易会出现问题。该问题虽已被修复，但修复方法的性能开销较大，若对此有所需求，可考虑使用[Mikael Hermansson的Jolt物理引擎插件](https://github.com/godot-jolt/godot-jolt)来缓解该问题。

# 安装需求
## 支持的Godot版本
4.2及以上

## 开发源码前置
需要下载[Fast Syntax](https://github.com/Lazy-Rabbit-2001/GDExtension-Fast-Syntax/)和[Mathorm](https://github.com/Lazy-Rabbit-2001/Godot-4-Mathorm)这两个源码作为前置

## 支持的开发系统及平台
Windows 10, 11 x86-64
