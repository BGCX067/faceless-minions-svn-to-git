#ifndef YTH_HANDLER_H
#define YTH_HANDLER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stack>

namespace icarus
{
namespace utils
{
class yth_node
{
    private:
        std::string         key_;
        std::string          value_;
        std::vector<yth_node*>  children_;

    public:
        yth_node(const std::string& key_, const std::string& value_ = "");
        virtual ~yth_node();

        const std::string& key() const;
        const std::string value() const;

        yth_node*   child(const unsigned index) const;
        yth_node*   child(const std::string& key, const unsigned instance = 0) const;

        void        child_add(yth_node* const child);
        unsigned    child_count() const;
        unsigned    child_count(const std::string& key) const;

        friend class    yth_handler;
};
class yth_handler
{
    private:
        yth_node*   nodes_;

    public:
        yth_handler();
        virtual ~yth_handler();

        bool    load(const std::string filename);

        yth_node* node(const unsigned index) const;
        yth_node* node(const std::string& key, const unsigned instance = 0) const;

        unsigned node_count() const;
        unsigned node_count(const std::string& key) const;

        //Foreach utility? Could not find implementation of this in original
        //Yuki code, leaving out for now. 14.03 Theoball.
};
}//namespace utils
}//namespace icarus

#endif // YTH_HANDLER_H
