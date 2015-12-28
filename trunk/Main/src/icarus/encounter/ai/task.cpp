
#include "icarus/encounter/ai/task.hpp"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace icarus
{
namespace ai
{
namespace encounter
{
task::~task()
{
}

node_result task::execute(node_result prev_result)
{
    node_result result;
    result.next_ = parent_node_;
    result.down_ = false;
    result.result_ = CALL_MEMBER_FN(class_, function_)();
    return result;
}
}   //namespace encounter
}   //namespace ai
}   //namespace icarus

