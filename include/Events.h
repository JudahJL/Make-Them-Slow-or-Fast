#pragma once

class InputEventProcessor : public RE::BSTEventSink<RE::InputEvent*>
{
	InputEventProcessor() = default;
	~InputEventProcessor() = default;
	InputEventProcessor(const InputEventProcessor&) = delete;
	InputEventProcessor(InputEventProcessor&&) = delete;
	InputEventProcessor& operator=(const InputEventProcessor&) = delete;
	InputEventProcessor& operator=(InputEventProcessor&&) = delete;

public:
	static InputEventProcessor* GetSingleton();

	RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* e, RE::BSTEventSource<RE::InputEvent*>*) override;

	static void RegisterEvent();

	static void UnregisterEvent();

private:
	std::shared_ptr<spdlog::logger> _refresh{ spdlog::get("refreshLogger") };
	bool                            _registered{ false };
};

class SKSEEvent
{
public:
	static void InitializeMessaging();

private:
	static void MessageListener(SKSE::MessagingInterface::Message* message);
};
