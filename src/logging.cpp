#include "logging.h"

void InitializeLogging() {
    // ReSharper disable once CppLocalVariableMayBeConst
    auto path{ logger::log_directory() };
    auto plugin = SKSE::PluginDeclaration::GetSingleton();
    if(!path) { SKSE::stl::report_and_fail("Unable to lookup SKSE logs directory."); }
    std::array<char, MAX_PATH> buff;
    sprintf_s(buff.data(), MAX_PATH, "%s.log", plugin->GetName().data());
    *path /= buff.data();

    auto  log{ std::make_shared<spdlog::logger>("Global") };
    auto& log_sinks{ log->sinks() };

    if(REX::W32::IsDebuggerPresent()) {
        log_sinks.reserve(2);
        const auto msvc_sink{ std::make_shared<spdlog::sinks::msvc_sink_mt>() };
        msvc_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] "
                               "[Make_Them_Slow_or_Fast.dll,%s:%#] %v");
        log_sinks.emplace_back(msvc_sink);
    }
    const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    log_sinks.emplace_back(file_sink);

    log->set_level(spdlog::level::trace);
    log->flush_on(spdlog::level::off);
    spdlog::set_default_logger(std::move(log));
    SPDLOG_INFO("{} v{} is loading...", plugin->GetName(), plugin->GetVersion().string("."));
}
