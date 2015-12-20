# 2048 AI #

AI Controller for the game 2048. As far as we are aware, it is the best 2048 strategy known do date (December 2015). It scores on average 532636±11444 at depth 3 (1000 games), and ca. 566561±27164 at depth 5 (300 games). At depth 5 it achieves the tile 8k in 100%, 16k in 97%, and 32k in 60% of games. 

The program uses classical expectimax with an evaluation function, which has been learned using a variant of temporal difference learning. This work extends our earlier one published as:

> Temporal Difference Learning of N-Tuple Networks for the Game 2048, Marcin Szubert and Wojciech Jaśkowski, Proceedings of IEEE Conference on Computational Intelligence and Games, pp. 1-8, August 2014, Dortmund, Germany, ([preprint](http://www.cs.put.poznan.pl/mszubert/pub/szubert2014cig.pdf "preprint"))

## Requirements ##

* C++11 compiler
* CMake 3.0+
* Boost 1.49.0+ (program_options, accumulators)

If you want to use the web application you will also need:

* Python 2
* Chrome or Firefox

Tested on:

* Mac OS X 10.11.1, Apple LLVM version 7.0.2
* Ubuntu 15.04 64-bit, g++ 4.9.2
* Windows 7 64-bit, Visual Studio 14 2015

## Strategy files ##

In order to run the application you need to have a file with an evaluation function. Some basic (not very good, but small) evaluation functions are already in ```data/2048_strategies/``` directory. For best results download and unzip our [best evaluation function](https://dl.dropboxusercontent.com/u/66539680/eval-function2.bin.special.zip).

Some strategy files are compressed - that's our way to save some memory. You can either use compressed version or decompress them and speed up calculations. Note that the decompressed version (of the above strategy) requires about 5GB of RAM whereas the compressed version requires about 4GB of RAM.

You can specify the strategy file and uncompress option using command line arguments (see [program parameters](#program-parameters)).

## Program parameters ##

#### Console application ####

For running multiple games and testing the AI's capabilities

+ **--strategy arg** - strategy input file (by default data/2048_strategies/2048_a_weak_player.bin.txt)
+ **--uncompress arg** - uncompress strategy (true/false, by default true). Faster but requires more RAM.
+ **--seed arg** - random seed (by default based on time elapsed since epoch)
+ **--games arg** - number of games (by default 1)
+ **--time arg** - maximum time for one round [ms], 0 means no time limit (by default 0)
+ **--depth arg** - maximum depth for expectimax (by default 1)
+ **--threads arg** - number of threads (each thread plays different game, by default 1)
+ **--eval_multithreading** - enable multithreading in expectimax algorithm
+ **-h [ --help ]** - produce help message
+ **-v [ --verbose ]** - show board and score after each round

#### Web Application ####

For observing how the AI works on the [2048 game site](http://gabrielecirulli.github.io/2048/)

+ **-b [ --browser ]** - choose browser (Chrome or Firefox, by default Firefox)
+ **-p [ --port PORT ]** - port number to control on (default: 32000 for Firefox, 9222 for Chrome)
+ **--strategy arg** - strategy input file (by default data/2048_strategies/2048_a_weak_player.bin.txt)
+ **--uncompress arg** - uncompress strategy (true/false, by default true). Faster but requires more RAM.
+ **--time arg** - maximum time for one round [ms], 0 means no time limit (by default 0)
+ **--depth arg** - maximum depth for expectimax (by default 1)
+ **--multithreading arg** - enable multithreading in expectimax algorithm (true/false, by default true)

## Building and running ##

#### Running console application ####

##### Linux/OS X #####

1. Go to the project root directory
2. Run ```cmake .```
3. Run ```make```. The ```lib/``` and ```bin/``` directories will be created.

##### Windows #####

1. Go to the project root directory
2. Create and go to ```_build/``` directory
3. Run ```cmake .. -G "Visual Studio 14 2015 Win64"```. You may have to set boost directories: ```set BOOST_ROOT=your_boost_root_directory``` and ```set BOOST_LIBRARYDIR=your_boost_library_directory``` first
4. Open the solution and build the release version of it

##### Examples #####

* 1 game, max depth 1, single thread:
```bash
./bin/main --strategy data/2048_strategies/eval-function2.bin.special --uncompress false
```
* 10 games, max depth 3, uncompressed model (faster, but requires more RAM):
```bash
./bin/main --strategy data/2048_strategies/eval-function2.bin.special --games 10 --depth 3
```
* 10 games, max depth 5, uncompressed model, multithreating in expectimax (*best reasonable settings*):
```bash
./bin/main --strategy data/2048_strategies/eval-function2.bin.special --games 10 --depth 5 --eval_multithreading
```
* 100 games, max depth 5, uncompressed model, playing 4 games in pararell:
```bash
./bin/main --strategy data/2048_strategies/eval-function2.bin.special --games 100 --depth 5 --threads 4
```
* 10 games, min depth 1, max depth 8, 1 game thread, multithreading in expectimax evaluation, max time 50 ms per round, uncompressed model, prints boards' states to the console:
```bash
./bin/main --strategy data/2048_strategies/eval-function2.bin.special --games 10 --depth 8 --time 50 --eval_multithreading  -v
```

#### Running web application ####

You may need to install the *websocket-client* first:
```
pip install websocket-client
```

##### Chrome #####

1. Close all instances of Chrome (including hangouts, etc...)
2. Run the browser with remote-debugging enabled:
    ```
    chrome --remote-debugging-port=9222
    ```
3. Go to [2048 game site](http://gabrielecirulli.github.io/2048/)
4. Run python script (see examples)

##### Firefox #####

1. First you need to install [Remote Control](https://addons.mozilla.org/pl/firefox/addon/remote-control/) plugin
2. Go to [2048 game site](http://gabrielecirulli.github.io/2048/)
3. Start remote debugging (click on plugin's icon)
4. Run python script (see examples)

Web api is strongly based on the code from https://github.com/nneonneo/2048-ai.

##### Examples #####

* chrome, max depth 4, multithreading, no time limit, uncompressed model:
```
python 2048.py -b chrome --strategy data/2048_strategies/eval-function2.bin.special --depth 4 --multithreading true
```

* On Windows you will need to specify WebApi library file:
```
python 2048.py -b chrome --lib _build/lib/Release/WebApi.dll
```

# Authors #
* [Wojciech Jaśkowski](http://www.cs.put.poznan.pl/wjaskowski) (learning the evaluation function)
* Adam Szczepański (this code)
