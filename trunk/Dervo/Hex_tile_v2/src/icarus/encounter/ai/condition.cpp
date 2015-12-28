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

status::value condition::execute()
{
    if (CALL_MEMBER_FN(class_, function_)())
        return status::success;
    return status::failed;
}
}   //namespace encounter
}   //namespace ai
}   //namespace icarus

