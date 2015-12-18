project(2048)

set(SRCS_IO
	BinaryInputFileReader.cpp
	BinaryInputFileReader.h
	BinaryInputFileWriter.cpp
	BinaryInputFileWriter.h
	CProgramOptionsReader.cpp
	CProgramOptionsReader.h
	ConsoleBoardDrawer.cpp
	ConsoleBoardDrawer.h
	IBoardDrawer.h
	IInputFileReader.h
	IProgramOptionsReader.h
	TextInputFileReader.cpp
	TextInputFileReader.h
)

set(LIBS_IO
	common
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

add_library(io ${SRCS_IO})
target_link_libraries(io ${LIBS_IO})
