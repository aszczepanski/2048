# 2048 #

AI for 2048 game. We created 2 apps - console application used for running multiple games and web application for observing how the AI works.

### What is this repository for? ###

* Console application
* Web API
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### How do I get set up? ###

In order to use the AI you will need to build main application. The requirements are listed below:

* C++11 compiler
* Cmake min 3.0
* Boost min 1.49.0 (program_options, accumulators)

#### Program parameters ####

##### Console application #####

+ **--strategy arg** - strategy input file (by default data/2048_strategies/2048_a_weak_player.bin.txt)
+ **--seed arg** - random seed (by default based on time elapsed since epoch)
+ **--games arg** - number of games (by default 1)
+ **--time arg** - maximum time for one round [ms], 0 means no time limit (by default 0)
+ **--depth arg** - maximum depth for expectimax (by default 1)
+ **--threads** - enable multithreading
+ **-h [ --help ]** - produce help message
+ **-v [ --verbose ]** - show board and score after each round

##### Web Application #####

+ **-b [ --browser ]** - choose browser (Chrome or Firefox, by default Firefox)
+ **-p [ --port PORT ]** - port number to control on (default: 32000 for Firefox, 9222 for Chrome)
+ **--strategy arg** - strategy input file (by default data/2048_strategies/2048_a_weak_player.bin.txt)
+ **--time arg** - maximum time for one round [ms], 0 means no time limit (by default 0)
+ **--depth arg** - maximum depth for expectimax (by default 1)
+ **--threads arg** - enable multithreading (true/false, by default true)

#### Building on Linux/Mac OS X ####

1. Go to the project root directory
2. Run ```cmake .```.
3. Run ```make```. The ```lib/``` and ```bin/``` directories will be created.

#### Building on Windows ####

TODO

#### Running console application examples ####

* 1 game, max depth 1, single thread, no time limit:
```bash
./bin/main
```
* 1000 games, max depth 1, single thread, no time limit:
```bash
./bin/main --strategy data/2048_strategies/tiles-test2-v1.bin.special --games 1000 --depth 1
```
* 100 games, max depth 3, multiple threads, no time limit:
```bash
./bin/main --strategy data/2048_strategies/tiles-test2-v1.bin.special --games 100 --depth 3 --threads
```
* 10 games, min depth 1, max depth 8, multiple threads, max time 10 ms per round:
```bash
./bin/main --strategy data/2048_strategies/tiles-test2-v1.bin.special --games 8 --depth 8 --time 10 --threads
```

#### Running web application ####

##### Chrome #####

1. Run the browser with remote-debugging enabled:
```
chrome --remote-debugging-port=9222
```
2. Go to [2048 game site](http://gabrielecirulli.github.io/2048/)
3. Run python script

TODO: examples

##### Firefox #####

1. First you need to install remote-debugging plugin (i.e. ...TODO)
2. Go to [2048 game site](http://gabrielecirulli.github.io/2048/)
3. Run python script

##### Running python script examples #####

* max depth 1, single thread, no time limit
```
python 2048.py
```

* max depth 4, multiple threads, no time limit
```
python 2048.py --depth 4 --threads true
```

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact