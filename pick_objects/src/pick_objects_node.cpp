#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int goToGoal(MoveBaseClient &ac, move_base_msgs::MoveBaseGoal &goal) {
    // Sends the goal to the action server
    ROS_INFO("Sending goal");
    ac.sendGoal(goal);

    // Wait an infinite time for the action server to finish performing the action
    ac.waitForResult();

    // Check to see if the result is a success
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Hooray, the base moved to the goal");
        ROS_INFO("Goal position: %f, %f", goal.target_pose.pose.position.x, goal.target_pose.pose.position.y);
        return 0;
    }
    else {
        ROS_INFO("The base failed to move to the goal");
        return -1;
    }

    return -1;
}

int main(int argc, char** argv){
    ros::init(argc, argv, "pick_objects");
    ros::NodeHandle nh;
    std::string node_name = ros::this_node::getName();

    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);

    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){
        ROS_INFO("Waiting for the move_base action server to come up");
    }

    move_base_msgs::MoveBaseGoal goal;

    //we'll send a goal to the robot to move 1 meter forward
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();

    
    // Define a pickup position
    nh.getParam(node_name + "/pickup_point_x", goal.target_pose.pose.position.x);
    nh.getParam(node_name + "/pickup_point_y", goal.target_pose.pose.position.y);

    goal.target_pose.pose.orientation.w = 1.0;

    ros::Duration(5.0).sleep();

    goToGoal(ac, goal);

    // wait for 5 seconds
    ros::Duration(5.0).sleep();

    // Define a dropoff position
    nh.getParam(node_name + "/dropoff_point_x", goal.target_pose.pose.position.x);
    nh.getParam(node_name + "/dropoff_point_y", goal.target_pose.pose.position.y);

    goToGoal(ac, goal);
    // wait for 5 seconds
    ros::Duration(5.0).sleep();

    return 0;
}
