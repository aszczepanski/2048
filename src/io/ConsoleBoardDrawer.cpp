#include "io/ConsoleBoardDrawer.h"

#include <iostream>

#include "common/GameState.h"

using namespace std;

void ConsoleBoardDrawer::draw(GameState& gameState) {
	cout << "================================" << endl;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			int val = gameState.getTileValue(i*4 + j);
			cout << (val ? 1<<val : 0) << "\t";
		}
		cout << endl;
	}
	cout << "================================" << endl;
}
