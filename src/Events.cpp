#include "Settings.h"
#include "Events.h"

void SKSEEvent::InitializeMessaging() {
    if(!SKSE::GetMessagingInterface()->RegisterListener(MessageListener))
        SKSE::stl::report_and_fail("Unable to register message listener.");
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void SKSEEvent::MessageListener(SKSE::MessagingInterface::Message* message) {
    switch(message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::GetSingleton().PatchSpells();
            break;
        default: break;
    }
}
