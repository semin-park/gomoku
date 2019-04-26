#ifndef types_hpp
#define types_hpp

#include <set>
#include <tuple>
#include <vector>

#include "xtensor/xtensor.hpp"


using Board = xt::xtensor<int, 3>;
using Action = typename std::array<int, 2>;
using ID = typename std::vector<Action>;

using Reward = xt::xtensor<float, 1>;
using Policy = xt::xtensor<float, 2>;

using Positions = typename std::set<Action>;
using State = typename std::tuple<ID, Board, Positions>;



#endif // types_hpp
