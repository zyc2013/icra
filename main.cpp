#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <math.h>
#include "usart.h"
#include <thread>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <ros/ros.h>
#include <fcntl.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int16.h>
using namespace std;
using namespace cv;

class Class_Usart usart;
vector<int> data;
void serial_thread();//串口函数
std::mutex serial_mutex;//某个锁
int x_vel=0,y_vel=0,z_vel=0;
int x_ad_vel=0,y_ad_vel=0,z_ad_vel=0;
double secs=0,secs_last;
geometry_msgs::Twist mcu_msgs;
/*
void serial_thread()
{
//extern int fd;
int err;   
ros::NodeHandle nh; 
ros::Publisher chatter_pub = nh.advertise<geometry_msgs::Twist>("raw_vel", 1000);                       //返回调用函数的状态      
while (ros::ok()) 
	{
		//if(send_new)
		//{
		serial_mutex.lock();
		//std::cout<<"send_new"<<send_new<<std::endl;
			if (usart.Usart_Recv(data)==0)
	{
		if(usart.d==254)
		{
			//printf("%d,%d,%d,%d;\n",usart.a,usart.b,usart.c,usart.d);
			mcu_msgs.linear.x=usart.a;//Class_Usart::a
			mcu_msgs.linear.y=usart.b;
			mcu_msgs.angular.z=usart.c;
			chatter_pub.publish(mcu_msgs);
		}
	}
		//sleep(0.01);
		serial_mutex.unlock();
       		//std::this_thread::sleep_for(std::chrono::milliseconds(10));  
		
	//}              
		//UART0_Close(fd);   
	} 
}
*/
void sendCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  // 将接收到的消息打印出来
 // ROS_INFO("I heard: [%f]", msg->linear.x);
x_vel=msg->linear.x*100;
/*
if(x<0)
{
x=32768-x;//big + data
}
*/
y_vel=-msg->linear.y*100*1.1;/*
if(y<0)
{
y=32768-y;//big + data
}
*/
z_vel=msg->angular.z*100*1.3;/*
if(z<0)
{
z=32768-z;//big + data
}
*/
//if(secs==0)
//{
//	secs =ros::Time::now().toSec();
 // ROS_INFO("I heard: [%f]", secs);
 if(x_ad_vel==0)
 {
	usart.Usart_Send(0xfe,x_vel,y_vel,1,1,z_vel);
 }
}
void adjustCallback(const geometry_msgs::Twist::ConstPtr& msg_ad)
{
  // 将接收到的消息打印出来
 // ROS_INFO("I heard: [%f]", msg->linear.x);
x_ad_vel=msg_ad->linear.x*100;
/*
if(x<0)
{
x=32768-x;//big + data
}
*/
y_ad_vel=-msg_ad->linear.y*100*1.1;/*
if(y<0)
{
y=32768-y;//big + data
}
*/
z_ad_vel=msg_ad->angular.z*100*1.3;/*
if(z<0)
{
z=32768-z;//big + data
}
*/
//if(secs==0)
//{
//	secs =ros::Time::now().toSec();
 // ROS_INFO("I heard: [%f]", secs);
	usart.Usart_Send(0xfe,x_ad_vel,y_ad_vel,1,1,z_ad_vel);
}
void visionCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
int x=msg->linear.x*100;//yaw
int y=msg->linear.y*100;//pitch
int z=msg->linear.z*100;//shoot
int w=msg->angular.x*100;//shoot
usart.Usart_Send(0xff,x,y,z,w,0);
}

int main(int argc,char** argv)
{
	ros::init(argc,argv,"serial");
	ros::NodeHandle n;
  	// 创建一个Publisher，发布名为chatter的topic，消息类型为geometry_msgs::Twist
	usart.Usart_Init();
	//std::thread thread1(serial_thread);
	//thread1.detach();
	ros::Subscriber sub = n.subscribe("cmd_vel", 1000,sendCallback);//位置？
	ros::Subscriber sub1 = n.subscribe("cmd_vel1", 1000,adjustCallback);//位置？
	ros::Subscriber sub2_ = n.subscribe("vision_tomcu", 1000,visionCallback);//位置？
	//ros::Rate rate(9);
	//while(ros::ok())
	//{
	//cout<<"1"<<endl;
	//usart.Usart_Send(1,21,10,1,1);
	ros::spin();
	//rate.sleep();
//}
return 0;
}

