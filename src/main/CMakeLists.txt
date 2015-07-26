project(2048)

set(SRCS_MAIN
	Game.cpp
	Game.h
	main.cpp
)

set(LIBS_MAIN
	common
	eval
	io
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

add_executable(main ${SRCS_MAIN})
target_link_libraries(main ${LIBS_MAIN})
