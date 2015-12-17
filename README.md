# 2048 AI #

Artificial Intelligence for 2048 game. There are 2 applications:

* **console application** - for running multiple games and testing the AI's capabilities
* **web application** - for observing how the AI works on the [2048 game site](http://gabrielecirulli.github.io/2048/)

## Requirements ##

In order to use the AI you will need to build main application. The requirements are listed below:

* C++11 compiler
* CMake 3.0+
* Boost 1.49.0+ (program_options, accumulators)

If you want to use the web application you will need:

* Python 2
* Chrome or Firefox

Tested on:

* Mac OS X 10.11.1, Apple LLVM version 7.0.2
* Ubuntu 15.04 64-bit, g++ 4.9.2
* Windows 7 64-bit, Visual Studio 14 2015

## Strategy files ##

In order to run the application you need to have a strategy file. Some basic (not very good, but small) strategies are already in ```data/2048_strategies/``` directory. We highly encourage you to download and unzip an [advanced strategy](http://www.cs.put.poznan.pl/wjaskowski/pub/2048/eval-function.bin.special.zip).

Some strategy files are compressed - that's our way to save some memory. You can either use compressed version or decompress them and speed up calculations. Note that the decompressed version (of the above strategy) requires about 5GB of RAM whereas the compressed version requires about 4GB of RAM.

You can specify the strategy file and unzip option using command line arguments (see Program parameters section).

## Program parameters ##

#### Console application ####

+ **--strategy arg** - strategy input file (by default data/2048_strategies/2048_a_weak_player.bin.txt)
+ **--unzip** - unzip compressed strategy file
+ **--seed arg** - random seed (by default based on time elapsed since epoch)
+ **--games arg** - number of games (by default 1)
+ **--time arg** - maximum time for one round [ms], 0 means no time limit (by default 0)
+ **--depth arg** - maximum depth for expectimax (by default 1)
+ **--threads arg** - number of threads (each thread plays different game, by default 1)
+ **--eval_multithreading** - enable multithreading in expectimax algorithm
+ **-h [ --help ]** - produce help message
+ **-v [ --verbose ]** - show board and score after each round

#### Web Application ####

+ **-b [ --browser ]** - choose browser (Chrome or Firefox, by default Firefox)
+ **-p [ --port PORT ]** - port number to control on (default: 32000 for Firefox, 9222 for Chrome)
+ **--strategy arg** - strategy input file (by default data/2048_strategies/2048_a_weak_player.bin.txt)
+ **--unzip arg** - unzip strategy input file (true/false, by default false)
+ **--time arg** - maximum time for one round [ms], 0 means no time limit (by default 0)
+ **--depth arg** - maximum depth for expectimax (by default 1)
+ **--multithreading arg** - enable multithreading (true/false, by default true)

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

* 1 game, max depth 1, single thread, no time limit:
```bash
./bin/main
```
* 1000 games, max depth 1, 1 game thread, no time limit, compressed model:
```bash
./bin/main --strategy data/2048_strategies/eval-function.bin.special --games 1000 --depth 1
```
* 100 games, max depth 3, 4 game threads, no time limit, decompressed model:
```bash
./bin/main --strategy data/2048_strategies/eval-function.bin.special --games 100 --depth 3 --unzip --threads 4
```
* 10 games, min depth 1, max depth 8, 1 game thread, multithreading in expectimax evaluation, max time 50 ms per round, decompressed model, prints boards' states to the console:
```bash
./bin/main --strategy data/2048_strategies/eval-function.bin.special --games 8 --depth 8 --time 50 --eval_multithreading --unzip -v
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

1. First you need to install [Remote Control](https://addons.mozilla.org/pl/firefox/addon/remote-control/) plugin and start remote debugging (click on plugin's icon)
2. Go to [2048 game site](http://gabrielecirulli.github.io/2048/)
3. Run python script (see examples)

Note that web api is strongly based on https://github.com/nneonneo/2048-ai.

##### Examples #####

* chrome, max depth 1, single threading, no time limit, compressed model:
```
python 2048.py -b chrome
```

* chrome, max depth 4, multithreading, no time limit, decompressed model:
```
python 2048.py -b chrome --strategy data/2048_strategies/eval-function.bin.special --unzip true --depth 4 --multithreading true
```

On Windows you will need to specify WebApi library file:
```
python 2048.py -b chrome --lib _build/lib/Release/WebApi.dll
```
