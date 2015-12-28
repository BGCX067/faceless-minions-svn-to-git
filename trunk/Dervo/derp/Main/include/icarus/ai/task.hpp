#ifndef ICARUS_AI_TASK_HPP_
#define ICARUS_AI_TASK_HPP_

#include "icarus/ai.hpp"

namespace icarus
{
namespace ai
{
class task
{
public:
    task() {}
    virtual ~task() = 0;
    virtual node_result execute(node_result prev_result) = 0;
};
}   // namespace ai
}   // namespace icarus

#endif // ICARUS_AI_TASK_HPP_