#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

// Note: add new modules here before using
#include <unordered_set>
#include <ranges>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#define RAPIDJSON_SCHEMA_USE_INTERNALREGEX 0
#define RAPIDJSON_SCHEMA_USE_STDREGEX      1
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/schema.h>

namespace logger = SKSE::log;
namespace fs     = std::filesystem;

using namespace std::literals;

#define DLLEXPORT __declspec(dllexport)

#ifndef BASE_PATH
#    define BASE_PATH "data/SKSE/Plugins/"
#endif
