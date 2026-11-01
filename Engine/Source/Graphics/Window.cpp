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

		jora::globalLogger.Message(
			jora::MessageBox{
				.message = "Destroy the window and GLFW library!",
				.type = jora::MessageType::Debug });
	}

	int32_t Window::CreateWindow()
	{
		if (!glfwInit())
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Failed to init GLFW!",
					.type = jora::MessageType::Highest });
			return GLFW_FALSE;
		}

		if (!glfwVulkanSupported())
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Vulkan is not supported!",
					.type = jora::MessageType::Highest });
			return GLFW_FALSE;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 920, "Jora", nullptr, nullptr);
		if (window_ == nullptr)
		{
			jora::globalLogger.Message(
				jora::MessageBox{
					.message = "Window is not created (null)!",
					.type = jora::MessageType::Highest });
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