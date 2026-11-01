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

		Message("Destroy Window and GLFW from memory.", MessageType::Success);
	}

	int32_t Window::CreateWindow()
	{
		if (!glfwInit())
		{
			Message("Failed to initialize GLFW.", MessageType::Error);
			return GLFW_FALSE;
		}

		if (!glfwVulkanSupported())
		{
			Message("Vulkan is not supported this desktop.", MessageType::Error);
			return GLFW_FALSE;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 920, "Jora", nullptr, nullptr);
		if (window_ == nullptr)
		{
			Message("Failed to create window OS.", MessageType::Error);
			return GLFW_FALSE;
		}

		return GLFW_TRUE;
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