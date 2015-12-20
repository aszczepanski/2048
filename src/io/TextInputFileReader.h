#ifndef SRC_IO_TEXT_INPUT_FILE_READER_H_
#define SRC_IO_TEXT_INPUT_FILE_READER_H_

#include "io/IInputFileReader.h"

#include <fstream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

class TextInputFileReader : public IInputFileReader {
public:
	virtual std::shared_ptr<TuplesDescriptor> read(const std::string& strategy, bool unzip);

private:
	void initializeTuplesDescriptor(std::shared_ptr<ExpandedTuplesDescriptor>);
	bool readOpenedTextFile(std::fstream&, std::shared_ptr<ExpandedTuplesDescriptor>);
	bool readStageDescription(std::fstream&, std::shared_ptr<ExpandedTuplesDescriptor>, int s);
	bool consumeCharacter(std::fstream&, char requested);
};

#endif  // SRC_IO_TEXT_INPUT_FILE_READER_H_
