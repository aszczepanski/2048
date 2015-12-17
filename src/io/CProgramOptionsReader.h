#ifndef SRC_IO_CPROGRAM_OPTIONS_READER_H_
#define SRC_IO_CPROGRAM_OPTIONS_READER_H_

#include "io/IProgramOptionsReader.h"

#include <memory>
#include <string>

#include "common/ProgramOptions.h"

class CProgramOptionsReader : public IProgramOptionsReader {
public:
	virtual std::shared_ptr<ProgramOptions> read(int argc, char* argv[]);

private:
	static const std::string DEFAULT_STRATEGY_VALUE;
	static const unsigned DEFAULT_GAMES_VALUE;
	static const unsigned DEFAULT_TIME_VALUE;
	static const unsigned DEFAULT_DEPTH_VALUE;
	static const unsigned DEFAULT_GAME_THREADS_VALUE;
};

#endif  // SRC_IO_CPROGRAM_OPTIONS_READER_H_
