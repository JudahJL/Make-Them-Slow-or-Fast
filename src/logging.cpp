#include "logging.h"

Logging::Logging()
{
	std::optional<fs::path> path = logger::log_directory();

	if (!path) {
		util::report_and_fail("Unable to lookup SKSE logs directory.");
	}

	*path /= std::format("{}.log", SKSE::PluginDeclaration::GetSingleton()->GetName());

	std::shared_ptr<spdlog::logger> log = std::make_shared<spdlog::logger>(SKSE::PluginDeclaration::GetSingleton()->GetName().data());

	SetupLog(path, log);
}

Logging::Logging(spdlog::level::level_enum CommonLevel, std::string_view Name)
{
	std::optional<fs::path> path = logger::log_directory();

	if (!path) {
		util::report_and_fail("Unable to lookup SKSE logs directory.");
	}

	*path /= std::format("{}.log", Name);

	std::shared_ptr<spdlog::logger> log = std::make_shared<spdlog::logger>(Name.data());

	SetupLog(path, log, CommonLevel, CommonLevel);
}

Logging::Logging(spdlog::level::level_enum SetLevel, spdlog::level::level_enum FlushLevel, std::string_view Name)
{
	std::optional<fs::path> path = logger::log_directory();

	if (!path) {
		util::report_and_fail("Unable to lookup SKSE logs directory.");
	}

	*path /= std::format("{}.log", Name);

	std::shared_ptr<spdlog::logger> log = std::make_shared<spdlog::logger>(Name.data());

	SetupLog(path, log, SetLevel, FlushLevel);
}

Logging::Logging(std::string_view Name)
{
	std::optional<fs::path> path = logger::log_directory();

	if (!path) {
		util::report_and_fail("Unable to lookup SKSE logs directory.");
	}

	*path /= std::format("{}.log", Name);

	std::shared_ptr<spdlog::logger> log = std::make_shared<spdlog::logger>(Name.data());

	SetupLog(path, log);
}

void Logging::SetupLog(std::optional<fs::path> path, std::shared_ptr<spdlog::logger>& log, spdlog::level::level_enum SetLevel, spdlog::level::level_enum FlushLevel)
{
	if (IsDebuggerPresent()) {
		log->sinks().reserve(2);
		log->sinks().push_back(_MSVCSink);
	} else {
		log->sinks().reserve(1);
	}
	log->sinks().push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));

	log->set_level(SetLevel);
	log->flush_on(FlushLevel);
	set_default_logger(std::move(log));
	spdlog::set_pattern(_Pattern);
}
