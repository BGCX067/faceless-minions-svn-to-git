#include "icarus/encounter/ai/condition.hpp"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

namespace icarus
{
namespace ai
{
namespace encounter
{
condition::~condition()
{
}

node_result condition::execute(node_result prev_result)
{
    node_result result;
    result.next_ = parent_node_;
    result.down_ = false;
    if (CALL_MEMBER_FN(class_, function_)())
    {
        result.result_ = status::success;
    }
    else
    {
        result.result_ = status::failed;
    }
    return result;
}
}   //namespace encounter
}   //namespace ai
}   //namespace icarus

