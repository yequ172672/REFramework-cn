#include "Localization.hpp"

#include <spdlog/spdlog.h>

#include <json.hpp>

#include <fstream>
#include <unordered_map>

namespace utility::localization {
namespace {
namespace fs = std::filesystem;
using json = nlohmann::json;

using TranslationTable = std::unordered_map<std::string, std::string>;

TranslationTable& chinese_table() {
    static TranslationTable table{};
    return table;
}

// English is the source language, so this table is empty by default. It exists
// so that an external en.json can override individual strings.
TranslationTable& english_table() {
    static TranslationTable table{};
    return table;
}

Language g_language{ Language::SimplifiedChinese };
bool g_initialized{ false };

void add(TranslationTable& table, const char* key, const char* value) {
    if (key != nullptr && value != nullptr) {
        table[key] = value;
    }
}

const TranslationTable& active_table() {
    return g_language == Language::English ? english_table() : chinese_table();
}

void load_table_from_file(const fs::path& file, TranslationTable& table) {
    std::ifstream stream{ file };

    if (!stream.is_open()) {
        return;
    }

    try {
        const auto data = json::parse(stream);

        if (!data.is_object()) {
            spdlog::warn("Localization: {} is not a JSON object, ignoring", file.string());
            return;
        }

        size_t count{ 0 };

        for (auto it = data.begin(); it != data.end(); ++it) {
            if (!it.value().is_string()) {
                continue;
            }

            table[it.key()] = it.value().get<std::string>();
            ++count;
        }

        spdlog::info("Localization: loaded {} entries from {}", count, file.string());
    } catch (const std::exception& e) {
        spdlog::error("Localization: failed to parse {}: {}", file.string(), e.what());
    }
}

struct Entry {
    const char* key;
    const char* value;
};

constexpr Entry g_chinese_entries[]{
    // ---------------------------------------------------------------- core
    { "Default Menu Key: Insert", "默认菜单键：Insert" },
    { "Transparency", "透明度" },
    { "Makes the UI transparent when not focused.", "界面未聚焦时使其变为半透明。" },
    { "Input Passthrough", "输入穿透" },
    { "Allows mouse and keyboard inputs to register to the game while the UI is focused.", "界面聚焦时仍允许鼠标和键盘输入传递给游戏。" },
    { "REFramework is currently initializing...", "REFramework 正在初始化……" },
    { "This menu will close after initialization if you have the remember option enabled.", "若启用了记忆选项，初始化完成后此菜单会自动关闭。" },
    { "REFramework error: %s", "REFramework 错误：%s" },
    { "Author: praydog", "作者：praydog" },
    { "Inspired by the Kanan project.", "灵感来源于 Kanan 项目。" },
    { "Branch: %s", "分支：%s" },
    { "Commits: %i", "提交数：%i" },
    { "Commit hash: %s", "提交哈希：%s" },
    { "Tag: %s", "标签：%s" },
    { "Commits past tag: %i", "标签之后的提交数：%i" },
    { "Build date: %s", "构建日期：%s" },
    { "Build time: %s", "构建时间：%s" },
    { "Licenses", "许可证" },
    { "Engine information", "引擎信息" },
    { " Config: %s", " 配置：%s" },
    { " Version: %s", " 版本：%s" },
    { " TDB Version: %i", " TDB 版本：%i" },
    { "Unable to determine engine version.", "无法确定引擎版本。" },
    { "Press any key...", "请按任意键……" },
    { "Not bound", "未绑定" },
    { "About", "关于" },
    { "Configuration", "配置" },
    { "Language", "语言" },
    { "Menu Key", "菜单键" },
    { "Show Cursor Key", "显示光标快捷键" },
    { "Remember Menu Open/Closed State", "记住菜单打开/关闭状态" },
    { "Draw Cursor With Menu Open", "打开菜单时显示光标" },
    { "Font", "字体" },
    { "Font Size", "字体大小" },

    // --------------------------------------------------------- shared widgets
    { "Enabled", "启用" },
    { "Disabled", "禁用" },
    { "Enable", "启用" },
    { "Value", "数值" },
    { "Type", "类型" },
    { "Name", "名称" },
    { "Address", "地址" },
    { "Size", "大小" },
    { "Options", "选项" },
    { "Info", "信息" },
    { "Index", "索引" },
    { "Flags", "标志" },
    { "Children", "子节点" },

    // ----------------------------------------------------------- mod titles
    { "APIProxy", "API 代理" },
    { "BackBufferRenderer", "后备缓冲渲染器" },
    { "Camera", "相机" },
    { "ChainViewer", "链条查看器" },
    { "DeveloperTools", "开发者工具" },
    { "FaultyFileDetector", "问题文件检测器" },
    { "FirstPerson", "第一人称" },
    { "FreeCam", "自由相机" },
    { "GameObjectsDisplay", "游戏对象显示" },
    { "Graphics", "图形" },
    { "Hooks", "钩子" },
    { "IntegrityCheckBypass", "完整性校验绕过" },
    { "LooseFileLoader", "松散文件加载器" },
    { "LooseTextureLoader", "松散纹理加载器" },
    { "ManualFlashlight", "手电筒" },
    { "MethodDatabase", "方法数据库" },
    { "ObjectExplorer", "对象浏览器" },
    { "PluginLoader", "插件加载器" },
    { "RE8VR", "RE8 VR" },
    { "REFrameworkConfig", "REFramework 配置" },
    { "Scene", "场景" },
    { "ScriptRunner", "脚本运行器" },
    { "VR", "VR" },

    // -------------------------------------------------------- ScriptRunner
    { "Online match detected. Scripts will not be loaded. Existing scripts have been unloaded.", "检测到在线对战。脚本将不会加载。已卸载现有脚本。" },
    { "Run script", "运行脚本" },
    { "Reset scripts", "重置脚本" },
    { "Spawn Debug Console", "生成调试控制台" },
    { "Open Debug Console at Startup", "启动时打开调试控制台" },
    { "Garbage Collection Stats", "垃圾回收统计" },
    { "Megabytes in use: %.2f", "已使用内存（MB）：%.2f" },
    { "Garbage Collection Handler", "垃圾回收处理器" },
    { "Garbage Collection Mode", "垃圾回收模式" },
    { "Minor GC Multiplier", "次要 GC 乘数" },
    { "Major GC Multiplier", "主要 GC 乘数" },
    { "Garbage Collection Type", "垃圾回收类型" },
    { "Garbage Collection Budget", "垃圾回收预算" },
    { "Log Lua Errors to Disk", "将 Lua 错误记录到磁盘" },
    { "Last Error Time: %.2f seconds ago", "上次错误时间：%.2f 秒前" },
    { "Last Script Error: %s", "上次脚本错误：%s" },
    { "No Script Errors... yet!", "目前没有脚本错误！" },
    { "Known scripts:", "已知脚本：" },
    { "No scripts loaded.", "未加载任何脚本。" },
    { "Script Generated UI", "脚本生成的界面" },

    // --------------------------------------------------------------- Hooks
    { "Performance", "性能" },
    { "Enable Profiling", "启用性能分析" },
    { "Application Entry Times", "应用入口耗时" },
    { "Total REFramework Time: %.3fms", "REFramework 总耗时：%.3fms" },
    { "Total Game Time: %.3fms", "游戏总耗时：%.3fms" },
    { "Game Time: %s: %.2fms", "游戏耗时：%s：%.2fms" },
    { "REFramework Pre Time: %.2fms", "REFramework 前置耗时：%.2fms" },
    { "REFramework Post Time: %.2fms", "REFramework 后置耗时：%.2fms" },
    { "Total Time: %.2fms", "总耗时：%.2fms" },

    // ------------------------------------------------------ LooseFileLoader
    { "Failed to hook successfully. This mod will not work.", "挂钩失败。此模块将无法工作。" },
    { "Enable Loose File Loader", "启用松散文件加载器" },
    { "Files encountered: %d", "已处理的文件数：%d" },
    { "Loose files loaded: %d", "已加载的松散文件数：%d" },
    { "Clear stats", "清除统计" },
    { "Debug", "调试" },
    { "Enable file cache", "启用文件缓存" },
    { "Cache hits: %d", "缓存命中数：%d" },
    { "Uncached hits: %d", "未缓存命中数：%d" },
    { "Clear existence cache", "清除存在性缓存" },
    { "Log accessed files", "记录访问过的文件" },
    { "Logs all accessed files to <game_dir>/reframework_accessed_files.txt", "将所有访问过的文件记录到 <game_dir>/reframework_accessed_files.txt" },
    { "Log loose files", "记录松散文件" },
    { "Logs loaded loose files to <game_dir>/reframework_loose_files.txt", "将已加载的松散文件记录到 <game_dir>/reframework_loose_files.txt" },
    { "Show recent files", "显示最近的文件" },
    { "Recent accessed files", "最近访问的文件" },
    { "Recent loose files", "最近的松散文件" },

    // --------------------------------------------------- LooseTextureLoader
    { "Loose Texture", "松散纹理" },
    { "Disable Texture Cache (force reload)", "禁用纹理缓存（强制重新加载）" },
    { "The game caches textures and only reloads them from disk when nothing references them anymore.\nEnabling this option forces a reload every time, bypassing the cache.\nWARNING: This creates duplicate texture instances in memory and can be very memory-intensive.\nOnly use this while actively editing textures, and disable it during normal play.", "游戏会缓存纹理，只有当没有任何东西再引用它们时才会从磁盘重新加载。\n启用此选项会强制每次都重新加载，从而绕过缓存。\n警告：这会在内存中产生重复的纹理实例，并可能非常消耗内存。\n仅在正在编辑纹理时使用，正常游玩时请将其禁用。" },
    { "Unique loose textures loaded: %zu", "已加载的唯一松散纹理数：%zu" },
    { "Reset Counters", "重置计数" },
    { "Loaded Loose Textures (recent)", "已加载的松散纹理（最近）" },
    { "... and %zu more", "……以及另外 %zu 个" },

    // ---------------------------------------------------------- PluginLoader
    { "Loaded plugins:", "已加载的插件：" },
    { "No plugins loaded.", "未加载任何插件。" },
    { "Errors:", "错误：" },
    { "Warnings:", "警告：" },

    // ------------------------------------------------------------- Graphics
    { "RE4 Scope Tweaks", "RE4 瞄准镜调整" },
    { "Enable Scope Tweaks", "启用瞄准镜调整" },
    { "Enable Interlaced Rendering", "启用隔行渲染" },
    { "Scope Image Quality", "瞄准镜画质" },
    { "Ultrawide/FOV Options", "超宽屏/视野选项" },
    { "Ultrawide/FOV/Aspect Ratio Fix", "超宽屏/视野/宽高比修复" },
    { "16:10 Mode: Use Black Bars (maintain 16:9)", "16:10 模式：使用黑边（保持 16:9）" },
    { "On a 16:10 display, keeps the game at 16:9 with black bars instead of\nstretching to fill the screen. Prevents UI element misalignment.", "在 16:10 显示器上，保持游戏以 16:9 显示并加黑边，\n而不是拉伸铺满屏幕。可避免 UI 元素错位。" },
    { "Ultrawide: Constrain UI to 16:9", "超宽屏：将 UI 限制为 16:9" },
    { "Ultrawide: Constrain Child UI to 16:9", "超宽屏：将子 UI 限制为 16:9" },
    { "Ultrawide: UI Correction", "超宽屏：UI 校正" },
    { "Ultrawide: Enable Vertical FOV", "超宽屏：启用垂直视野" },
    { "Ultrawide: Override FOV", "超宽屏：覆盖视野" },
    { "Ultrawide: FOV Multiplier", "超宽屏：视野倍率" },
    { "Force Render Resolution to Window Size", "强制渲染分辨率匹配窗口尺寸" },
    { "GUI Options", "界面选项" },
    { "Hide GUI", "隐藏界面" },
    { "Hide GUI key", "隐藏界面快捷键" },
    { "Ray Tracing Tweaks", "光线追踪调整" },
    { "Enable Ray Tracing Tweaks", "启用光线追踪调整" },
    { "Disable Raster Shadows (with PT)", "禁用光栅阴影（使用 PT 时）" },
    { "Always Recreate RT Component", "始终重建 RT 组件" },
    { "Recreates the RT component. Useful if Ray Tracing Tweaks is not working.", "重建光线追踪组件。当光线追踪调整不起作用时很有用。" },
    { "Ray Trace Type", "光线追踪类型" },
    { "Can draw another RT pass over the main RT pass. Useful for hybrid rendering.\nExample: Set Ray Trace Type to Pure and Ray Trace Clone Type to ASVGF. This adds RTGI to the path traced image.\nPath Space Filter is also another good alternative for RTGI but it costs more performance.\n", "可在主 RT 通道之上再绘制一个 RT 通道，适用于混合渲染。\n示例：将光线追踪类型设为 Pure，将光线追踪克隆类型设为 ASVGF。这样可为路径追踪图像添加 RTGI。\n路径空间滤波（Path Space Filter）也是 RTGI 的另一个不错选择，但性能开销更高。\n" },
    { "Ray Trace Clone Type Pre", "光线追踪克隆类型 Pre" },
    { "Ray Trace Clone Type Post", "光线追踪克隆类型 Post" },
    { "Ray Trace Clone Type True", "光线追踪克隆类型 True" },
    { "Uses a completely separate RT component instead of re-using the main RT component.\nMight crash or have other issues. Use with caution.\n", "使用完全独立的 RT 组件，而不复用主 RT 组件。\n可能崩溃或出现其他问题，请谨慎使用。\n" },
    { "Bounce Count", "反弹次数" },
    { "Samples Per Pixel", "每像素采样数" },
    { "Shader Playground", "着色器试验场" },
    { "Enable Shader Playground", "启用着色器试验场" },
    { "Interception Shader", "拦截着色器" },
    { "Replace Shader", "替换着色器" },
    { "Custom Shader {}", "自定义着色器 {}" },
    { "Dispatch Mode", "调度模式" },
    { "Thread Group X", "线程组 X" },
    { "Thread Group Y", "线程组 Y" },
    { "Thread Group Z", "线程组 Z" },
    { "Constant", "常量" },
    { "Valid hash", "有效哈希" },

    // ---------------------------------------------------------- FirstPerson
    { "Show In Cutscenes", "过场动画中显示" },
    { "VR Specific Settings", "VR 专用设置" },
    { "Smooth XZ Movement (VR)", "平滑 XZ 移动（VR）" },
    { "Smooth Y Movement (VR)", "平滑 Y 移动（VR）" },
    { "Roomscale Movement (VR)", "房间尺度移动（VR）" },
    { "Adjust Hand Offset", "调整手部偏移" },
    { "Scale Debug", "缩放调试" },
    { "Scale Debug 2", "缩放调试 2" },
    { "Offset Debug", "偏移调试" },
    { "VR Scale", "VR 缩放" },
    { "Controller rotation (Left)", "控制器旋转（左）" },
    { "Controller rotation (Right)", "控制器旋转（右）" },
    { "Controller position (Left)", "控制器位置（左）" },
    { "Controller position (Right)", "控制器位置（右）" },
    { "Controller 1", "控制器 1" },
    { "Controller 2", "控制器 2" },
    { "General Settings", "常规设置" },
    { "Disable Camera Light", "禁用相机灯光" },
    { "Hide Joint Mesh", "隐藏关节模型" },
    { "Force Rotate Joint", "强制旋转关节" },
    { "Rotate Body", "旋转身体" },
    { "Disable Vignette", "禁用暗角" },
    { "Change Toggle Key", "更改切换键" },
    { "CameraOffset", "相机偏移" },
    { "CameraSpeed", "相机速度" },
    { "CameraShake", "相机抖动" },
    { "FOVOffset", "视野偏移" },
    { "FOVMultiplier", "视野倍率" },
    { "CurrentFOV", "当前视野" },
    { "BodyRotateSpeed", "身体旋转速度" },
    { "Joint", "关节" },
    { "Joints", "关节" },

    // --------------------------------------------------------------- Camera
    { "Make sure to tick \"Enabled\" for any of the below settings to take effect.", "请确保勾选下方任意设置中的“启用”才会生效。" },
    { "Vignette Brightness", "暗角亮度" },
    { "RE8 FOV", "RE8 视野" },
    { "RE8 Aiming FOV", "RE8 瞄准视野" },
    { "These below settings are separate and do not require \"Enabled\" to be ticked.", "以下设置相互独立，无需勾选“启用”。" },
    { "Use Custom Global FOV", "使用自定义全局视野" },
    { "Global FOV", "全局视野" },

    // ----------------------------------------------------- ManualFlashlight
    { "Change Key", "更改按键" },
    { "Ignore Light Power On Zones", "忽略照明区域" },
    { "Light power on zones: %i\n", "照明区域数量：%i\n" },

    // ---------------------------------------------------- FaultyFileDetector
    { "Error: %s", "错误：%s" },
    { "Total faulty files encountered: %zu", "检测到的问题文件总数：%zu" },
    { "No faulty files detected!", "未检测到问题文件！" },
    { "Faulty files detected!", "检测到问题文件！" },
    { "See reframework_faulty_files.txt for full list and details. Use external tool to find out what mod/patch is causing the issue.", "完整列表与详情请查看 reframework_faulty_files.txt。请使用外部工具找出导致问题的 mod/补丁。" },
    { "Enable Faulty File Detector", "启用问题文件检测器" },
    { "Max Recent Files to Display", "最多显示的最近文件数" },
    { "Show recent##ShowRecentFaultyFiles", "最近的文件##ShowRecentFaultyFiles" },
    { "Unknown", "未知" },
    { "Missing File", "文件缺失" },
    { "Invalid File", "文件无效" },
    { "Should Be Encrypted", "应为加密" },

    // -------------------------------------------------- IntegrityCheckBypass
    { "PAK Directory Loading", "PAK 目录加载" },
    { "Allow loading PAKs inside %s directory. PAKs can be of any filename and ends with .pak (case-sensitive)", "允许加载 %s 目录内的 PAK。PAK 可任意命名，但必须以 .pak 结尾（区分大小写）" },
    { "Restart the game to apply changes.", "重启游戏以应用更改。" },
    { "List of custom PAKs loaded:", "已加载的自定义 PAK 列表：" },

    // -------------------------------------------------------------- FreeCam
    { "Lock Position", "锁定位置" },
    { "Disable Character Movement", "禁用角色移动" },
    { "Toggle Key", "切换键" },
    { "Move camera up Key", "相机上移键" },
    { "Move camera down Key", "相机下移键" },
    { "Lock Position Toggle Key", "锁定位置切换键" },
    { "Disable Movement Toggle Key", "禁用移动切换键" },
    { "Speed modifier Fast key", "速度加成（快）键" },
    { "Speed modifier Slow key", "速度加成（慢）键" },
    { "Rotation Speed", "旋转速度" },
    { "Speed", "速度" },
    { "Speed Modifier", "速度倍率" },

    // ---------------------------------------------------------------- Scene
    { "Timescale (Toggle) Key", "时间缩放（切换）键" },
    { "Timescale (Continuous) Key", "时间缩放（持续）键" },
    { "Use Application Timescale", "使用应用程序时间缩放" },
    { "Timescale", "时间缩放" },

    // ------------------------------------------------------------------- VR
    { "%s not loaded: %s not found", "%s 未加载：找不到 %s" },
    { "Please drop the %s file into the game's directory if you want to use %s", "如需使用 %s，请将 %s 文件放入游戏目录" },
    { "%s not loaded: %s", "%s 未加载：%s" },
    { "%s not loaded: Unknown error", "%s 未加载：未知错误" },
    { "No runtime loaded.", "未加载任何运行时。" },
    { "Hardware scheduling: %s", "硬件调度：%s" },
    { "WARNING: Hardware-accelerated GPU scheduling is enabled. This will cause the game to run slower.", "警告：已启用硬件加速 GPU 调度。这会导致游戏运行变慢。" },
    { "Go into your Windows Graphics settings and disable \"Hardware-accelerated GPU scheduling\"", "请进入 Windows 图形设置并禁用“硬件加速 GPU 调度”" },
    { "VR Runtime: %s", "VR 运行时：%s" },
    { "Render Resolution: %d x %d", "渲染分辨率：%d x %d" },
    { "Resolution can be changed in SteamVR", "可在 SteamVR 中更改分辨率" },
    { "Bindings", "按键绑定" },
    { "Resolution Scale", "分辨率缩放" },
    { "Sync Mode", "同步模式" },
    { "Set Standing Height", "设置站立高度" },
    { "Set Standing Origin", "设置站立原点" },
    { "Recenter View", "视角回中" },
    { "Reinitialize Runtime", "重新初始化运行时" },
    { "Set Standing Origin Key", "设置站立原点快捷键" },
    { "Recenter View Key", "视角回中快捷键" },
    { "Use AFR", "使用 AFR" },
    { "Decoupled Camera Pitch", "解耦相机俯仰" },
    { "Positional Tracking", "位置追踪" },
    { "Head Oriented Audio", "头部朝向音频" },
    { "Use Custom View Distance", "使用自定义视距" },
    { "View Distance/FarZ", "视距/FarZ" },
    { "Inactivity Timer", "无操作计时器" },
    { "Joystick Deadzone", "摇杆死区" },
    { "2D UI Scale", "2D UI 缩放" },
    { "2D UI Distance", "2D UI 距离" },
    { "World-Space UI Scale", "世界空间 UI 缩放" },
    { "Overlay Rotation", "覆盖层旋转" },
    { "Overlay Position", "覆盖层位置" },
    { "Graphical Options", "图形选项" },
    { "Force Uncap FPS", "强制解除 FPS 上限" },
    { "Force Disable TAA", "强制禁用 TAA" },
    { "Force Disable Motion Blur", "强制禁用动态模糊" },
    { "Force Disable V-Sync", "强制禁用垂直同步" },
    { "Force Disable Lens Distortion", "强制禁用镜头畸变" },
    { "Force Disable Volumetrics", "强制禁用体积光" },
    { "Force Disable Lens Flares", "强制禁用镜头光晕" },
    { "Force Enable Dynamic Shadows", "强制启用动态阴影" },
    { "Allow Engine Overlays", "允许引擎覆盖层" },
    { "Enable Asynchronous Rendering", "启用异步渲染" },
    { "Desktop Recording Fix", "桌面录制修复" },
    { "Skip Present", "跳过呈现" },
    { "Debug info", "调试信息" },
    { "Disable Projection Matrix Override", "禁用投影矩阵覆盖" },
    { "Disable GUI Projection Matrix Override", "禁用 GUI 投影矩阵覆盖" },
    { "Disable View Matrix Override", "禁用视图矩阵覆盖" },
    { "Disable Backbuffer Size Override", "禁用后备缓冲尺寸覆盖" },
    { "Disable Temporal Fix", "禁用时间性修复" },
    { "Disable Post Effect Fix", "禁用后处理修复" },
    { "Prediction Scale", "预测缩放" },
    { "Raw Left", "原始左眼" },
    { "Raw Right", "原始右眼" },
    { "Avg Input Processing Delay (MS)", "平均输入处理延迟 (毫秒)" },

    // -------------------------------------------------------------- OpenXR
    { "Interaction Profile: %s", "交互配置：%s" },
    { "Restore Default Bindings", "恢复默认绑定" },
    { "Save Bindings", "保存绑定" },
    { "Interaction profile not loaded, try putting on your headset.", "交互配置未加载，请尝试戴上头显。" },
    { "New Binding (e.g. /user/hand/left/input/trigger)", "新绑定（例如 /user/hand/left/input/trigger）" },
    { "Action", "动作" },
    { "Add Binding", "添加绑定" },
    { "Vector2 Associations", "Vector2 关联" },
    { "Insert New Output", "插入新输出" },
    { "New Vector2 Activator", "新 Vector2 激活器" },
    { "New Vector2 Modifier", "新 Vector2 修饰器" },
    { "New Vector2 Output", "新 Vector2 输出" },
    { "Add Vector2 Association", "添加 Vector2 关联" },
    { "New Vector2 Value", "新 Vector2 数值" },

    // ---------------------------------------------------------- ChainViewer
    { "Effect Alpha", "效果透明度" },
    { "Chains", "链条" },
    { "Collision %d %d", "碰撞 %d %d" },
    { "Collision %d", "碰撞 %d" },
    { "Pair Joint", "配对关节" },

    // --------------------------------------------------- GameObjectsDisplay
    { "Legacy Mode", "旧版模式" },
    { "Object Effect Alpha", "物体效果透明度" },
    { "Max Distance for GameObjects", "GameObjects 最大距离" },

    // -------------------------------------------------------------- RE8VR
    { "Hide Upper Body", "隐藏上半身" },
    { "Hide Lower Body", "隐藏下半身" },
    { "Hide Arms", "隐藏手臂" },
    { "Auto Hide Upper Body in Cutscenes", "过场动画中自动隐藏上半身" },
    { "Auto Hide Lower Body in Cutscenes", "过场动画中自动隐藏下半身" },

    // ------------------------------------------------------- ObjectExplorer
    { "Dump SDK", "转储 SDK" },
    { "Dump il2cpp json Only", "仅转储 il2cpp json" },
    { "Initializing Dump...", "正在初始化转储……" },
    { "Dumping Types...", "正在转储类型……" },
    { "Dumping RSZ...", "正在转储 RSZ……" },
    { "Dumping Methods...", "正在转储方法……" },
    { "Dumping Fields...", "正在转储字段……" },
    { "Dumping Properties...", "正在转储属性……" },
    { "Adjusting RSZ...", "正在调整 RSZ……" },
    { "Dumping Deserializer Chains...", "正在转储反序列化链……" },
    { "Dumping Non-TDB Types...", "正在转储非 TDB 类型……" },
    { "Generating IDA SDK...", "正在生成 IDA SDK……" },
    { "Singletons", "单例" },
    { "Native Singletons", "原生单例" },
    { "Renderer", "渲染器" },
    { "Root layer: 0x%p", "根层：0x%p" },
    { "Scene layer: 0x%p", "场景层：0x%p" },
    { "Root Layer", "根层" },
    { "Scene Layer", "场景层" },
    { "Types", "类型" },
    { "Assemblies", "程序集" },
    { "Location: %s", "位置：%s" },
    { "Module Name: %s", "模块名称：%s" },
    { "Assembly Types", "程序集类型" },
    { "Assembly Methods", "程序集方法" },
    { "Assembly Instantiated Methods", "程序集实例化方法" },
    { "Assembly Member References", "程序集成员引用" },
    { "Search using Regex", "使用正则搜索" },
    { "Type Name", "类型名称" },
    { "Method Signature", "方法签名" },
    { "Field Signature", "字段签名" },
    { "Method Address", "方法地址" },
    { "Invalid address", "无效地址" },
    { "TDB Method Address", "TDB 方法地址" },
    { "REObject Address", "REObject 地址" },
    { "Create new game object", "创建新游戏对象" },
    { "Read Listeners", "读取监听器" },
    { "%s [Hits: %d [%d instructions]]", "%s [命中：%d [%d 条指令]]" },
    { "Pinned objects", "已固定对象" },
    { "Hooked methods", "已挂钩方法" },
    { "Hide uncalled methods", "隐藏未调用的方法" },
    { "Reset Stats", "重置统计" },
    { "Sort by", "排序方式" },
    { "Enable Auto Unhook", "启用自动取消挂钩" },
    { "Auto Unhook When", "自动取消挂钩时机" },
    { "Max Call Count", "最大调用次数" },
    { "Skip function call", "跳过函数调用" },
    { "Call count: %i", "调用次数：%i" },
    { "Time (ms): Delta %f, Total %f", "时间（毫秒）：增量 %f，总计 %f" },
    { "Callers", "调用者" },
    { "Sort Callers by", "调用者排序方式" },
    { "Return Addresses", "返回地址" },
    { "Thread IDs", "线程 ID" },
    { "Array Entries", "数组元素" },
    { "AutoGenerated Types", "自动生成的类型" },
    { "Add Component", "添加组件" },
    { "Name: %s", "名称：%s" },
    { "Transform", "变换" },
    { "Folder", "文件夹" },
    { "Destroy Component", "销毁组件" },
    { "Owner", "所有者" },
    { "0x%X: ChildComponents", "0x%X: 子组件" },
    { "PrevComponent", "上一个组件" },
    { "NextComponent", "下一个组件" },
    { "Attempt to Clone", "尝试克隆" },
    { "Child Layers", "子层" },
    { "Trees", "树" },
    { "Nodes", "节点" },
    { "ID: %u", "ID：%u" },
    { "Status1: %i", "状态1：%i" },
    { "Status2: %i", "状态2：%i" },
    { "Selector", "选择器" },
    { "Selector Condition", "选择器条件" },
    { "SINGLETON", "单例" },
    { "Size: 0x%X", "大小：0x%X" },
    { "Type Information", "类型信息" },
    { "TypeFlags", "类型标志" },
    { "Generic Type Definition", "泛型类型定义" },
    { "Reflection Methods: %i", "反射方法：%i" },
    { "Address: 0x%p", "地址：0x%p" },
    { "Function: 0x%p", "函数：0x%p" },
    { "Attempt to call", "尝试调用" },
    { "Type: %s", "类型：%s" },
    { "Reflection Properties: %i", "反射属性：%i" },
    { "STATIC", "静态" },
    { "Additional Information", "附加信息" },
    { "TypeKind: %i (%s)", "类型种类：%i (%s)" },
    { "Qualifiers: %i", "限定符：%i" },
    { "Attributes: %i", "属性标志：%i" },
    { "Size: %i", "大小：%i" },
    { "ManagedStr: %i", "托管字符串：%i" },
    { "VarType: %i", "变量类型：%i" },
    { "GlobalIndex: %i", "全局索引：%i" },
    { "TDB Fields: %i", "TDB 字段：%i" },
    { "CONST", "常量" },
    { "Listen for reads", "监听读取" },
    { "STUB", "占位" },
    { "DUPLICATE", "重复" },
    { "Virtual Index", "虚表索引" },
    { "Impl flags", "实现标志" },
    { "Called Method", "被调用的方法" },
    { "Bytes", "字节" },
    { "Instruction", "指令" },
    { "TDB Methods: %i", "TDB 方法：%i" },
    { "Null pointer", "空指针" },
    { "Ref Target: 0x%p", "引用目标：0x%p" },
    { "Set Value", "设置数值" },
    { "true", "真" },
    { "false", "假" },
    { "Invalid enum size, falling back to int32", "无效的枚举大小，回退为 int32" },
    { "Variable: 0x%p", "变量：0x%p" },
    { "Copy Address", "复制地址" },
    { "Copy Name", "复制名称" },
    { "Unpin", "取消固定" },
    { "Pin", "固定" },
    { "Log Hierarchy", "记录层级" },
    { "Hook All Methods", "挂钩所有方法" },
    { "Hook", "挂钩" },
    { "Unhook", "取消挂钩" },
    { "Unhook All Methods", "取消所有方法" },
    { "Call", "调用" },
};

void add_builtin_chinese() {
    auto& table = chinese_table();

    for (const auto& entry : g_chinese_entries) {
        add(table, entry.key, entry.value);
    }
}
} // namespace

void initialize() {
    if (g_initialized) {
        return;
    }

    g_initialized = true;
    g_language = Language::SimplifiedChinese;

    add_builtin_chinese();

    spdlog::info("Localization: initialized with {} zh-CN entries", chinese_table().size());
}

void load_language_files(const fs::path& languages_dir) {
    std::error_code ec{};
    fs::create_directories(languages_dir, ec);

    if (ec) {
        spdlog::warn("Localization: unable to create {}: {}", languages_dir.string(), ec.message());
        return;
    }

    load_table_from_file(languages_dir / "zh-CN.json", chinese_table());
    load_table_from_file(languages_dir / "en.json", english_table());
}

Language get_language() {
    return g_language;
}

void set_language(Language language) {
    g_language = language;
    spdlog::info("Localization: language set to {} ({})", get_language_name(language), get_language_code(language));
}

const char* get_language_code(Language language) {
    switch (language) {
    case Language::English:
        return "en";
    case Language::SimplifiedChinese:
    default:
        return "zh-CN";
    }
}

const char* get_language_name(Language language) {
    switch (language) {
    case Language::English:
        return "English";
    case Language::SimplifiedChinese:
    default:
        return "简体中文";
    }
}

Language language_from_name(std::string_view name, Language fallback) {
    if (name == "English" || name == "en" || name == "en-US") {
        return Language::English;
    }

    if (name == "简体中文" || name == "zh-CN" || name == "zh_CN" || name == "zh") {
        return Language::SimplifiedChinese;
    }

    return fallback;
}

const char* tr(const char* text) {
    if (text == nullptr) {
        return "";
    }

    const auto& table = active_table();
    const auto it = table.find(text);

    if (it != table.end()) {
        return it->second.c_str();
    }

    return text;
}

std::string translate(std::string_view text, Language language) {
    const auto& table = language == Language::English ? english_table() : chinese_table();
    const auto it = table.find(std::string{ text });

    if (it != table.end()) {
        return it->second;
    }

    return std::string{ text };
}

} // namespace utility::localization
