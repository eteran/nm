
#include "Reader.h"

/**
 * @brief Reader::Reader
 * @param input
 */
Reader::Reader(string_view input) noexcept
	: input_(input) {
}

/**
 * @brief Reader::peek
 * @return
 */
char Reader::peek() const noexcept {

	if (eof()) {
		return '\0';
	}

	return input_[index_];
}

/**
 * @brief Reader::read
 * @return
 */
char Reader::read() noexcept {

	if (eof()) {
		return '\0';
	}

	char ch = input_[index_++];

	switch (ch) {
	case '\n':
		++line_;
		column_ = 0;
		break;
	default:
		++column_;
	}

	return ch;
}

/**
 * @brief Reader::eof
 * @return
 */
bool Reader::eof() const noexcept {
	return index_ == input_.size();
}

/**
 * @brief Reader::consume
 * @param chars
 */
size_t Reader::consume(string_view chars) noexcept {

	// consume while the next character is in the input set
	return consume_while([chars](char ch) {
		return chars.find(ch) != std::string::npos;
	});
}

/**
 * @brief Reader::consume_whitespace
 */
size_t Reader::consume_whitespace() noexcept {

	// consume while the next character is whitespace
	return consume_while([](char ch) {
		return (ch == ' ' || ch == '\t');
	});
}

/**
 * @brief Reader::match_any
 * @param match
 * @return
 */
auto Reader::match_any() -> optional<std::string> {
	std::string m;
	while (!eof()) {
		m.push_back(read());
	}

	if (!m.empty()) {
		return m;
	}

	return {};
}

/**
 * @brief Reader::match
 * @param regex
 * @return
 */
auto Reader::match(const std::regex &regex) -> optional<std::string> {

	std::cmatch matches;

	const char *first = &input_[index_];
	const char *last  = &input_[input_.size()];

	if (std::regex_search(first, last, matches, regex, std::regex_constants::match_continuous)) {
		std::string m = std::string(matches[0].first, matches[0].second);
		column_ += m.size();
		index_ += m.size();
		return m;
	}

	return {};
}

/**
 * @brief Reader::match
 * @param s
 * @return
 */
bool Reader::match(string_view s) noexcept {

	if (index_ + s.size() > input_.size()) {
		return false;
	}

	size_t new_index_  = index_ + s.size();
	size_t new_line_   = line_;
	size_t new_column_ = column_;

	for (size_t i = 0; i < s.size(); ++i) {
		const char ch = input_[index_ + i];
		if (ch != s[i]) {
			return false;
		}

		if (ch == '\n') {
			new_column_ = 0;
			++new_line_;
		} else {
			++new_column_;
		}
	}

	line_   = new_line_;
	column_ = new_column_;
	index_  = new_index_;
	return true;
}

/**
 * @brief Reader::match
 * @param ch
 * @return
 */
bool Reader::match(char ch) noexcept {

	if (peek() != ch) {
		return false;
	}

	if (ch == '\n') {
		column_ = 0;
		++line_;
	} else {
		++column_;
	}

	++index_;
	return true;
}

/**
 * @brief Reader::index
 * @return
 */
size_t Reader::index() const noexcept {
	return index_;
}

/**
 * @brief Reader::line
 * @return
 */
size_t Reader::line() const noexcept {
	return line_;
}

/**
 * @brief Reader::column
 * @return
 */
size_t Reader::column() const noexcept {
	return column_;
}
