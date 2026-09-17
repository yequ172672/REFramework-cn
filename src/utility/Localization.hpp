#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

// Lightweight runtime localization for the REFramework UI.
//
// English is treated as the source language: the English strings in the source
// code are used verbatim, so a missing translation always falls back to them.
// Translations may be provided either by the built-in tables in
// Localization.cpp or by external JSON files dropped into
// "<persistent_dir>/reframework/languages/<language code>.json".
namespace utility::localization {

enum class Language : int32_t {
    SimplifiedChinese = 0,
    English = 1,
    // Follows the operating system UI language: Chinese systems get Simplified
    // Chinese, every other language gets English.
    Auto = 2,
};

// Registers the built-in translation tables. Idempotent.
void initialize();

// Loads "<languages_dir>/zh-CN.json" and "<languages_dir>/en.json", using their
// contents to override or extend the built-in translations. Missing or invalid
// files are ignored.
void load_language_files(const std::filesystem::path& languages_dir);

Language get_language();
void set_language(Language language);

// Language implied by the operating system UI language. Chinese systems map to
// Simplified Chinese, all other systems map to English.
Language detect_system_language();

// Stable identifier used for external language files, e.g. "zh-CN" or "en".
const char* get_language_code(Language language);

// Human readable name shown in the UI, e.g. "简体中文" or "English".
const char* get_language_name(Language language);

// Accepts either a language name ("简体中文", "English") or a code
// ("zh-CN", "en").
Language language_from_name(std::string_view name, Language fallback = Language::SimplifiedChinese);

// Returns the translation of `text` for the current language, or `text` itself
// when no translation is available. The returned pointer remains valid for the
// lifetime of the program. `text` may be null.
const char* tr(const char* text);

// Translation of `text` for an explicit language.
std::string translate(std::string_view text, Language language);

} // namespace utility::localization

// Wraps a user facing string literal so that it is translated when drawn.
#define REF_TR(text) ::utility::localization::tr(text)
