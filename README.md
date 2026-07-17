# CampusQueueSystem 🏫

校园资源多业务动态调度与排队系统 —— 基于 C++ 的实时监控大屏模拟。

## 项目概述

本系统模拟校园内两类紧缺资源（**图书馆座位** 和 **洗衣房洗衣机**）的智能排队调度。通过多维度优先级评分公式，在并发学生请求下实现高效、公平的资源分配，并提供终端动态可视化监控界面。

## 功能特性

- **智能优先级调度** — 综合 `紧急程度 × 50 + 信用分 - 请求时间 × 2` 三维权重评分，高分优先
- **多业务分流** — 图书馆座位与洗衣机独立排队，互不干扰
- **资源状态自刷新** — 座位固定占用 3 分钟，洗衣机固定占用 5 分钟，到期自动释放
- **实时动画大屏** — 每秒刷新终端界面，展示各资源占用人、排队积压量、拥堵提示
- **数据驱动** — 学生请求从 `students.txt` 反序列化加载，支持注释行与空行
- **运行效益报告** — 模拟结束后输出服务总量、平均等待时间等关键指标

## 项目结构

```
CampusQueueSystem/
├── CampusQueueSystem.sln          # Visual Studio 解决方案
├── CampusQueueSystem/
│   ├── CampusQueueSystem.cpp      # 主程序入口 + 动画渲染引擎
│   ├── models.h                   # 数据模型 (User / CampusResource / LibrarySeat / WashingMachine)
│   ├── scheduler.h                # 优先级队列调度器 (SystemScheduler)
│   ├── dataloader.h               # 学生数据文件加载器 (DataLoader)
│   ├── stats.h                    # 运行统计数据管理器 (StatsManager)
│   ├── students.txt               # 学生请求样本数据
│   ├── CampusQueueSystem.vcxproj       # VS 项目文件
│   └── CampusQueueSystem.vcxproj.filters
└── .gitignore
```

## 快速开始

### 环境要求

- **Windows** 操作系统（使用了 `<windows.h>` 控制台 API）
- **Visual Studio 2022**（或兼容的 MSBuild / C++17 工具链）

### 编译运行

1. 使用 Visual Studio 打开 `CampusQueueSystem.sln`
2. 确保解决方案配置为 `Debug | x64`
3. 按 `Ctrl+F5` 编译并运行

或通过命令行：

```powershell
msbuild CampusQueueSystem.sln /p:Configuration=Debug /p:Platform=x64
.\x64\Debug\CampusQueueSystem.exe
```

> 确保 `students.txt` 与可执行文件在同一目录下。

## 数据格式

`students.txt` 每行定义一个学生请求：

```
# 姓名  信用分  请求时间  紧急度  目标资源(1:座位, 2:洗衣机)
stud_A 85 800 1 1
stud_B 92 800 3 1
stud_C 65 800 2 2
```

| 字段 | 说明 | 取值范围 |
|------|------|----------|
| 姓名 | 学生标识 | 字符串 |
| 信用分 | 信誉评分 | 0–100 |
| 请求时间 | 发起请求的系统时钟 | 800–830（代表 8:00–8:30） |
| 紧急度 | 需求紧急程度 | 1（普通）, 2（较急）, 3（非常急） |
| 目标资源 | 请求的资源类型 | 1: 图书馆座位, 2: 洗衣机 |

以 `#` 开头的行为注释，空行自动跳过。

## 调度算法

```
优先级分数 = urgency × 50 + credibility − requestTime × 2
```

- **紧急度** 权重最高（×50），确保急事优先
- **信用分** 作为正向加成，鼓励诚信
- **请求时间** 作为轻微衰减，相同条件下先到者分数更低（越早越优先）

系统使用 `std::priority_queue`（大顶堆）维护等待队列，每次资源释放时弹出最高分用户。

## 运行效果

```
===================== 校园资源多业务动态监控大屏 =====================

 [当前系统时间] 08:15
 [专属队列积压] 图书馆座位排队: 3 人   |   洗衣房排队: 1 人

-----------------------------------------------------------------
| 时间(8:xx) |   101 (座位)   |   102 (座位)   |  201 (洗衣机)  |
-----------------------------------------------------------------
|     00     |       A        |       B        |       -        |
|     01     |       A        |       B        |       -        |
|     02     |       A        |       B        |       C        |
|     ...    |      ...       |      ...       |      ...       |
-----------------------------------------------------------------

 [监控提示] 业务高峰拥堵，双业务队列正在智能分流叫号...
```

## OOP 设计

```
CampusResource (抽象基类)
├── LibrarySeat    — 固定占用 3 个时间单位
└── WashingMachine — 固定占用 5 个时间单位

User — 学生实体，包含 5 维属性

SystemScheduler — 优先级队列封装
StatsManager    — 服务统计与报告
DataLoader      — 文件反序列化工厂
```

核心设计遵循：
- **开闭原则**：新增资源类型只需继承 `CampusResource` 并实现 `updateStatus()`
- **单一职责**：模型、调度、统计、数据加载各司其职
- **RAII 内存管理**：通过全局 `g_memoryGarbageCollector` 统一回收堆对象

## License

MIT — 仅供学习与课程设计参考。
