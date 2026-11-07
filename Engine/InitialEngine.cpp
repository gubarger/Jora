#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Graphics/Renderer.hpp>
#include <Jora/Core/Time.hpp>
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

	jora::Renderer render(context);

	if (render.Init() != vk::Result::eSuccess)
	{
		jora::DebugMessenger::Print({ .message = "Failed to init renderer.", .type = jora::MessageType::Error });
		return static_cast<int>(vk::Result::eErrorInitializationFailed);
	}

	jora::Time time;
	time.Reset();

	while (!window.WindowCloseCheck())
	{
		time.Tick();

		jora::HandlingWindowEvens();
		render.Record();
	}

	return 0;
}