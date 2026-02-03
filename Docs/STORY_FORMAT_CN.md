# Salt2D / manosaba-plus 剧本规范（Draft）

本规范描述 `StoryGraph + Node Resources` 的组织方式，以及 VN / Debate / Present 三类节点资源的格式约定。
当前版本允许脚本包含“未来扩展字段/指令”，未实现时应**安全忽略**。

---

## 1. 目录结构

一个 Story 包（如 Demo）目录结构如下：

```
Assets/Story/<StoryName>/
  <story>.graph.json
  cast.json
  VN/         # VN/错误/BE 文本
  Debate/     # debate 结构化资源
  Present/    # present 结构化资源
```

* `*.graph.json`：拓扑（节点/边/触发/effects/params），是运行时跳转的唯一真相。
* `cast.json`：人物字典
* `VN/`：以文本为主的节点内容（VN、Error、BE）。
* `Debate/`：辩论节点资源（statements + menus + options）。
* `Present/`：证物/证人选择节点资源（items）。

---

## 2. 图文件：`*.graph.json`

### Node（节点）

* `id`：稳定 id（建议 ASCII）
* `type`：`vn | debate | present | error | be`
* `resource`：相对路径（相对 graph 文件所在目录）
* `params`：可选参数（如 `time_limit_sec`、`be_node`）

### Edge（边）

* `from` / `to`
* `trigger`：`auto | option | pick | no_commit | time_depleted | hp_depleted`
* `key`：当 trigger 为 `option/pick` 时必须存在（对应 optionId/itemId）
* `effects`：可选，边上转场演出（如 `cue: rebuttal`）

> 约定：跳转只由 graph 控制；资源只负责“显示/交互入口”，不直接写 jump。

---

## 3. VN 节点资源（文本 DSL）

### 3.1 台词行（必须支持）

格式（推荐）：

```
<speaker> "<text>"
```

示例：

```
樱羽 艾玛 "好大的苹果，为什么苹果这么大……"
二阶堂 希罗 "别走神，艾玛。"
```

说明：

* `<speaker>` 为显示名（可中文/日文），后续可通过 `cast.json` 映射到 actorId。
* `<text>` 为 UTF-8 字符串。
* 允许括号式旁白写在 text 里（例如 `"(……)"`）。

### 3.2 CUE 行（可选）

用于演出提示（当前可忽略）：

```
= cue_name =
```

示例：

```
= 论破 =
```

### 3.3 指令行（未来扩展，当前可忽略但必须不报错）

用于舞台/立绘/镜头等导演提示。以 `@` 开头：

```
@cmd key=value key=value ...
```

**执行时机约定**：指令在其后的第一条“可见输出”（通常是下一条台词）出现之前执行；连续多条 `@` 表示同一时刻的多条变化。

建议的最小命令集合（后续实现 Stage 时对齐）：

* `@show actor=<ActorId> slot=<left|right|center> pose=<...> expr=<...>`
* `@hide actor=<ActorId>`
* `@set  actor=<ActorId> expr=<...> pose=<...>`（差分修改）
* `@move actor=<ActorId> slot=<...>`
* `@bg   id=<BgId>`（可选）
* `@camera preset=<PresetId>`（可选）

示例：

```
@show actor=Emma slot=left pose=stand expr=normal
@set actor=Emma expr=awkward
樱羽 艾玛 "我感觉想不到什么想问的事情……"
```

> 约定：VN 中允许同屏多角色。指令只表达“变化（delta）”，舞台全量状态应在运行时持久化维护。

---

## 4. Debate 节点资源（JSON）

文件：`Debate/<id>.json`

### 4.1 statements

每条 statement 表示“一段飘字内容”，一次只显示一个 speaker 与一段 text。

```json
"statements": [
  { "speaker": "...", "text": "..." }
]
```

* `text` 允许包含可疑点标记：`{sus:SPAN_ID}...{/sus}`
  解析时应保留原文；交互命中由 spanId 驱动。

### 4.2 menus

菜单定义与可疑点绑定，并指定所属 statement：

```json
"menus": [
  {
    "menu_id": "m_q1",
    "statement_index": 3,
    "span_id": "Q1",
    "options": [
      { "option_id": "opt_rebut", "label": "（反驳）..." },
      { "option_id": "opt_agree", "label": "（赞成）..." }
    ]
  }
]
```

约定：

* 一个 statement 可以有多个疑问点（多个 spanId），对应多个 menu（同 statement_index，不同 span_id）。
* 打开菜单（open）不算 commit；选择 option（commit）才触发 graph 的 `option` 边。
* 若一轮 statements 播放结束仍无 commit，应触发 graph 的 `no_commit` 边。

### 4.3 未来扩展（可选字段，当前可忽略）

* `stage_init`：进入 debate 节点时的舞台摆放
* statement 级 `delta`：该 statement 触发的单角色差分（用于立绘/镜头对准说话人）

---

## 5. Present 节点资源（JSON）

文件：`Present/<id>.json`

```json
{
  "prompt": "出示证物",
  "items": [
    { "item_id": "evid_pen", "label": "希罗的钢笔" },
    { "item_id": "evid_apple", "label": "有点大的苹果" }
  ]
}
```

约定：

* 玩家 pick 某 item 后触发 graph 的 `pick` 边（key=item_id）。
* Present 可用于证物/证人，语义一致。

---

## 6. 编码与命名约定

* 所有文件使用 UTF-8（推荐无 BOM 或可兼容 BOM）。
* `nodeId / optionId / itemId / spanId` 建议使用 ASCII 稳定 id，便于引用与 diff。
* 显示文案（speaker、text、label）允许中文/日文。
