# MVC 架构（UI 与 GAS 整合）

## 1. 结构（Structure）

**MVC 分层：**
```
Model (数据层)
├── AttributeSet (Health, Mana)
├── ASC (Ability 状态)
└── GameState (全局游戏状态)

    ↑ 监听变化

Controller (控制层)
├── UAuraWidgetController (基类)
│   ├── PlayerController
│   ├── PlayerState
│   ├── ASC
│   └── AttributeSet
│
└── UOverlayWidgetController (主界面控制器)
    ├── BroadcastInitialValues()
    └── BindCallbacksToDependencies()

    ↓ 广播事件

View (视图层)
├── AAuraHUD
├── UAuraUserWidget (基类)
└── UMG Widgets (进度条、按钮等)
```

**关键数据结构：**
```cpp
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
    TObjectPtr<APlayerController> PlayerController;
    TObjectPtr<APlayerState> PlayerState;
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
    TObjectPtr<UAttributeSet> AttributeSet;
};
```

## 2. 生命周期 / 完整流程（Lifecycle & Flow）

**初始化链：**
```
Character::InitAbilityActorInfo()
      │
      ▼
HUD::InitOverlay(PC, PS, ASC, AS)
      │
      ├──> 创建 WidgetController
      │       └── SetWidgetControllerParams()
      │
      ├──> 创建 Widget
      │       └── AddToViewport()
      │
      ├──> BroadcastInitialValues()  [关键：初始值]
      │
      └──> BindCallbacksToDependencies()  [关键：持续监听]
```

**属性变化响应链：**
```
AttributeSet::Health 变化
      │
      ▼
ASC 广播 OnHealthAttributeChanged
      │
      ▼
OverlayWidgetController::HealthChanged()
      │
      ▼
OnHealthChanged.Broadcast(NewValue)
      │
      ▼
UMG Widget 绑定到委托
      │
      ▼
更新 ProgressBar 值
```

**关键代码：**
```cpp
// 1. 初始值广播（避免启动时 UI 为空）
void UOverlayWidgetController::BroadcastInitialValues()
{
    const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);
    OnHealthChanged.Broadcast(AS->GetHealth());
    OnManaChanged.Broadcast(AS->GetMana());
}

// 2. 持续监听（属性变化时更新 UI）
void UOverlayWidgetController::BindCallbacksToDependencies()
{
    const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        AS->GetHealthAttribute()
    ).AddLambda([this](const FOnAttributeChangeData& Data) {
        OnHealthChanged.Broadcast(Data.NewValue);
    });
}
```

## 3. 职责边界（Responsibility）

| 层级 | ✅ 应该做 | ❌ 不应该做 |
|------|----------|------------|
| **Model** | 存储数据、属性复制 | 知道 UI 存在 |
| **Controller** | 监听数据变化、广播事件 | 直接修改 Model（只读） |
| **View** | 订阅事件、更新显示 | 修改游戏逻辑（只读） |

**单向数据流：**
```
Model (AttributeSet)
      ↑ 只监听，不修改
Controller (WidgetController)
      ↑ 只订阅，不修改
View (UMG Widget)
```

**边界检查：**
- ❌ Widget 直接调用 `SetHealth()`（破坏单向流）
- ❌ WidgetController 修改 Attribute（应通过 ASC）
- ✅ Widget 响应玩家输入 → 调用 ASC::TryActivateAbility()

## 4. 关键原理（Principle）

**为什么需要 WidgetController？**

1. **解耦 UI 和游戏逻辑**
   - UMG 不直接依赖 ASC/AttributeSet
   - 通过 Controller 中转，降低耦合

2. **统一数据访问**
   - 复杂的获取逻辑封装在 Controller
   - Widget 只关心"显示什么"

3. **支持不同显示方式**
   - 同样的 Health 数据
   - 可以是血球、血条、数字
   - 各自 Widget 订阅同一个 Controller 事件

**BroadcastInitialValues 的必要性**
```cpp
// 问题：如果不广播初始值
// 1. 游戏启动
// 2. Widget 创建
// 3. AttributeSet 已经有 Health=100
// 4. 但 Widget 显示 0（默认值）
// 5. 直到 Health 变化才更新

// 解决：初始化时主动广播一次当前值
BroadcastInitialValues();  // 同步初始状态
BindCallbacksToDependencies();  // 监听后续变化
```

**委托类型的选择**
```cpp
// 动态多播委托（蓝图可绑定）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnHealthChangedSignature, 
    float, 
    NewHealth
);

// UPROPERTY(BlueprintAssignable) 让蓝图可以订阅
UPROPERTY(BlueprintAssignable)
FOnHealthChangedSignature OnHealthChanged;
```

## 5. 使用模式（Industry Pattern）

**常见 UI 架构对比：**

| 模式 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| **MVC** | 结构清晰，职责明确 | 类多，代码量稍大 | 复杂 RPG |
| **直接绑定** | 简单快速 | 耦合严重 | 原型开发 |
| **MVVM** | 数据驱动，自动同步 | 学习成本高 | 现代 UE5 推荐 |

**UE5 MVVM 替代方案：**
```cpp
// UE 5.2+ 提供 MVVM 插件
UCLASS()
class UHealthViewModel : public UMVVMViewModelBase
{
    UPROPERTY(FieldNotify)
    float Health;
};

// Widget 直接绑定到 ViewModel
// 数据变化自动同步，无需手动委托
```

**常见坑：**
- **Widget 创建时 Controller 为 null**：初始化顺序错误
- **属性变化 UI 不更新**：忘记 BindCallbacksToDependencies
- **初始值为 0**：忘记 BroadcastInitialValues
- **内存泄漏**：Widget 销毁时没解绑委托（用 AddWeakLambda 或 UPROPERTY）

**面试常问：**
> "为什么要分 WidgetController 和 Widget？"
> 
> 答：解耦 - Controller 处理数据监听和转换，Widget 只负责显示；Controller 可以复用给不同 Widget（血条/血球）；方便单元测试。

> "BroadcastInitialValues 和 BindCallbacks 有什么区别？"
> 
> 答：Broadcast 是一次性同步当前值，解决启动时 UI 为空；Bind 是注册回调监听后续变化。两者都要做。
