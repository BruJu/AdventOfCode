#include <regex>
#include <string>
#include <span>

namespace bj {
    /**
     * A class that can take pairs of regex - builder of objets of type T
     * and then build Ts from strings.
     *
     * This is used to code the logic of reading a file with instructions lines
     * by lines, and only having to use the `add_handler` function to write
     * the correct regex and the associated constructor.
     *
     * Example of usage can be found on day 2016-08
     */
    template<typename T>
    class InstructionReader {
    public:
        /** Type of functions that can convert a regex decoded string into a T */
        using Handler = T(const std::vector<std::string> &);

    private:
        /** Individual handler, with the regex and the handler */
        class RegexConverterElement {
        private:
            std::regex m_regex;
            Handler * m_handler;

        public:
            RegexConverterElement(const char * regex, Handler * handler)
                : m_regex(regex), m_handler(handler) {}

            std::optional<T> operator()(const std::string & str) const {
                std::smatch smatch;
                const bool r = std::regex_search(str, smatch, m_regex);

                if (!r) return std::nullopt;

                std::vector<std::string> values;

                for (int i = 1 ; i != smatch.length() ; ++i) {
                    values.emplace_back(smatch[i].str());
                }

                return m_handler(values);
            }
        };

    private:
        std::vector<RegexConverterElement> m_handlers;

    public:
        /** Adds a handler for a regex, with the given constructor */
        void add_handler(const char * regex, Handler * handler) {
            m_handlers.emplace_back(regex, handler);
        }

        /** If a T can be built from the string, build it and return it */
        std::optional<T> operator()(const std::string & str) const {
            for (const auto & h : m_handlers) {
                const auto e = h(str);
                if (e) return e;
            }

            return std::nullopt;
        }

        /**
         * If Ts can be built from all the strings in the vector, return a
         * vector of these Ts. Else, return std::nullopt and write in std::cerr.
         */
        std::optional<std::vector<T>> operator()(std::span<const std::string> lines) const {
            std::vector<T> elements;

            for (const std::string & line : lines) {
                const auto opt_e = this->operator()(line);
                if (!opt_e) {
                    std::cerr << "Bad input\n" << line << "\n";
                    return std::nullopt;
                }
                elements.emplace_back(std::move(opt_e.value()));
            }

            return elements;
        }
    };
}
