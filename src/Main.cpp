#include "DataManager.h"
#include "Events.h"
#include "UI.h"
#include "logging.h"

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
#ifdef NDEBUG
	while( !IsDebuggerPresent() ) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
#endif
	std::chrono::time_point startSPL = std::chrono::high_resolution_clock::now();
	Logging                 logger;

	logger::info("{} {} is loading...", SKSE::PluginDeclaration::GetSingleton()->GetName(), SKSE::PluginDeclaration::GetSingleton()->GetVersion());
	DataManager* d = DataManager::GetSingleton();
	d->LoadMainJson();
	d->ReloadLoggingIfNecessary(d->_MainJsonData["Logging"]["LogLevel"]);
	d->ProcessMainJson();
	d->LogDataManagerContents();
	d->LoadExclusionJsonFiles();
	SKSE::Init(skse);
	SKSEEvent::InitializeMessaging();
	SMFRenderer::Register();
	std::chrono::nanoseconds nanosecondsTakenForSPL = std::chrono::duration(std::chrono::high_resolution_clock::now() - startSPL);

	logger::info("Time Taken in SKSEPluginLoad(const SKSE::LoadInterface* a_skse) totally is {} nanoseconds or {} microseconds or {} milliseconds or {} seconds or {} minutes", nanosecondsTakenForSPL.count(),
		std::chrono::duration_cast<std::chrono::microseconds>(nanosecondsTakenForSPL).count(), std::chrono::duration_cast<std::chrono::milliseconds>(nanosecondsTakenForSPL).count(),
		std::chrono::duration_cast<std::chrono::seconds>(nanosecondsTakenForSPL).count(), std::chrono::duration_cast<std::chrono::minutes>(nanosecondsTakenForSPL).count());
	return true;
}
