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
	)
else ()
	target_compile_options(JoraWarnings INTERFACE
		-Wall
		-Wextra
		-Wshadow
		-Wnon-virtual-dtor
		-Wold-style-cast
		-Wcast-align
		-Wunused
		-Woverloaded-virtual
	)
endif ()

add_subdirectory(Engine)