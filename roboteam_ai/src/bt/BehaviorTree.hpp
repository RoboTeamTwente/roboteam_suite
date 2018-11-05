#pragma once

#include "Node.hpp"
#include "Blackboard.hpp"

namespace bt {

class BehaviorTree : public Node {
    public:
        BehaviorTree();

        BehaviorTree(const Node::Ptr &rootNode);

        BehaviorTree(const Blackboard::Ptr &shared);

        ~BehaviorTree();

        Status Update() override;

        void Terminate(Status s) override;

        void SetRoot(const Node::Ptr &node);

        Node::Ptr GetRoot();

        void SetGlobalBlackboard(const Blackboard::Ptr &globalBB);

    private:
        Node::Ptr root = nullptr;
        Blackboard::Ptr globalBB = nullptr;


};

} // bt
