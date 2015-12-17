project(2048)

set(SRCS_WEB_API
	WebApi.cpp
	WebApi.h
)

set(SRCS_MAIN
	Game.cpp
	Game.h
	main.cpp
	MainApplication.cpp
	MainApplication.h
)

set(LIBS_MAIN
	common
	eval
	io
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

add_library(WebApi SHARED ${SRCS_WEB_API})
target_link_libraries(WebApi ${LIBS_MAIN})

add_executable(main ${SRCS_MAIN})
target_link_libraries(main ${LIBS_MAIN})
