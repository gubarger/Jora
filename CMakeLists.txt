cmake_minimum_required(VERSION 3.30)
project(JoraEngine)

add_library(JoraWarnings INTERFACE)

if (MSVC)
	target_compile_options(JoraWarnings INTERFACE
		/W4
		/permissive-
		/w14242
		/w14265
		/w14311
		$<$<CONFIG:Release>:/O2>
	)
else () # Clang / GCC
	target_compile_options(JoraWarnings INTERFACE
		-Wall
		-Wextra
		-Wshadow
		-Wnon-virtual-dtor
		-Wold-style-cast
		-Wcast-align
		-Wunused
		-Woverloaded-virtual
		$<$<CONFIG:Release>:-msse4.2>
	)
endif ()

option(JORA_ENGINE_TESTS "Build test for Jora engine." OFF)

add_subdirectory(Engine)

if (JORA_ENGINE_TESTS)
	add_subdirectory(Tests)
endif ()