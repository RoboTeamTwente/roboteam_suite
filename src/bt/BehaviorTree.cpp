#include "bt/BehaviorTree.hpp"

#include "bt/Node.hpp"

namespace bt {

    BehaviorTree::BehaviorTree() { globalBB = std::make_shared<Blackboard>(); }
    BehaviorTree::BehaviorTree(std::string name) {
        globalBB = std::make_shared<Blackboard>();
        this->name = name;
    }
    BehaviorTree::BehaviorTree(const Node::Ptr &rootNode) : BehaviorTree() {
        root = rootNode;
        this->root->globalBB = globalBB;
    }

    BehaviorTree::BehaviorTree(const Blackboard::Ptr &shared) : BehaviorTree() { globalBB = shared; }

    Node::Status BehaviorTree::update() { return root->tick(world, field); }

    void BehaviorTree::terminate(Status s) {
        if (root->getStatus() == Status::Running) {
            root->terminate(root->getStatus());
        }

        if (s == Node::Status::Running) {
            setStatus(Node::Status::Failure);
        }
    }

    void BehaviorTree::SetRoot(const Node::Ptr &node) {
        root = node;
        node->globalBB = globalBB;
    }

    Node::Ptr BehaviorTree::GetRoot() { return root; }

    void BehaviorTree::SetGlobalBlackboard(const Blackboard::Ptr &global) { globalBB = global; }
    std::string BehaviorTree::node_name() { return "Behaviour Tree"; }

}  // namespace bt
