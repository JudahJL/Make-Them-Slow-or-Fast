#include "DataManager.h"
#include "Events.h"

void SKSEEvent::InitializeMessaging()
{
	if (!SKSE::GetMessagingInterface()->RegisterListener("SKSE", MessageListener))
		util::report_and_fail("Unable to register message listener.");
}

void SKSEEvent::MessageListener(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		DataManager::GetSingleton()->PatchSpells();
		break;
	default:
		break;
	}
}
