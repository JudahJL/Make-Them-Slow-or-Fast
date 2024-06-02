#include "logging.h"
namespace fs = std::filesystem;

void MTSOF::InitializeLogging()
{
	auto path = logger::log_directory();
	std::optional<fs::path> customPath{ LOGPATH };

	if (!path) {
		util::report_and_fail("Unable to lookup SKSE logs directory.");
	}

	if (!customPath) {
		fs::create_directory(LOGPATH);
	}
	auto sData = std::format("{}.log", SKSE::PluginDeclaration::GetSingleton()->GetName()); //Type: const char*

	*path /= sData;
	*customPath /= sData;
	
	auto log = std::make_shared<spdlog::logger>("MTSOF");
	auto refreshLogger = std::make_shared<spdlog::logger>("refreshLogger");

	if (IsDebuggerPresent()) {
		log->sinks().reserve(2);
		refreshLogger->sinks().reserve(2);
		log->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
		refreshLogger->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
	} else {
		log->sinks().reserve(1);
		refreshLogger->sinks().reserve(1);
	}
	log->sinks().push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
	refreshLogger->sinks().push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(customPath->string(), true));
	
	const char* pattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v";
	const auto& commonLogLevel{ spdlog::level::info }; //Type: spdlog::level::level_enum

	log->set_level(commonLogLevel);
	log->flush_on(commonLogLevel);
	set_default_logger(std::move(log));
	spdlog::set_pattern(pattern);

	
	refreshLogger->set_level(commonLogLevel);
	refreshLogger->flush_on(commonLogLevel);
	refreshLogger->set_pattern(pattern);

	spdlog::register_logger(refreshLogger);
}
