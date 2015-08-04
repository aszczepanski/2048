project(2048)

set(SRCS_COMMON
	GameAction.cpp
	GameAction.h
	GameState.cpp
	GameState.h
	GameStats.cpp
	GameStats.h
	ProgramOptions.h
	TuplesDescriptor.h
)

add_library(common ${SRCS_COMMON})
