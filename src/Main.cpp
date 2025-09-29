#include "Settings.h"
#include "Events.h"
#include "UI.h"
#include "logging.h"
#include "timeit.h"

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    InitializeLogging();
    {
        [[maybe_unused]] const timeit t;
        SKSE::Init(skse, false);
        Settings::GetSingleton().PopulateFormIDMap().LoadCurrPreset().LoadPresets().SetLogAndFlushLevel();
        SKSEEvent::InitializeMessaging();
        SMFRenderer::Register();
    }
    return true;
}
