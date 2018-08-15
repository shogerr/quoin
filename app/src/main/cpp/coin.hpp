#include <random>

class Coin
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::bernoulli_distribution d{.5};
public:
    bool flip() { return d(gen); }
};
