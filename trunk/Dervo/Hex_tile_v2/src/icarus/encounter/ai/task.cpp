
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

status::value task::execute()
{
    return (CALL_MEMBER_FN(class_, function_)());
}
}   //namespace encounter
}   //namespace ai
}   //namespace icarus

