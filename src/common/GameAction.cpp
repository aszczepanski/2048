#include "common/GameAction.h"

#include <ostream>

using namespace std;

ostream& operator<<(ostream& out, const GameAction action) {
	switch (action) {
	case LEFT: return out << "LEFT";
	case RIGHT: return out << "RIGHT";
	case UP: return out << "UP";
	case DOWN: return out << "DOWN";
	default: return out << "NO_ACTION";
	}
}
