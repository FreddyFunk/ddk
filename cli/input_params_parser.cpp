#include <algorithm>
#include "input_params_parser.hpp"

InputParser::InputParser(int & argc, char ** argv) {
	for (int i = 1; i < argc; ++i)
		m_tokens.push_back(std::string(argv[i]));
}

const std::string & InputParser::getCmdOption(const std::string & option) const {
	std::vector<std::string>::const_iterator itr;
	itr = std::find(m_tokens.begin(), m_tokens.end(), option);
	if (itr != m_tokens.end() && ++itr != m_tokens.end()) {
		return *itr;
	}
	return std::string{};
}

bool InputParser::cmdOptionExists(const std::string & option) const {
	return std::find(m_tokens.begin(), m_tokens.end(), option)
		!= m_tokens.end();
}
