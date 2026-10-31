#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Core/Logger.hpp>

int main() {
	jora::Window window;
	jora::Context context;

	jora::MessageLogging::Message("Launch engine.", jora::MessageType::Default);

	if (!window.CreateWindow() && context.Init(window.GetWindow()) != vk::Result::eSuccess)
	{
		jora::MessageLogging::Message(
			"An error occurred during initialization. Terminating with an error."
			, jora::MessageType::Error);
		return 1;
	}

	while (!window.WindowCloseCheck())
	{
		jora::HandlingEvens();
	}

	jora::MessageLogging::Message("Shutting down engine.", jora::MessageType::Default);

	return 0;
}