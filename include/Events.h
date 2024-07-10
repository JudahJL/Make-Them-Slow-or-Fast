#pragma once

class SKSEEvent
{
public:
	static void InitializeMessaging();

private:
	static void MessageListener(SKSE::MessagingInterface::Message* message);
};
