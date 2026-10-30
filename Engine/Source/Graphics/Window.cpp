#include <Jora/Graphics/Window.hpp>

#include <GLFW/glfw3.h>

namespace jora
{

	int32_t Window::CreateWindow()
	{
		if (const auto result = glfwInit(); !result)
		{
			return result;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 920, "Jora", nullptr, nullptr);
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