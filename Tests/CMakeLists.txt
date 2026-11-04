enable_testing()

find_package(GTest CONFIG REQUIRED)

set(gtest_forse_shared_crt ON CACHE BOOL "" FORCE)

add_executable(JoraTests
	LoggerTest.cpp
	ThreadPoolTest.cpp
)

target_include_directories(JoraTests PUBLIC
	${CMAKE_SOURCE_DIR}/Engine/Include
)

target_link_libraries(JoraTests
	PRIVATE
		JoraWarnings
		GTest::gtest_main
)

target_compile_features(JoraTests PRIVATE cxx_std_23)

include(GoogleTest)
gtest_discover_tests(JoraTests)