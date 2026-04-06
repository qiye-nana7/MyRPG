# Chapter 4 - UI 系统 问答记录

## 视频要点
Chapter 4 part 2-8：建立 MVC 架构，WidgetController 监听属性变化，通过委托广播给 UI，包括初始值广播和持续监听。

---

### 问题 1：概念理解 - WidgetController 的作用是什么？为什么不能直接在 Widget 里访问 ASC？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- 解耦 UI 和游戏逻辑，UMG 不直接依赖 ASC
- 封装复杂的数据获取逻辑
- 支持不同 Widget 复用同一个 Controller
- 方便单元测试

---

### 问题 2：代码分析 - BroadcastInitialValues 和 BindCallbacksToDependencies 有什么区别？为什么要分别调用？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- BroadcastInitialValues：一次性同步当前值，解决启动时 UI 为空
- BindCallbacksToDependencies：注册回调监听后续变化
- 两者都要做：初始值 + 后续更新
- 如果只 Bind 不 Broadcast，UI 初始显示为 0 直到属性变化

---

### 问题 3：设计决策 - FWidgetControllerParams 结构体的作用是什么？为什么不直接在 WidgetController 构造函数里传参数？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- 打包参数便于传递（PC, PS, ASC, AS）
- UObject 构造函数限制，不能自定义参数
- 蓝图友好的参数传递方式
- 支持延迟初始化

---

### 问题 4：扩展场景 - 如果要在 HUD 上显示敌人的血条，你会怎么设计？需要新建哪些类？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- 创建 EnemyHealthWidgetController（复用或新建）
- 传入敌人的 ASC 而不是玩家的
- 敌人的 ASC 在 Character 上（不是 PlayerState）
- 考虑性能：只在视野内敌人才创建 Widget

---

## 章节回顾

### 本章节核心收获
（用户填写）

### 遗留疑问
（用户填写）
