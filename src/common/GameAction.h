#ifndef SRC_COMMON_GAME_ACTION_H_
#define SRC_COMMON_GAME_ACTION_H_

#include <ostream>

enum GameAction {
	NO_ACTION = 0x0,
	UP = 0x1,
	RIGHT = 0x2,
	DOWN = 0x4,
	LEFT = 0x8
};

const GameAction gameActions[] = {UP, RIGHT, DOWN, LEFT};

std::ostream& operator<<(std::ostream& out, const GameAction action);

#endif  // SRC_COMMON_GAME_ACTION_H_
