project(2048)

set(SRCS_MAIN
	main.cpp
)

set(LIBS_MAIN
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

add_executable(main ${SRCS_MAIN})
target_link_libraries(main ${LIBS_MAIN})
