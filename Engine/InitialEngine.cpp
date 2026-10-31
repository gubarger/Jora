#include <Jora/Graphics/Window.hpp>
#include <Jora/Graphics/Context.hpp>

int main() {
	jora::Window window;
	jora::Context context;

	if (!window.CreateWindow() && context.Init(window.GetWindow()) != vk::Result::eSuccess)
	{
		return 1;
	}

	while (!window.WindowCloseCheck())
	{
		jora::HandlingEvens();
	}
}