#include <Jora/Graphics/Window.hpp>

#include <GLFW/glfw3.h>

namespace jora
{

	int32_t Window::CreateWindow()
	{
		if (!glfwInit())
		{
			return 1;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 920, "Jora", nullptr, nullptr);
		if (window_ == nullptr)
		{
			return 1;
		}

		return 0;
	}

	bool Window::WindowCloseCheck() const
	{
		return glfwWindowShouldClose(window_);
	}

	GLFWwindow* Window::GetWindow() const { return window_; }

	std::pair<uint32_t, const char**> GetWindowExtensions()
	{
		uint32_t count = 0u;

		const char** extensions = glfwGetRequiredInstanceExtensions(&count);

		return { count, extensions };
	}

	void HandlingEvens()
	{
		glfwPollEvents();
	}

}