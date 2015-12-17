#!/usr/bin/python
# -*- coding: utf-8 -*-

''' Help the user achieve a high score in a real game of 2048 by using a move searcher. '''

from __future__ import print_function
import ctypes
import time
import os

def initialize_ailib(args):
	if not args.lib:
		for suffix in ['so', 'dll', 'dylib']:
				dllfn = 'lib/libWebApi.' + suffix
				if not os.path.isfile(dllfn):
						continue
				ailib = ctypes.CDLL(dllfn)
				break
		else:
				print("Couldn't find 2048 library lib/libWebApi.{so,dll,dylib}! Make sure to build it first.")
				exit()
	else:
		ailib = ctypes.CDLL(args.lib)

	ailib.setStrategy(args.strategy.encode('UTF-8'))
	ailib.setUnzip(args.unzip)
	ailib.setMaxTime(args.time)
	ailib.setMaxDepth(args.depth)
	ailib.setEvalMultithreading(args.multithreading)

	ailib.loadTuplesDescriptor.restype = ctypes.c_bool
	if not ailib.loadTuplesDescriptor():
		exit()
	ailib.initializeTables()
	ailib.initializeEvaluator();

	ailib.bestAction.argtypes = [ctypes.c_uint64]

	return ailib

def to_c_board(m):
    board = 0
    i = 0
    for row in m:
        for c in row:            
            board |= c << (4*i)
            i += 1
    return board

def print_board(m):
    for row in m:
        for c in row:
            print('%8d' % c, end=' ')
        print()

def _to_val(c):
    if c == 0: return 0
    return 2**c

def to_val(m):
    return [[_to_val(c) for c in row] for row in m]

def _to_score(c):
    if c <= 1:
        return 0
    return (c-1) * (2**c)

def to_score(m):
    return [[_to_score(c) for c in row] for row in m]

def find_best_move(ailib, m):
    board = to_c_board(m)
    return ailib.bestAction(board)

def movename(move):
    return ['up', 'down', 'left', 'right'][move]

def play_game(ailib, gamectrl):
    moveno = 0
    start = time.time()
    while 1:
        state = gamectrl.get_status()
        if state == 'ended':
            break
        elif state == 'won':
            time.sleep(0.75)
            gamectrl.continue_game()

        moveno += 1
        board = gamectrl.get_board()
        move = find_best_move(ailib, board)
        if move < 0:
            break
        print("%010.6f: Score %d, Move %d: %s" % (time.time() - start, gamectrl.get_score(), moveno, movename(move)))
        gamectrl.execute_move(move)

    score = gamectrl.get_score()
    board = gamectrl.get_board()
    maxval = max(max(row) for row in to_val(board))
    print("Game over. Final score %d; highest tile %d." % (score, maxval))

def parse_args(argv):
    import argparse

    parser = argparse.ArgumentParser(description="Use the AI to play 2048 via browser control")
    parser.add_argument('-p', '--port', help="Port number to control on (default: 32000 for Firefox, 9222 for Chrome)", type=int)
    parser.add_argument('-b', '--browser', help="Browser you're using. Only Firefox with the Remote Control extension, and Chrome with remote debugging, are supported right now.", default='firefox', choices=('firefox', 'chrome'))
    parser.add_argument('-k', '--ctrlmode', help="Control mode to use. If the browser control doesn't seem to work, try changing this.", default='hybrid', choices=('keyboard', 'fast', 'hybrid'))

    parser.add_argument('--multithreading', help="Enable multithreading.", default=True, type=bool)
    parser.add_argument('--depth', help="Maximum depth.", default=1, type=int)
    parser.add_argument('--time', help="Time limit [ms].", default=0, type=int)
    parser.add_argument('--strategy', help="Strategy input file.", default="data/2048_strategies/tiles-test-v1.bin.special", type=str)
    parser.add_argument('--unzip', help="Unzip strategy input file.", default=False, type=bool)

    parser.add_argument('--lib', help="WebApi library.", default="", type=str)

    return parser.parse_args(argv)

def main(argv):
    args = parse_args(argv)

    ailib = initialize_ailib(args)

    if args.browser == 'firefox':
        from web_ctrl.ffctrl import FirefoxRemoteControl
        if args.port is None:
            args.port = 32000
        ctrl = FirefoxRemoteControl(args.port)
    elif args.browser == 'chrome':
        from web_ctrl.chromectrl import ChromeDebuggerControl
        if args.port is None:
            args.port = 9222
        ctrl = ChromeDebuggerControl(args.port)

    if args.ctrlmode == 'keyboard':
        from web_ctrl.gamectrl import Keyboard2048Control
        gamectrl = Keyboard2048Control(ctrl)
    elif args.ctrlmode == 'fast':
        from web_ctrl.gamectrl import Fast2048Control
        gamectrl = Fast2048Control(ctrl)
    elif args.ctrlmode == 'hybrid':
        from web_ctrl.gamectrl import Hybrid2048Control
        gamectrl = Hybrid2048Control(ctrl)

    if gamectrl.get_status() == 'ended':
        gamectrl.restart_game()

    play_game(ailib, gamectrl)

if __name__ == '__main__':
    import sys
    exit(main(sys.argv[1:]))
