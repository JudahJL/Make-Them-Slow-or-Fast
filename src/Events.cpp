#include "DataManager.h"
#include "Events.h"
#include "reload.h"

InputEventProcessor* InputEventProcessor::GetSingleton()
{
	static InputEventProcessor singleton;
	return std::addressof(singleton);
}

RE::BSEventNotifyControl InputEventProcessor::ProcessEvent(RE::InputEvent* const* e, RE::BSTEventSource<RE::InputEvent*>*)
{
	if (*e)
		if (auto buttonEvent = (*e)->AsButtonEvent(); buttonEvent && buttonEvent->HasIDCode() && (buttonEvent->IsDown() || buttonEvent->IsPressed())) {
			auto m = MTSOF::DataManager::GetSingleton();
			if (int key = buttonEvent->GetIDCode(); key == m->_keyForRefresh) {
				m->LoadJson(false, _refresh);
				m->spellPatch(false, _refresh);
			} else if (key == m->_keyForReset) {
				SpellItemVectorClass::GetSingleton()->resetSpells(_refresh);
			}
		}
	return RE::BSEventNotifyControl::kContinue;
}

void InputEventProcessor::RegisterEvent()
{
	if (!GetSingleton()->_registered)
		if (auto input = RE::BSInputDeviceManager::GetSingleton(); input)
			input->AddEventSink(GetSingleton());
		else
			logger::info("Already Registered for Event");
}

void InputEventProcessor::UnregisterEvent()
{
	if (GetSingleton()->_registered)
		if (auto input = RE::BSInputDeviceManager::GetSingleton(); input)
			input->RemoveEventSink(GetSingleton());
		else
			logger::info("Already Registered for Event");
}

void SKSEEvent::InitializeMessaging()
{
	if (!SKSE::GetMessagingInterface()->RegisterListener(MessageListener))
		util::report_and_fail("Unable to register message listener.");
}

void SKSEEvent::MessageListener(SKSE::MessagingInterface::Message* message)
{
	auto refresh = spdlog::get("refreshLogger");
	auto m = MTSOF::DataManager::GetSingleton();
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		SpellItemVectorClass::GetSingleton()->loadVectors();
		m->spellPatch(true, refresh);
		if (m->_redo)
			InputEventProcessor::GetSingleton()->RegisterEvent();
		break;
	default:
		break;
	}
}
