#include <Jora/Graphics/Window.hpp>

#include <Jora/Core/Logger.hpp>

#include <GLFW/glfw3.h>

namespace jora
{

	Window::~Window()
	{
		glfwDestroyWindow(window_);
		glfwTerminate();

		MessageLogging::Message("Destroy Window and GLFW from memory.", MessageType::Success);
	}

	int32_t Window::CreateWindow()
	{
		if (!glfwInit())
		{
			MessageLogging::Message("Failed to initialize GLFW.", MessageType::Error);
			return 1;
		}

		if (!glfwVulkanSupported())
		{
			MessageLogging::Message("Vulkan is not supported this desktop.", MessageType::Error);
			return 1;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 920, "Jora", nullptr, nullptr);
		if (window_ == nullptr)
		{
			MessageLogging::Message("Failed to create window OS.", MessageType::Error);
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