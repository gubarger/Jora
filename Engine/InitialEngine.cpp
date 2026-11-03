#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Core/Logger.hpp>

int main() {
	jora::Window window;
	jora::Context context;

	if (const auto windowResult = window.CreateWindow(); !windowResult)
	{
		jora::DebugMessenger::Print({
			.message = "Failed to initialize window.",
			.type = jora::MessageType::Error });

		return windowResult;
	}

	if (context.Init(window.GetWindow()) != vk::Result::eSuccess)
	{
		jora::DebugMessenger::Print({
			.message = "Failed to initialize Vulkan API v1.3",
			.type = jora::MessageType::Error });

		return static_cast<int>(vk::Result::eErrorInitializationFailed);
	}

	while (!window.WindowCloseCheck())
	{
		jora::HandlingWindowEvens();
	}

	return 0;
}