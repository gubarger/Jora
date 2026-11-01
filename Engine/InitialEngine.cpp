#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Core/Logger.hpp>

int main() {
	jora::Window window;
	jora::Context context;

	jora::Message("Launch engine.", jora::MessageType::Default);

	if (const auto windowResult = window.CreateWindow(); !windowResult)
	{
		jora::Message(
			"An error occurred during initialization. Window create error."
			, jora::MessageType::Error);
		return windowResult;
	}

	if (const auto contextResult = context.Init(window.GetWindow()); contextResult != vk::Result::eSuccess)
	{
		jora::Message(
			"An error occurred during initialization. Vulkan context create error."
			, jora::MessageType::Error);

		return static_cast<VkResult>(contextResult);
	}

	while (!window.WindowCloseCheck())
	{
		jora::HandlingEvens();
	}

	jora::Message("Shutting down engine.", jora::MessageType::Default);

	return 0;
}