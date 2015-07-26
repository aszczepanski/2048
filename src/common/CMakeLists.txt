project(2048)

set(SRCS_COMMON
	GameAction.h
	GameState.cpp
	GameState.h
	ProgramOptions.h
	TuplesDescriptor.h
)

add_library(common ${SRCS_COMMON})
