#ifndef ICARUS_AI_HPP_
#define ICARUS_AI_HPP_

namespace icarus
{
namespace ai
{
class behavior_node;
namespace status
{
enum value
{
    success,
    running,
    failed,
    error
};
}
struct node_result
{
    status::value result_;
    behavior_node* next_;
    bool down_;
};
}   // namespace ai
}   // namespace icarus

#endif // ICARUS_AI_HPP_

