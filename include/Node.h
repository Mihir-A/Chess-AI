#pragma once
#include "Move.h"

#include <vector>

class Node
{
public:
    Node() = default;

    Node(const Move &t)
    {
        this->t = t;
    }

    void addChild(const Move &t)
    {
        this->children.emplace_back(t);
    }

    // remove a child by value, note: if the node has multiple children with the same value, this will only delete the first child
    void removeChild(const Move &t)
    {
        for (int i = 0; i < this->children.size(); ++i) {
            if (this->children.at(i).t == t) {
                this->children.erase(this->children.begin() + i);
                return;
            }
        }
    }

    void removeChildByIndex(const int index)
    {
        this->children.erase(this->children.begin() + index);
    }

    void setValue(const Move &t)
    {
        this->t = t;
    }

    Move& getValue()
    {
        return this->t;
    }

    const Move& getValue() const
    {
        return this->t;
    }

    std::vector<Node>& getChildren()
    {
        return this->children;
    }

    const std::vector<Node>& getChildren() const
    {
        return this->children;
    }


private:
    Move t;
    std::vector<Node> children;
};
