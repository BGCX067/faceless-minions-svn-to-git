#include "icarus/utilities/yth_handler.hpp"
namespace icarus
{
namespace utilities
{

yth_node::yth_node(const std::string& key, const std::string& value)
{
    key_ = key;
    value_ = value;
}

const std::string& yth_node::key() const
{
    return key_;
}

const std::string yth_node::value() const
{
    return value_;
}

yth_node* yth_node::child(const unsigned index) const
{
    if(index < children_.size())
        return children_[index];

    return NULL;
}

yth_node* yth_node::child(const std::string& key, const unsigned instance) const
{
    unsigned current = 0;

    for(unsigned i = 0; i < children_.size(); ++i)
        if(children_[i]->key() == key)
            if(instance == current++)
                return children_[i];

    return NULL;
}

void yth_node::child_add(yth_node* const child)
{
    children_.push_back(child);
}

unsigned yth_node::child_count() const
{
    return children_.size();
}

unsigned yth_node::child_count(const std::string& key) const
{
    unsigned count = 0;

    for(unsigned i = 0; i < children_.size(); ++i)
        if(children_[i]->key() == key)
            ++count;

    return count;
}

yth_node::~yth_node()
{

}

yth_handler::yth_handler()
{
    //ctor
}

bool yth_handler::load(const std::string filename)
{
    //freeing function needed, cleanup!
    nodes_ = new yth_node("root");

    std::ifstream current_yth;

    current_yth.open(filename.c_str());
    if(!current_yth.good())
    {
        return 1;
    }

    std::stack<yth_node*> parents_;
    std::string line_;
    unsigned line_count = 0;

    do
    {
        std::getline(current_yth, line_);
        line_count++;
        if(!line_.length())
            continue;

        if((int)line_.find("\n") != -1)
            line_.erase(line_.find("\n", 1));

        if((int)line_.find("#") != -1)
            if((line_.find("#") < line_.find("<")) ||
               (line_.find("#") > line_.find(">")))
               line_.erase((line_.find("#")));

        if(!line_.length())
            continue;

        int tag_start   = line_.find("<");
        int tag_end     = line_.find(">", tag_start);

        if(tag_end > (tag_start > -1))
        {
            if((int)line_.find("</") != -1 && parents_.size())
            {
                if(line_.substr(line_.find("</") + 2,
                    (tag_end - 2) - tag_start) == parents_.top()->key())
                {
                    parents_.pop();
                    continue;
                }
                std::cout << "Syntax error: Tag " << parents_.top()->key()
                << " not properly closed on line " << line_count << " in file "
                << filename << "!" << std::endl;
                return 1;
            }

                bool line_tag_      = ((int)line_.find("/>", tag_start) != -1);

    std::string key     = line_.substr(tag_start + 1, (((int)line_.find(":",tag_start)
                            != -1) ? (int)line_.find(":", tag_start) : tag_end) - (tag_start+1));
    std::string value   = (int)line_.find(":", tag_start) != -1 ? line_.substr(line_.find(":", tag_start) + 1, (tag_end - (line_tag_ ? 2: 1)) - line_.find(":", tag_start)) : "";

                yth_node* nnode = new yth_node(key, value);

                //if((!parents_.empty() ? nodes_->child_add(nnode) : parents_.top()->child_add(nnode)) && !line_tag_) parents_.push(nnode);
                //Yuki code returns boolean value upon insertion, ours does not.

                if(parents_.empty())
                {
                    nodes_->child_add(nnode);
                }
                else
                {
                    parents_.top()->child_add(nnode);
                }
                if(!line_tag_)
                {
                    parents_.push(nnode);
                }
        }
    }
    while(!current_yth.eof());

    current_yth.close();
    return true;
}

yth_handler::~yth_handler()
{
    //dtor
}

yth_node* yth_handler::node(const unsigned index) const
{
    if(index< nodes_->child_count())
        return nodes_->child(index);

    return NULL;
}

yth_node* yth_handler::node(const std::string& key, const unsigned instance) const
{
    if(instance < nodes_->child_count(key))
        return nodes_->child(key, instance);

    return NULL;
}

unsigned yth_handler::node_count() const
{
    return nodes_->child_count();
}

unsigned yth_handler::node_count(const std::string& key) const
{
    return nodes_->child_count(key);
}
}//namespace utilites
}//namespace icarus
