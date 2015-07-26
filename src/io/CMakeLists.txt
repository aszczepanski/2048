project(2048)

set(SRCS_IO
	BasicInputFileReader.cpp
	BasicInputFileReader.h
	CProgramOptionsReader.cpp
	CProgramOptionsReader.h
	ConsoleBoardDrawer.cpp
	ConsoleBoardDrawer.h
	IBoardDrawer.h
	IInputFileReader.h
	IProgramOptionsReader.h
)

add_library(io ${SRCS_IO})
