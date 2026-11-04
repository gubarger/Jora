find_package(Vulkan REQUIRED)
find_package(vk-bootstrap CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(Jora)

target_precompile_headers(Jora PRIVATE Include/Jora/Graphics/DefaultVulkan.hpp)

target_sources(Jora
	PRIVATE
		InitialEngine.cpp
		Source/Graphics/Context.cpp
		Source/Graphics/DefaultVulkan.cpp
		Source/Graphics/Window.cpp
		Source/Graphics/CommandPools.cpp
		Source/Graphics/Renderer.cpp
		Source/Core/Logger.cpp
		Source/Core/ThreadPool.cpp
		Source/Core/Time.cpp
)

target_include_directories(Jora
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Jora
	PRIVATE
		$<$<CONFIG:Debug>:JoraWarnings>
		Vulkan::Vulkan
		vk-bootstrap::vk-bootstrap
		glfw
		volk::volk
)

target_compile_definitions(Jora
	PRIVATE
		GLM_FORCE_INLINE
		GLM_FORCE_PURE
		GLM_FORCE_ALIGNED
)

target_compile_definitions(Jora PRIVATE GLM_FORCE_SSE42)
target_compile_features(Jora PRIVATE cxx_std_23)