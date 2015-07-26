#ifndef SRC_IO_CONSOLE_BOARD_DRAWER_H_
#define SRC_IO_CONSOLE_BOARD_DRAWER_H_

#include "io/IBoardDrawer.h"

class ConsoleBoardDrawer : public IBoardDrawer {
public:
	virtual void draw(GameState&);
};

#endif  // SRC_IO_CONSOLE_BOARD_DRAWER_H_
