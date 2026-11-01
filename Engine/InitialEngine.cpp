#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Core/Logger.hpp>

int main() {
	jora::Window window;
	jora::Context context;

	jora::globalLogger.Message(
		jora::MessageBox{
			.message = "Launch engine!",
			.type = jora::MessageType::Debug });

	if (const auto windowResult = window.CreateWindow(); !windowResult)
	{
		jora::globalLogger.Message(
			jora::MessageBox{
				.message = "Failed create window!",
				.type = jora::MessageType::Highest });
		return windowResult;
	}

	if (const auto contextResult = context.Init(window.GetWindow()); contextResult != vk::Result::eSuccess)
	{
		jora::globalLogger.Message(
			jora::MessageBox{
				.message = "Failed create window!",
				.type = jora::MessageType::Highest });
		return static_cast<VkResult>(contextResult);
	}

	while (!window.WindowCloseCheck())
	{
		jora::HandlingEvens();
	}

	jora::globalLogger.Message(
		jora::MessageBox{
			.message = "Shutting down engine!",
			.type = jora::MessageType::Debug });

	return 0;
}