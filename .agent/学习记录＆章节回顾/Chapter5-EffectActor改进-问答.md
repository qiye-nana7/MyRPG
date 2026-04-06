# Chapter 5 - EffectActor 改进 问答记录

## 视频要点
Chapter 5 part 1-2：将 EffectActor 从直接修改属性改为通过 GameplayEffect 接口，表现层移至蓝图，逻辑层使用标准四行代码。

---

### 问题 1：概念理解 - GameplayEffect 应用的四行代码分别是什么作用？为什么必须按这个顺序？

**你的回答：**
创建GE实例的上下文实例，一一对应，包含除了技能本身属性之外的信息，比如伤害造成者；绑定伤害造成者实例对象到上下文；创建GE实例，由目标ASC对象调用，并绑定上下文，level暂不清楚意义；最后调用的属性应用，也由目标对象调用

**判断：** ⭐⭐⭐ 基本正确，Level概念缺失

**关键点：**
1. `MakeEffectContext()` - 创建上下文，存储元数据
2. `AddSourceObject(this)` - 标记来源对象（谁施加的）
3. `MakeOutgoingSpec(GameplayEffectClass, Level, Context)` - 打包 GE 类、等级、上下文成规格
   - **Level**：GE 的等级，用于数值曲线。比如治疗药水等级1回25血，等级2回50血，通过 `FScalableFloat` 查表计算
4. `ApplyGameplayEffectSpecToSelf()` - 执行效果修改属性
- **顺序不可变**：必须先有 Context 才能 AddSourceObject，必须先有 Spec 才能 Apply

---

### 问题 2：代码分析 - 如果去掉 `AddSourceObject(this)` 会发生什么？什么情况下会用到 SourceObject？

**你的回答：**
找不到伤害来源，可能有一些功能无法实现如反弹技能

**判断：** ⭐⭐⭐⭐ 正确，例子恰当

**关键点：**
- 失去来源追踪，无法知道是谁施加的效果
- **使用场景**：
  - 伤害统计面板（谁打了多少输出）
  - 击杀归属（谁拿的击杀）
  - 效果溯源（这个 Buff 是哪件装备给的）
  - **反弹伤害**（你提到的例子）
- EffectContext 在 GE 整个生命周期都可用

---

### 问题 3：设计决策 - 为什么要将表现层（网格/碰撞）移到蓝图，而保留逻辑层在 C++？

**你的回答：**
方便策划和美术快速调参实现功能微调

**判断：** ⭐⭐⭐ 正确，但角度单一

**关键点：**
- **职责分离**（架构角度）：程序专注逻辑，美术策划调整表现
- **编译效率**（工程角度）：改蓝图不触发 C++ 重编译
- **复用性**（设计角度）：同一个 C++ 类配不同蓝图 = 血瓶/蓝瓶/炸弹
- **灵活性**（团队角度）：蓝图更适合快速迭代视觉表现

---

### 问题 4：扩展场景 - 如果要让 EffectActor 提供持续回血（站在区域内持续恢复），需要修改哪些代码？

**你的回答：**
更改GESpec的伤害类型

**判断：** ⭐⭐ 理解有偏差

**正确答案：**
不是改"伤害类型"，而是改 **DurationType**：

```cpp
// 方案 A：Duration（持续一段时间内回血）
GE.DurationType = EGameplayEffectDurationType::Duration;
GE.Duration = 10.0f;  // 持续10秒

// 方案 B：Periodic（周期性回血，类似Hot）
GE.DurationType = EGameplayEffectDurationType::Duration;  // 或 Infinite
GE.Period = 1.0f;  // 每1秒触发一次Modifier
```

**代码修改：**
```cpp
// 类成员保存Handle
UPROPERTY()
FActiveGameplayEffectHandle ActiveHandle;

// OnOverlapBegin：应用 GE，保存 Handle
ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

// OnOverlapEnd：移除 GE
ASC->RemoveActiveGameplayEffect(ActiveHandle);
```

---

## 面试故事包装

**EffectActor 重构（项目难点）：**

> "我最初直接用 const_cast 修改属性，虽然能跑但很快发现三个问题：
> 1. **没法追踪来源** - 不知道谁施加的效果，做不出伤害统计
> 2. **没有网络同步** - 客户端看不到属性变化  
> 3. **无法扩展** - 不支持持续Buff、等级加成等
>
> 后来改成 GameplayEffect 四行代码的标准流程，特别是 **AddSourceObject** 解决了来源追踪问题，**DurationType** 让同样的代码能支持瞬发/持续/周期三种效果。表现层移到蓝图后，策划可以自己配数值和特效，我专注写逻辑。"

---

## 章节回顾

### 本章节核心收获
（用户填写学习后的总结）

### 遗留疑问
（用户填写还不理解的地方）

### 薄弱点记录（Claude维护）
- [ ] DurationType 的概念和应用场景（第4题答错）
- [ ] Level 在 GE 中的作用（第1题未提及）
