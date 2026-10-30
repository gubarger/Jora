find_package(Vulkan REQUIRED)
find_package(vk-bootstrap CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)

add_executable(Jora)

target_sources(Jora
	PRIVATE
		InitialEngine.cpp
		Source/Graphics/Context.cpp
)

target_include_directories(Jora
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Jora
	PRIVATE
		Vulkan::Vulkan
		vk-bootstrap::vk-bootstrap
		glfw
		volk::volk
)