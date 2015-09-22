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

add_library(eval ${SRCS_EVAL})
