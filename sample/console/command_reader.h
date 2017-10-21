/* ********************************************************************** */
/**
 * @brief   ESM: Command line reader (sample && test application).
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#ifndef COMMAND_READER_H_INCLUDED
#define COMMAND_READER_H_INCLUDED

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/* ---------------------------------------------------------------------- */
/* Classes */
/* ---------------------------------------------------------------------- */

/** Command line reader class. */
class CommandReader {
private:
    // Type Aliases
    using VS = std::vector<std::string>;
    using CT = std::char_traits<char>;

    // Constants
    static const std::string CMD_SEPS;

    // Variables - initialized only once
    std::istream& m_fin;
    std::string m_comment_character;

    // Variables
    std::uintmax_t m_line_number;
    VS m_argv;

    // Return true if [c] is a command separator character.
    static bool iscmdsep(const char c) {
        return std::any_of(CMD_SEPS.cbegin(), CMD_SEPS.cend(), [&c](char elem) {
            return elem == c;
        });
    }

    // Consume command separator characters.
    static void consume_cmdsep(std::istream& in) {
        CT::int_type c;

        while ((c = in.peek()) != CT::eof()) {
            if (!iscmdsep(CT::to_char_type(c))) {
                break;
            }
            in.ignore();
        }
    }

    // Consume characters until reach to end of line.
    static void consume_until_end_of_line(std::istream& in) {
        CT::int_type c;

        while ((c = in.get()) != CT::eof()) {
            if (CT::to_char_type(c) == '\n') {
                (void) in.peek();
                break;
            }
        }
    }

    // Read characters from an input stream.
    static bool read_line(std::istream& in, std::string& line) {
        if (in.eof()) {
            return false;
        }
        std::getline(in, line);
        if ((!in) && (line.size() >= line.max_size())) {
            // Probably huge line
            in.clear();
            consume_until_end_of_line(in);
            return false;
        }
        return true;
    }

    // Return true if [line] is a comment line.
    static bool recognize_comment_line(const std::string& line,
                                       const std::string& comment_character) {
        return line.substr(0, comment_character.size()) == comment_character;
    }

    // Tokenize a string.
    static void split_line(std::string& line, VS& argv) {
        std::istringstream iss(line);
        std::string arg;

        argv.clear();
        while (iss >> arg) {
            argv.push_back(arg);
        }
    }

public:
    CommandReader(std::istream& in = std::cin,
                  const std::string& comment_character = "#") :
        m_fin(in),
        m_comment_character(comment_character),
        m_line_number { 0 } {};
    virtual ~CommandReader() = default;
    CommandReader(const CommandReader&) = delete;
    CommandReader& operator=(const CommandReader&) = delete;

    // Read command line inputs and tokenize it.
    bool read() {
        for (;;) {
            auto& in = m_fin;

            consume_cmdsep(in);
            if (in.eof()) {
                return false;
            }

            std::string line;
            if (!read_line(in, line)) {
                if (line.empty()) {
                    return false;
                }
                // XXX: Currently ignore huge line input.
            }
            m_line_number++;

            if (line.empty()) {
                continue;
            }
            if (recognize_comment_line(line, m_comment_character)) {
                continue;
            }

            split_line(line, m_argv);
            assert(!m_argv.empty());
            return true;
        }
    }

    bool eof() const {
        return m_fin.eof();
    }

    std::uintmax_t lineNumber() const {
        return m_line_number;
    }

    VS::size_type argc() const {
        return m_argv.size();
    }

    const std::string& argv(const VS::size_type index) const {
        return m_argv.at(index);
    }

    const VS& argv() const {
        return m_argv;
    }
};

const std::string CommandReader::CMD_SEPS{" \f\r\t\v"};

#endif /* ndef COMMAND_READER_H_INCLUDED */
