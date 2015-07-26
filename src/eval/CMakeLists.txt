project(2048)

set(SRCS_EVAL
	IEvaluator.h
	RandomEvaluator.cpp
	RandomEvaluator.h
)

add_library(eval ${SRCS_EVAL})
