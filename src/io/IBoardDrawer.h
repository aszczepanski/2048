#ifndef SRC_IO_IBOARD_DRAWER_H_
#define SRC_IO_IBOARD_DRAWER_H_

class GameState;

class IBoardDrawer {
public:
	virtual void draw(GameState&) = 0;
};

#endif  // SRC_IO_IBOARD_DRAWER_H_
