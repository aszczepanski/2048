#ifndef SRC_COMMON_GAME_ACTION_H_
#define SRC_COMMON_GAME_ACTION_H_

#include <ostream>

enum GameAction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

std::ostream& operator<<(std::ostream& out, const GameAction action);

#endif  // SRC_COMMON_GAME_ACTION_H_
