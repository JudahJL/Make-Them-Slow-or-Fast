#include "DataManager.h"
#include "Events.h"
#include "logging.h"

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	auto startSPL = std::chrono::high_resolution_clock::now();
	MTSOF::InitializeLogging();

	logger::info("{} {} is loading...", SKSE::PluginDeclaration::GetSingleton()->GetName(), SKSE::PluginDeclaration::GetSingleton()->GetVersion());
	auto refresh = spdlog::get("refreshLogger");
	if (refresh)
		MTSOF::DataManager::GetSingleton()->LoadJson(true, refresh);
	SKSE::Init(skse);
	SKSEEvent::InitializeMessaging();
	auto nanosecondsTakenForSPL = std::chrono::duration(std::chrono::high_resolution_clock::now() - startSPL);

	logger::info("Time Taken in SKSEPluginLoad(const SKSE::LoadInterface* a_skse) totally is {} nanoseconds or {} microseconds or {} milliseconds or {} seconds or {} minutes", nanosecondsTakenForSPL.count(),
		std::chrono::duration_cast<std::chrono::microseconds>(nanosecondsTakenForSPL).count(), std::chrono::duration_cast<std::chrono::milliseconds>(nanosecondsTakenForSPL).count(),
		std::chrono::duration_cast<std::chrono::seconds>(nanosecondsTakenForSPL).count(), std::chrono::duration_cast<std::chrono::minutes>(nanosecondsTakenForSPL).count());
	return true;
}
