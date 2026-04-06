# Chapter 3 - 属性系统 问答记录

## 视频要点
Chapter 3 part 2-4：定义 Health/Mana 属性，使用 ATTRIBUTE_ACCESSORS 宏生成访问器，创建 EffectActor 直接修改属性（v1 版本，后续会改进）。

---

### 问题 1：概念理解 - ATTRIBUTE_ACCESSORS 宏生成了哪些代码？为什么要用它而不是手写？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- 生成：Getter、Setter、Initter、PropertyGetter
- 手写繁琐且容易出错，宏保证一致性
- 与 GAS 的反射系统配合，支持蓝图访问

---

### 问题 2：代码分析 - FGameplayAttributeData 包含哪些成员？BaseValue 和 CurrentValue 有什么区别？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- BaseValue：基础值（装备、等级决定）
- CurrentValue：当前值（Buff/Debuff 修改后的实际值）
- GE 修改的是 CurrentValue，升级/换装修改 BaseValue

---

### 问题 3：设计决策 - 为什么 EffectActor v1 使用 const_cast 直接修改属性是错误的？这会带来什么问题？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- 绕过 GAS 框架，破坏设计一致性
- 没有网络同步，客户端看不到变化
- 无法追踪来源（谁修改的）
- 无法支持复杂逻辑（持续时间、标签条件等）

---

### 问题 4：扩展场景 - 如果要添加一个新属性"护盾值(Shield)"，需要修改哪些文件？做哪些步骤？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
1. AttributeSet.h：定义 FGameplayAttributeData Shield + ATTRIBUTE_ACCESSORS
2. AttributeSet.cpp：GetLifetimeReplicatedProps 中添加 DOREPLIFETIME
3. 添加 OnRep_Shield 回调函数
4. 在 UI 中添加对应的委托和绑定

---

## 章节回顾

### 本章节核心收获
（用户填写）

### 遗留疑问
（用户填写）
