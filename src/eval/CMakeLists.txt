project(2048)

set(SRCS_EVAL
	ExpectimaxEvaluator.cpp
	ExpectimaxEvaluator.h
	IEvaluator.h
	IterativeExpectimaxEvaluator.cpp
	IterativeExpectimaxEvaluator.h
	RandomEvaluator.cpp
	RandomEvaluator.h
)

set(LIBS_EVAL
	common
)

add_library(eval ${SRCS_EVAL})
target_link_libraries(eval ${LIBS_EVAL})
