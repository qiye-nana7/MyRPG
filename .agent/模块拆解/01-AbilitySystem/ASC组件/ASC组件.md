# ASC 组件（AbilitySystemComponent）

## 1. 结构（Structure）

**组成：**
- `UAbilitySystemComponent` - GAS 核心组件
- `IAbilitySystemInterface` - 获取 ASC 的接口
- `FGameplayAbilitySpec` - Ability 规格（实例化后的 Ability）
- `FActiveGameplayEffect` - 活跃的 GE 列表

**类关系：**
```
IAbilitySystemInterface
      │
      └── GetAbilitySystemComponent()
            │
            └── UAbilitySystemComponent
                  ├── ActivatableAbilities[]
                  ├── ActiveGameplayEffects[]
                  └── ActiveGameplayCues[]
```

## 2. 生命周期 / 完整流程（Lifecycle & Flow）

**创建时机：**
- **Player**：`PlayerState::PostInitializeComponents()`
- **AI/Enemy**：`Character::PostInitializeComponents()`

**初始化流程（关键）：**
```cpp
// Server: PossessedBy 时调用
void AAuraCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitAbilityActorInfo();  // 绑定 Owner 和 Avatar
}

// Client: PlayerState 复制到时调用
void AAuraCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitAbilityActorInfo();  // 客户端同样需要初始化
}

// 统一初始化
void AAuraCharacter::InitAbilityActorInfo()
{
    AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
    AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(
        AuraPlayerState,  // Owner: 数据的拥有者
        this              // Avatar: 表现的载体
    );
}
```

**更新驱动：**
- **Tick 驱动**：GE 的 Duration/Period 更新
- **事件驱动**：输入触发、碰撞触发、网络复制

**销毁时机：**
- **Player**：PlayerState 销毁时（跨关卡保留）
- **Enemy**：Character 销毁时

## 3. 职责边界（Responsibility）

| ✅ 应该做 | ❌ 不应该做 |
|----------|------------|
| 管理 GameplayAbility 的生命周期 | 处理具体技能逻辑（如"发射火球"） |
| 应用和同步 GameplayEffect | 直接修改 Attribute 数值 |
| 处理输入绑定（BindAbilityActivationToInputComponent） | 处理 UI 显示（应广播事件） |
| 管理 GameplayTags | 处理镜头控制（3C 职责） |

**边界原则：**
- ASC 是**容器和调度器**，不是**业务逻辑执行者**
- 具体技能逻辑在 `UGameplayAbility` 子类中实现

## 4. 关键原理（Principle）

**所有权分离（Owner vs Avatar）**
- **Owner**：数据的拥有者（PlayerState），决定数据存多久
- **Avatar**：表现的载体（Character），可以销毁/重生
- **意义**：玩家角色死亡重生时，技能冷却、Buff 等数据不丢失

**复制模式（Replication Mode）**
- `EGameplayAnimMode::PlayAutonomous` - 客户端预测播放
- `EGameplayEffectReplicationMode` - GE 复制策略
- `Mixed` 模式：GE 和 Cue 复制，Ability 激活服务器权威

**网络同步优化**
- 只同步变化的属性（Delta Compression）
- 预测（Prediction）减少延迟感

## 5. 使用模式（Industry Pattern）

**所有权放置策略：**
| 角色类型 | ASC 位置 | 原因 |
|---------|---------|------|
| Player | PlayerState | 跨关卡持久化 |
| AI Enemy | Character | 随怪物销毁 |
| Pet/Companion | 独立 Actor | 独立生命周期 |

**常见组合方式：**
```cpp
// 给角色添加 Ability
GiveAbility(FGameplayAbilitySpec(AbilityClass, Level));

// 绑定输入
BindAbilityActivationToInputComponent(
    InputComponent,
    ConfirmTargetData,  // 确认键
    CancelTargetData    // 取消键
);
```

**常见坑：**
- **忘记 InitAbilityActorInfo**：导致 ASC 不知道 Owner/Avatar，网络同步失败
- **Owner 和 Avatar 混淆**：PlayerState 和 Character 角色搞反
- **复制模式错误**：客户端看不到 GE 效果，或服务器不接收 Ability 输入
