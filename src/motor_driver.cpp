// /*
//  * Author: Modou Dibba
//  * Year: 2022
//  *
//  */

// #include "motor_driver.hpp"

// namespace SkippyRobot {

//     RobotDriver::RobotDriver(ros::NodeHandle *nh){}
//     RobotDriver::~RobotDriver() {}

//     int main(int argc, char **argv){}

// } // namespace SkippyRobot



// // Topic messages callback
// void twistCallback(const geometry_msgs::Twist& msg)
// {
//     double leftmotor;
//     double rightmotor;
//     char velMsg[256];


//     if((abs(msg.linear.x) <= 1) && (abs(msg.angular.z) <= 2))
//     {
//         rightmotor = msg.linear.x + (0.5 * msg.angular.z);
//         leftmotor = msg.linear.x - (0.5 * msg.angular.z);

//         rightmotor = boost::algorithm::clamp(round(rightmotor*127 + 127), 0, 254);
//         leftmotor = boost::algorithm::clamp(round(leftmotor*127 + 127), 0, 254);


//         uint8_t r_motor = static_cast<uint8_t>(rightmotor);
//         uint8_t l_motor = static_cast<uint8_t>(leftmotor);

//         uint8_t start_byte = 253;
//         uint8_t mod_byte = 255;

//         uint8_t checksum = (((start_byte - (r_motor + l_motor)) % mod_byte) + mod_byte) % mod_byte;



//         // sprintf(velMsg, "jx%dz%dy", r_motor, l_motor);
//         sprintf(velMsg, "%d@%d#%d&%d\n", start_byte, l_motor, r_motor, checksum);
//         ser.write(velMsg);

//         ROS_INFO("Linear velocity %3.2f    Angualar velocity %3.2f", msg.linear.x, msg.angular.z);
//         // ROS_INFO("Start byte: %d   Right motor:%d    Left motor:%d  Checksum:%d", start_byte, r_motor, l_motor, checksum);
//         // ROS_INFO("checksum %d",checksum);
//     }
// }
// ////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////End function callback/////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////




// /////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////Begin main function//////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////
// int main(int argc, char **argv)
// {
//     // Initiate a new ROS node named "listener"
// 	ros::init(argc, argv, "motor_driver_node");
// 	//create a node handle: it is reference assigned to a new node
// 	ros::NodeHandle node;


//     // Subscribe to a given topic, in this case "chatter".
// 	//chatterCallback: is the name of the callback function that will be executed each time a message is received.
//     ros::Subscriber cmdVel_sub = node.subscribe("/cmd_vel", 1000, twistCallback);


//     try
//     {
//         ser.setPort("/dev/ttyACM0");
//         ser.setBaudrate(115200);
//         serial::Timeout to = serial::Timeout::simpleTimeout(1000);
//         ser.setTimeout(to);
//         ser.open();
//     }
//     catch (serial::IOException& e)
//     {
//         ROS_ERROR_STREAM("Unable to open port ");
//         return -1;
//     }

//     if(ser.isOpen()){
//         ROS_INFO_STREAM("Serial Port initialized");
//     }else{
//         return -1;
//     }

//     ros::Rate loop_rate(5);
//     while(ros::ok()){

//         ros::spinOnce();
//         // encoders();

//          if(ser.available()){
//             ROS_INFO_STREAM("Reading from serial port");
//             std::string result;
//             result = ser.read(ser.available());
//             ROS_INFO_STREAM("Read: " << result);
//         }   

//     // Enter a loop, pumping callbacks
//     //ros::spin();

//         loop_rate.sleep();
//     }
//     return 0;
// }
// /////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////End main function//////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Author: Modou Dibba
 * Year: 2022
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Program includes///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Bool.h"
#include <serial/serial.h>
#include <boost/algorithm/clamp.hpp>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Global variables////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
serial::Serial ser;
std::string estop_trigger_topic_;
bool estop_state_ = false;
uint8_t brake = 0;

///////////////////////////////////////////////////////////////////////////////////////////
// Topic messages callback
void twistCallback(const geometry_msgs::Twist& msg)
{
    double leftmotor;
    double rightmotor;
    char velMsg[256];


    if((abs(msg.linear.x) <= 1) && (abs(msg.angular.z) <= 2))
    {
        rightmotor = msg.linear.x + (0.5 * msg.angular.z);
        leftmotor = msg.linear.x - (0.5 * msg.angular.z);

        rightmotor = boost::algorithm::clamp(round(rightmotor*127 + 127), 0, 254);
        leftmotor = boost::algorithm::clamp(round(leftmotor*127 + 127), 0, 254);


        uint8_t r_motor = static_cast<uint8_t>(rightmotor);
        uint8_t l_motor = static_cast<uint8_t>(leftmotor);

        uint8_t start_byte = 253;
        uint8_t mod_byte = 255;

        uint8_t checksum = (((start_byte - (r_motor + l_motor)) % mod_byte) + mod_byte) % mod_byte;


        
        sprintf(velMsg, "%d@%d#%d&%d^%d\n", start_byte, l_motor, r_motor, brake, checksum);
        ser.write(velMsg);


        // ROS_INFO("Linear velocity %3.2f    Angualar velocity %3.2f", msg.linear.x, msg.angular.z);
        ROS_INFO("Start byte: %d   Right motor:%d    Left motor:%d  Brakes:%d Checksum:%d", start_byte, r_motor, l_motor, brake, checksum);

        
    }
}
// braking motion
void estopCallback(const std_msgs::Bool::ConstPtr &msg) {

    if (msg->data == true) 
    {
        // estop_state_ = true;
        brake = 1;
    }
}

// release brake
void estopCallbackReset(const std_msgs::Bool::ConstPtr &msg) {

    if (msg->data == true) 
    {
        // estop_state_ = true;
        brake = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////End function callback/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Begin main function//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    // Initiate a new ROS node named "listener"
	ros::init(argc, argv, "motor_driver_node");
	//create a node handle: it is reference assigned to a new node
	ros::NodeHandle node;


    // Subscribe to a given topic, in this case "chatter".
	//chatterCallback: is the name of the callback function that will be executed each time a message is received.
    ros::Subscriber cmdVel_sub = node.subscribe("/cmd_vel", 1000, twistCallback);
    ros::Subscriber estop_sub = node.subscribe("estop_trigger_topic_", 10, estopCallback);
    ros::Subscriber estopreset_sub = node.subscribe("estop_trigger_reset_topic_", 10, estopCallbackReset);


    try
    {
        ser.setPort("/dev/ttyACM0");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    if(ser.isOpen()){
        ROS_INFO_STREAM("Serial Port initialized");
    }else{
        return -1;
    }

    ros::Rate loop_rate(5);
    while(ros::ok()){

        ros::spinOnce();

         if(ser.available()){
            // ROS_INFO_STREAM("Reading from serial port");
            std::string result;
            result = ser.read(ser.available());
            ROS_INFO_STREAM("Reading: " << result);
        }   

    // Enter a loop, pumping callbacks
    //ros::spin();

        loop_rate.sleep();
    }
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////End main function//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

