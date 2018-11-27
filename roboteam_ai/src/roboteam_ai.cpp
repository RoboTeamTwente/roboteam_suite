#include "ros/ros.h"
#include "dangerfinder/DangerFinder.h"
#include "io/IOManager.h"
#include "utilities/Referee.hpp"
#include "utilities/StrategyManager.h"
#include "treeinterp/BTFactory.h"
#include "interface/Interface.h"
#include "interface/mainWindow.h"
#include "roboteam_ai/src/interface/visualizer.h"

#include <QApplication>

namespace df = rtt::ai::dangerfinder;
namespace io = rtt::ai::io;
namespace ai = rtt::ai;

using Status = bt::Node::Status;


void runBehaviourTrees() {
    // init IOManager and subscribe to all topics immediately
    io::IOManager IOManager(true);

    roboteam_msgs::World worldMsg;
    roboteam_msgs::GeometryData geometryMsg;
    roboteam_msgs::RefereeData refereeMsg;

    bt::BehaviorTree::Ptr strategy;

    // Where we keep our trees
    auto factory = BTFactory::getFactory();
    factory.init();

    // Start running this tree first
    std::string currentTree = "victoryDanceStrategy";

    ros::Rate rate(50);

    // Main loop
    while (ros::ok()) {
        ros::spinOnce();

        // make ROS world_state and geometry data globally accessible
        worldMsg = IOManager.getWorldState();
        geometryMsg = IOManager.getGeometryData();
        refereeMsg = IOManager.getRefereeData();
        ai::World::set_world(worldMsg);
        ai::Field::set_field(geometryMsg.field);
        ai::Referee::setRefereeData(refereeMsg);

        if (!ai::World::didReceiveFirstWorld) continue;

        if (df::DangerFinder::instance().hasCalculated()) {
            df::DangerData dangerData = df::DangerFinder::instance().getMostRecentData();
        }

        // for refereedata:
        if (! ai::World::didReceiveFirstWorld) {
            ROS_ERROR("No first world");
            ros::Duration(0.2).sleep();
            continue;
        }

        // for referee_data:
        // ai::StrategyManager strategyManager;
        // std::string strategyName = strategyManager.getCurrentStrategyName();
        // strategy = factory.getTree(strategyName);

        strategy = factory.getTree(currentTree);

        Status status = strategy->tick();

        switch (status) {

        case Status::Running:
            break;

        case Status::Success:
            ROS_INFO_STREAM("Status returned: Success");
            ROS_INFO_STREAM(" === TREE CHANGE === ");
            currentTree = "ParallelSequenceStrategy";
            break;

        case Status::Failure:
            ROS_INFO_STREAM("Status returned: Failure");
            break;

        case Status::Invalid:
            ROS_INFO_STREAM("Status returned: Invalid");
            break;

        }
        rate.sleep();
    }

    // Terminate if needed
    if (strategy->getStatus() == Status::Running) {
        strategy->terminate(Status::Running);
    }
}


int main(int argc, char* argv[]) {
    // Init ROS node in main thread
    ros::init(argc, argv, "StrategyNode");

    // start the ros loop in seperate thread
    std::thread behaviourTreeThread = std::thread(&runBehaviourTrees);

    // initialize the interface
    QApplication a(argc, argv);
    ui::MainWindow w;
    w.show();
    Widget wi;
    wi.show();

    return a.exec();
}
