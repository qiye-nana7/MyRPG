ru# GAS 核心接口速查

## 1. ASC (AbilitySystemComponent)

### 初始化
```cpp
// 设置 Owner 和 Avatar（必须在使用 ASC 前调用）
void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);

// 获取接口（实现 IAbilitySystemInterface 的类）
UAbilitySystemComponent* GetAbilitySystemComponent(const AActor* Actor);
```

### GameplayEffect 操作
```cpp
// 核心：应用 GE（四行代码的第四行）
FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(
    const FGameplayEffectSpec& Spec
);

// 移除 GE（用于 Duration/Infinite 类型）
void RemoveActiveGameplayEffect(FActiveGameplayEffectHandle Handle);

// 创建 Spec（四行代码的第三行）
FGameplayEffectSpecHandle MakeOutgoingSpec(
    TSubclassOf<UGameplayEffect> GameplayEffectClass,
    float Level,
    FGameplayEffectContextHandle Context
);
```

### 属性监听
```cpp
// 绑定属性变化回调（UI 用）
FOnGameplayAttributeValueChange& GetGameplayAttributeValueChangeDelegate(
    FGameplayAttribute Attribute
);

// 获取属性当前值
float GetNumericAttribute(const FGameplayAttribute& Attribute) const;
```

### Ability 操作（预告）
```cpp
// 给予 Ability
FGameplayAbilitySpecHandle GiveAbility(const FGameplayAbilitySpec& Spec);

// 激活 Ability
bool TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate);

// 输入绑定
void BindAbilityActivationToInputComponent(
    UInputComponent* InputComponent,
    FGameplayAbilityInputBinds BindInfo
);
```

---

## 2. AttributeSet

### 定义属性（宏生成）
```cpp
// 头文件
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
FGameplayAttributeData Health;
ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health)

// 生成的方法：
// float GetHealth() const
// void SetHealth(float NewVal)
// void InitHealth(float NewVal)
// static FGameplayAttribute GetHealthAttribute()
```

### 生命周期回调
```cpp
// 修改前（可修改请求值）
virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue);

// 修改后
virtual void PostAttributeChange(
    const FGameplayAttribute& Attribute,
    float OldValue,
    float NewValue
);

// 复制回调
UFUNCTION()
void OnRep_Health(const FGameplayAttributeData& OldHealth);
```

### 网络同步
```cpp
// 在 GetLifetimeReplicatedProps 中注册
DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
```

---

## 3. GameplayEffect

### 蓝图配置（Data Asset）
```cpp
UCLASS()
class UGameplayEffect : public UObject
{
    // 持续时间类型
    EGameplayEffectDurationType DurationType;
    // Instant - 立即
    // Duration - 持续（有期限）
    // Infinite - 无限
    // Periodic - 周期（Dot/Hot）
    
    // 属性修改器数组
    TArray<FGameplayModifierInfo> Modifiers;
    
    // 赋予的标签（施加 GE 时给目标）
    FGameplayTagContainer GrantedTags;
    
    // 所需标签（目标必须拥有才能施加）
    FGameplayTagContainer RequiredTags;
};
```

### Modifier 配置
```cpp
struct FGameplayModifierInfo
{
    FGameplayAttribute Attribute;           // 修改哪个属性
    TEnumAsByte<EGameplayModOp::Type> ModifierOp;  // 运算类型
    // Add - 加法
    // Multiply - 乘法
    // Divide - 除法
    // Override - 覆盖
    
    FScalableFloat Magnitude;               // 数值（可配置曲线）
};
```

### 运行时结构
```cpp
// 效果规格（运行时创建）
struct FGameplayEffectSpec
{
    UGameplayEffect* Def;                   // GE 定义
    float Level;                            // 等级
    FGameplayEffectContextHandle Context;   // 上下文（来源信息）
    TArray<FModifierSpec> Modifiers;        // 计算后的修改器
};

// 活跃的 GE（应用后返回）
struct FActiveGameplayEffect
{
    FActiveGameplayEffectHandle Handle;     // 唯一标识（用于移除）
    FGameplayEffectSpec Spec;               // 规格副本
    float StartWorldTime;                   // 开始时间
};
```

### EffectContext（来源信息）
```cpp
FGameplayEffectContextHandle MakeEffectContext();
void AddSourceObject(UObject* SourceObject);

// 可获取的信息：
UObject* GetSourceObject();           // 来源对象（如血瓶 Actor）
AActor* GetInstigator();              // 发起者（Controller）
AActor* GetEffectCauser();            // 效果造成者（Actor）
```

---

## 4. 快速对照表

| 需求 | 使用的类/方法 |
|------|--------------|
| 修改角色血量 | ASC::ApplyGameplayEffectSpecToSelf |
| 监听血量变化 | ASC::GetGameplayAttributeValueChangeDelegate |
| 定义新属性 | AttributeSet + ATTRIBUTE_ACCESSORS |
| 创建治疗药水 | AAuraEffectActor + Instant GE |
| 创建持续 Buff | Duration GE + Remove on EndOverlap |
| 知道谁打的我 | EffectContext::GetSourceObject |
| 属性网络同步 | RepNotify + DOREPLIFETIME |

---

## 5. 常见组合模式

### 治疗药水（Instant）
```cpp
// GE 配置：DurationType = Instant
// Modifier：Health +25
ApplyGameplayEffectSpecToSelf(Spec);  // 立即生效，自动销毁
```

### 持续回血（Duration）
```cpp
// GE 配置：DurationType = Duration, Duration = 10s
// Modifier：Health +5 per second (Periodic)
FActiveGameplayEffectHandle Handle = ApplyGameplayEffectSpecToSelf(Spec);
// ... 10秒后自动移除，或手动：
RemoveActiveGameplayEffect(Handle);
```

### 属性 Buff（Infinite）
```cpp
// GE 配置：DurationType = Infinite
// Modifier：MaxHealth * 1.2
// 需要手动移除（如卸下装备时）
```

---

**建议：先看此文档建立全局地图，再看视频实践，最后问答检验。**
