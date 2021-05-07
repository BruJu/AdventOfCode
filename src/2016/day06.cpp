#include "../advent_of_code.hpp"
#include "../util/regex_helper.hpp"

// https://adventofcode.com/2016/day/6

// Find the most / least frequent letter at a given position for different words

namespace {
    enum class Criterion {
        MostFrequent,
        LeastFrequent
    };

    struct LetterReader {
        std::map<char, size_t> occurrences;

        void increment(char c) {
            occurrences[c] += 1;
        }

        [[nodiscard]] char get(Criterion criterion) const {
            const auto is_better = [criterion](size_t current, size_t candidate) {
                if (criterion == Criterion::MostFrequent) {
                    return current < candidate;
                } else {
                    return current == 0 || current > candidate;
                }
            };

            char letter = '\0';
            size_t occ = 0;

            for (const auto [candidate, cand_occ] : occurrences) {               
                if (is_better(occ, cand_occ)) {
                    letter = candidate;
                    occ = cand_occ;
                }
            }

            return letter;
        }
    };

    struct WordReader {
        std::vector<LetterReader> letter_readers;

        void read(const std::string & word) {
            while (word.size() > letter_readers.size()) {
                letter_readers.push_back(LetterReader{});
            }

            for (size_t i = 0 ; i < word.size() ; ++i) {
                letter_readers[i].increment(word[i]);
            }
        }

        std::string get(Criterion criterion) const {
            std::string acc;

            for (const auto & letter_reader : letter_readers) {
                acc += letter_reader.get(criterion);
            }

            return acc;
        }
    };
}

Output day_2016_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
    WordReader word_reader;

    for (const auto & line : lines) {
        word_reader.read(line);
    }

    return Output(
        word_reader.get(Criterion::MostFrequent),
        word_reader.get(Criterion::LeastFrequent)
    );
}
