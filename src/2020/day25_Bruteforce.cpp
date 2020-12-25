#include "../advent_of_code.hpp"

// https://adventofcode.com/2020/day/25

class EncryptionKey {
private:
    long long int m_key;
    size_t m_loops = 0;

public:
    explicit EncryptionKey(long long int key, long long int subject_number = 7) : m_key(key) {
        long long int value = 1;
        while (value != m_key) {
            value = (value * subject_number) % 20201227;
            ++m_loops;
        }
    }

    [[nodiscard]] auto public_key() const noexcept { return m_key; }

    long long int operator()(long long int subject_number) const {
        long long int value = 1;
        for (size_t i = 0 ; i != m_loops ; ++i) {
            value = (value * subject_number) % 20201227;
        }
        return value;
    }

    friend std::ostream & operator<<(std::ostream & stream, const EncryptionKey & self) {
        return stream << "{ " << self.m_key << ", " << self.m_loops << " }";
    }
};

Output day_2020_25(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto keys = lines_transform::map<long long int>(lines, [](const std::string & s) { return std::stoll(s);});

    const EncryptionKey card { keys[0] };
    const EncryptionKey door { keys[1] };
    
    return Output(card(door.public_key()), 0);
}
