# 界面语言 / UI Localization

REFramework 的界面支持中英双语，并可自行覆盖或扩展翻译。

## 切换语言

在游戏内按 `Insert` 打开菜单，进入 **配置（Configuration） → 语言（Language）**，可选择：

- **简体中文**（默认）
- **English**

切换后立即生效，并保存到 `re2_fw_config.txt`，下次启动沿用。

## 覆盖或自定义翻译

程序启动时会读取以下两个文件（若存在）：

```
<游戏目录>/reframework/languages/zh-CN.json
<游戏目录>/reframework/languages/en.json
```

- 文件为 JSON 对象，**键**是界面上的英文原文，**值**是对应语言的文本。
- 与内置翻译同名的键会被覆盖；未覆盖的文案仍使用内置翻译。
- 文件必须为 UTF-8 编码。
- 文件缺失或格式错误时会被忽略，不影响正常使用。

示例（`zh-CN.json`）：

```json
{
  "Configuration": "配置",
  "Run script": "运行脚本",
  "My Custom Button": "我的自定义按钮"
}
```

注意事项：

- 键必须与界面中的英文原文**完全一致**，包括 `%s`、`%d`、`%.2f` 等格式符。
- 翻译中需保留相同的格式符，顺序与英文一致，例如 `"Loaded: %d"` 对应 `"已加载：%d"`。
- 以 `##` 结尾的文本，`##` 之后是 ImGui 内部 ID，翻译时必须原样保留，例如 `"Show recent##ShowRecentFaultyFiles"` 对应 `"最近的文件##ShowRecentFaultyFiles"`。
- 界面中不存在的键不会生效，但不会报错。

## 为 C++ mod 添加本地化

内置 mod 之外的 C++ 代码可以使用同一个翻译接口：

```cpp
#include "utility/Localization.hpp"

ImGui::Text(REF_TR("My custom label"));
ImGui::Text(REF_TR("Items: %d"), count);
```

`REF_TR` 在英文模式下原样返回英文，在中文模式下返回对应翻译（无翻译时回退英文）。随后可在 `reframework/languages/zh-CN.json` 中为这些文案提供中文。
