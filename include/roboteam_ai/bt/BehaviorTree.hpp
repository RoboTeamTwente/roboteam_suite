#pragma once

#include "Blackboard.hpp"
#include "Node.hpp"
#include "mutex"

namespace bt {

class BehaviorTree : public Node {
    FRIEND_TEST(BehaviorTreeTest, it_sets_blackboards);

   public:
    std::string name;
    using Ptr = std::shared_ptr<BehaviorTree>;

    BehaviorTree();

    BehaviorTree(std::string name);

    explicit BehaviorTree(const Node::Ptr &rootNode);

    explicit BehaviorTree(const Blackboard::Ptr &shared);

    std::string node_name() override;

    Status update() override;

    void terminate(Status s) override;

    void SetRoot(const Node::Ptr &node);

    Node::Ptr GetRoot();

    void SetGlobalBlackboard(const Blackboard::Ptr &globalBB);

   private:
    Node::Ptr root = nullptr;
    Blackboard::Ptr globalBB = nullptr;
};

}  // namespace bt
