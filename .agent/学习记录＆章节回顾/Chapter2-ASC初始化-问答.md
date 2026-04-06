# Chapter 2 - ASC 与 AS 初始化 问答记录

## 视频要点
Chapter 2 part 6-8：构建 ASC 和 AS，设置复制模式，初始化 Ability Actor Info。核心是将 ASC 放在 PlayerState 中，并通过 InitAbilityActorInfo 绑定 Owner 和 Avatar。

---

### 问题 1：概念理解 - 为什么 ASC 要放在 PlayerState 而不是 Character 中？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- PlayerState 在网络游戏中跨关卡存在（持久化）
- Character 会销毁/重生（如死亡），但玩家数据需要保留
- 死亡重生时技能冷却、Buff 等数据不丢失
- 支持换角色不换数据（如变身系统）

---

### 问题 2：代码分析 - PossessedBy 和 OnRep_PlayerState 分别什么时候调用？为什么要在这两个地方都调用 InitAbilityActorInfo？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- PossessedBy：只在服务器调用，当 Controller 控制 Character 时
- OnRep_PlayerState：只在客户端调用，当 PlayerState 从服务器复制到客户端时
- 两者都要初始化是因为服务器和客户端都需要知道 ASC 的 Owner 和 Avatar
- 确保无论服务器还是客户端，ASC 都能正确工作

---

### 问题 3：设计决策 - InitAbilityActorInfo 中的 Owner 和 Avatar 分别是什么？为什么需要区分它们？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- Owner：通常是 PlayerState，数据的拥有者，决定数据存多久
- Avatar：通常是 Character，表现的载体，可以销毁/重生
- 分离设计支持：死亡重生（Avatar 换，Owner 不变）、变身系统（换 Avatar 不换数据）

---

### 问题 4：扩展场景 - 如果要在敌人（AI）上使用 ASC，应该放在哪里？怎么初始化？

**你的回答：**
（待填写）

**判断：**
（待判断）

**关键点：**
- AI 没有 PlayerState，所以 ASC 放在 Character 上
- 在 Character::PostInitializeComponents() 中初始化
- AI 不需要区分 Owner/Avatar，都是自己

---

## 章节回顾

### 本章节核心收获
（用户填写学习后的总结）

### 遗留疑问
（用户填写还不理解的地方）
