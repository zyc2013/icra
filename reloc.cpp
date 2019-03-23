#include<ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <tf/transform_broadcaster.h>
geometry_msgs::PoseStamped amcl_pose;
geometry_msgs::Twist raw_msg;
geometry_msgs::PoseWithCovarianceStamped revify_pose;
void amcl_pose_callback(const geometry_msgs::PoseStamped &pose_msg_)
{
amcl_pose.pose.position.x =pose_msg_.pose.position.x ;
amcl_pose.pose.position.y= pose_msg_.pose.position.y ;
ROS_INFO_STREAM("amcl_pose.pose.position.x="<<amcl_pose.pose.position.x<<"amcl_pose.pose.position.y"<<amcl_pose.pose.position.y);
}
void pos_callback(const geometry_msgs::Twist &msg_)
{
raw_msg.linear.x =msg_.linear.x ;
raw_msg.linear.y= msg_.linear.y ;
ROS_INFO_STREAM("msg.position.x="<<raw_msg.linear.x<<"msg.position.y"<<raw_msg.linear.y);
}

int main(int argc,char** argv)//argv没有[]
{
// ROS节点初始化
  ros::init(argc, argv, "reloc");
 // 创建节点句柄
  ros::NodeHandle n;
ros::Subscriber pose_sub_=n.subscribe("amcl_pose", 2, amcl_pose_callback);
ros::Subscriber raw_sub_=n.subscribe("raw_pos", 2, pos_callback);
ros::Publisher pose_pub=n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 1000);
double distance=(amcl_pose.pose.position.x-raw_msg.linear.x)*(amcl_pose.pose.position.x-raw_msg.linear.x)+(amcl_pose.pose.position.y-raw_msg.linear.y)*(amcl_pose.pose.position.y-raw_msg.linear.y);
//ROS_INFO_STREAM("distance"<<distance);
//ros::Rate rate(1);
while(ros::ok())
{
double distance=(amcl_pose.pose.position.x-raw_msg.linear.x)*(amcl_pose.pose.position.x-raw_msg.linear.x)+(amcl_pose.pose.position.y-raw_msg.linear.y)*(amcl_pose.pose.position.y-raw_msg.linear.y);
ROS_INFO_STREAM("distance"<<distance);
if(distance>3)
{
revify_pose.header.stamp=ros::Time::now();
revify_pose.header.frame_id="map";
revify_pose.pose.pose.position.x=raw_msg.linear.x;
revify_pose.pose.pose.position.y=raw_msg.linear.y;
revify_pose.pose.pose.orientation = tf::createQuaternionMsgFromYaw(raw_msg.linear.z);
revify_pose.pose.covariance[0] = 20;
revify_pose.pose.covariance[7] = 20;
revify_pose.pose.covariance[14] = FLT_MAX;
revify_pose.pose.covariance[21] = FLT_MAX;
revify_pose.pose.covariance[28] =FLT_MAX;
revify_pose.pose.covariance[35] = 50;
pose_pub.publish(revify_pose);
}
ros::spinOnce();
//rate.sleep();
ros::Duration(5).sleep();
}
return 0;
}
