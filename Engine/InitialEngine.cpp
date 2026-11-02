#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>
#include <Jora/Core/Logger.hpp>
#include <iostream>
int main() {
	jora::Window window;
	jora::Context context;

	if (const auto windowResult = window.CreateWindow(); !windowResult)
	{
	}

	if (const auto contextResult = context.Init(window.GetWindow()); contextResult != vk::Result::eSuccess)
	{
	}

	while (!window.WindowCloseCheck())
	{
		jora::HandlingEvens();
	}

	return 0;
}