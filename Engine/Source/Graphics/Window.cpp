#include <Jora/Graphics/Window.hpp>

#include <Jora/Core/Logger.hpp>

#include <GLFW/glfw3.h>

namespace jora
{

	Window::~Window()
	{
		if (window_ != nullptr)
		{
			glfwDestroyWindow(window_);
		}

		glfwTerminate();
	}

	int32_t Window::CreateWindow()
	{
		if (!glfwInit())
		{
			return GLFW_FALSE;
		}

		if (!glfwVulkanSupported())
		{
			return GLFW_FALSE;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 920, "Jora", nullptr, nullptr);
		if (window_ == nullptr)
		{
			return GLFW_FALSE;
		}

		return GLFW_TRUE;
	}

	bool Window::WindowCloseCheck() const
	{
		return glfwWindowShouldClose(window_);
	}

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