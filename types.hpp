#ifndef types_hpp
#define types_hpp

#include <set>
#include <tuple>
#include <vector>

#include <torch/torch.h>


using Board = torch::Tensor;
using Action = typename std::array<int, 2>;
using ID = typename std::vector<Action>;

using Reward = torch::Tensor;

using Positions = typename std::set<Action>;
using State = typename std::tuple<ID, Board, Positions>;



#endif // types_hpp
