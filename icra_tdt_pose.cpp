#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/Int16.h>
//#include <sensor_msgs/Imu.h>
#include <math.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#define RAD2DEG(x) ((x)*180./M_PI)
double slide=0;
double cos_angle=0;
double cal_angle=0;
double ans=0;
double x_wheel_pos = -0.10;
double y_wheel_pos = 0.0;

double x_wheel_vel = 0.0;
double y_wheel_vel = 0.0;
double x_origin_wheel_vel = 0.0;
double y_origin_wheel_vel = 0.0;
double x_base_vel= 0.0;
double y_base_vel = 0.0;
double robot_x_base_vel=0;
double robot_y_base_vel=0;
double theta = 0.0;
double theta_vel = 0.0;
double delta_theta = 0.0;
double x_base_pos = 0.0;

double y_base_pos = 0.0;
double uwb_x_pos=0.0;
double uwb_y_pos=0.0;
double uwb_angular=0.0;
double last_vel_time=0.0;
double delta_time=0.0;
double back_x_offset=-0.05;
double back_y_offset=0;
double right_x_offset=0.18;
double right_y_offset=0;
double front_x_offset=0.18;
double front_y_offset=0;
double left_x_offset=0.18;
double left_y_offset=0;
float degree=0;
struct laser{
double front;
double left;
double back;
double right;
double front_used;
double left_used;
double back_used;
double right_used;
double ans;
};
laser laser_;
     //geometry_msgs::TransformStamped map_trans;
std::vector<double> angle;
 geometry_msgs::PoseWithCovarianceStamped revify_pose;
int revify_sign=0;
//x方向对应y摩擦轮
//y方向对应x摩擦轮
double myabs(double msg)
{
msg=msg>0?msg:-msg;
return msg;
}
void uwbCallback(const geometry_msgs::Twist& pos){
uwb_x_pos=pos.linear.x;
uwb_y_pos=pos.linear.y;
uwb_angular=pos.angular.z;
laser_.back=pos.angular.x;
	}
void velCallback(const geometry_msgs::Twist& vel) {
	//callback every time the robot's linear velocity is received
	ros::Time current_time = ros::Time::now();
	
	x_origin_wheel_vel = vel.linear.x;
	y_origin_wheel_vel = vel.linear.y;
	theta_vel = vel.angular.z;
	//laser_.back = vel.angular.x;
ROS_INFO("vel=%f,%f,%f",x_origin_wheel_vel,y_origin_wheel_vel,theta_vel);//9-1
	if (theta_vel < 0.02&&theta_vel >- 0.02)
	{
		theta_vel = 0;
	}
	//第一帧赋初值
	if (last_vel_time == 0)
	{
		last_vel_time = current_time.toSec();
		delta_time = 0;
	}
	else
	{
		//时间差
		delta_time = current_time.toSec() - last_vel_time;
		//累加出角度
		theta += theta_vel*delta_time;
//ROS_INFO_STREAM("theta="<<theta);
		//角度变换，变到0-2pi度数
		if (theta > 6.2832)
		{
			theta -= 6.2832;
		}
		else if (theta < 0)
		{
			theta += 6.2832;
		}
		//换算速度坐标系至odom坐标系(由于小车坐标系与陀螺仪坐标系旋转相反，采用原始相反的theta角度,x=x*cos(theta)+y*sin(-theta);
		x_wheel_vel = x_origin_wheel_vel*sin(-theta) + y_origin_wheel_vel*cos(theta);
		y_wheel_vel = x_origin_wheel_vel*cos(-theta) + y_origin_wheel_vel*sin(theta);
//ROS_INFO("wheel_vel:%f,%f,%f",x_wheel_vel,y_wheel_vel,theta_vel);//9-1
		//计算小车速度
		x_base_vel = x_wheel_vel+ 0.10*theta_vel*sin(theta);
		y_base_vel = y_wheel_vel+ 0.10*theta_vel*cos(theta);
//ROS_INFO("base_vel:%f,%f,%f",x_base_vel,y_base_vel,theta_vel);//9-1
		robot_x_base_vel=x_base_vel*cos(theta)+y_base_vel*sin(theta);
		robot_y_base_vel=x_base_vel*sin(-theta)+y_base_vel*cos(theta);
		//累加成位移
		x_wheel_pos += x_wheel_vel*delta_time;
		y_wheel_pos += y_wheel_vel*delta_time;
	  //ROS_INFO("wheel_pose:%f,%f,%f",x_wheel_pos,y_wheel_pos ,delta_time);//9-1
		//建立正交轮坐标系
		//x_wheel_pos = x_wheel_pos - 0.275;
		//建立小车坐标系
		x_base_pos = x_wheel_pos + 0.10*cos(theta);
		y_base_pos = y_wheel_pos + 0.10*sin(theta);
	 // ROS_INFO("base_pose:%f,%f,%f",x_base_pos,y_base_pos,theta);//9-1
		//时间重新赋值
		last_vel_time = current_time.toSec();
	}
}
void sendCallback(const sensor_msgs::LaserScan& msg)
{

int count = msg.scan_time / msg.time_increment;
//ROS_INFO_STREAM("count"<<count);
 if(theta>=0&&theta<42.5*M_PI/180)//right
  {

for(int i = 0; i < count; i++) {
         degree= RAD2DEG(msg.angle_min + msg.angle_increment * i);
if(degree>-90.1&&degree<-89.9)
{
       // ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
}
if(degree>-85.1&&degree<-84.9)
{
      // ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
}
if(degree>-80.1&&degree<-79.9)
{
      // ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
}
   }
     //    ROS_INFO_STREAM("slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans<<"angle"<<angle[1]);
  }//if
      else if(theta>=42.5*M_PI/180&&theta<117.5*M_PI/180)//left
    {
      for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
	if(degree>89.9&&degree<90.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
} 
if(degree>84.9&&degree<85.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
} 
if(degree>79.9&&degree<80.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
} 
      }//for
    }//else if
    else if(theta>=117.5*M_PI/180&&theta<180*M_PI/180)//front 
    {
      for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
	if(degree>-0.1&&degree<0.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
} 
if(degree>4.9&&degree<5.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
} 
if(degree>9.9&&degree<10.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
} 
      }//for
 //ROS_INFO_STREAM("angle[0]"<<angle[0]<<"angle[1]"<<angle[1]<<"angle[2]"<<angle[2]);
    }//else if
      else if(theta>=180*M_PI/180&&theta<222.5*M_PI/180)//left
    {
      for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
	if(degree>89.9&&degree<90.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
} 
if(degree>84.9&&degree<85.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
} 
if(degree>79.9&&degree<80.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
} 
      }//for
    }//else if
     if(theta>=222.5*M_PI/180&&theta<270*M_PI/180)//right
  {

for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
if(degree>-90.1&&degree<-89.9)
{
       // ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
}
if(degree>-85.1&&degree<-84.9)
{
        //ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
}
if(degree>-80.1&&degree<-79.9)
{
       // ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
}
   }
     //    ROS_INFO_STREAM("slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans<<"angle"<<angle[1]);
  }//if
  else if(theta>=270*M_PI/180&&theta<312.5*M_PI/180)//front 
    {
      for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
	if(degree>-0.1&&degree<0.1)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
} 
if(degree>-5.1&&degree<-4.9)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
} 
if(degree>-10.1&&degree<-9.9)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
} 
      }//for
    }//else if
    else if(theta>=312.5*M_PI/180&&theta<360*M_PI/180)//right
    {
      for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
	if(degree>-90.1&&degree<-89.9)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[0]=msg.ranges[i];
} 
if(degree>-85.1&&degree<-84.9)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[1]=msg.ranges[i];
} 
if(degree>-80.1&&degree<-79.9)
{
       // ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
angle[2]=msg.ranges[i];
} 
      }//for
    }//else if
for(int i = 0; i < count; i++) {
        degree = RAD2DEG(msg.angle_min + msg.angle_increment * i);
if(degree>-90.1&&degree<-89.9)
{
       // ROS_INFO("right=: [%f, %f]", degree, msg.ranges[i]);
laser_.right=msg.ranges[i];
}
if(degree>-0.1&&degree<0.1)
        {
//ROS_INFO(": [%f, %f]", degree, msg.ranges[i]);
laser_.front=msg.ranges[i];
}
if(degree>89.9&&degree<90.1)
{
    //    ROS_INFO(":left= [%f, %f]", degree, msg.ranges[i]);
laser_.left=msg.ranges[i];
} 
   }
//ROS_INFO_STREAM("angle[0]="<<angle[0]<<"angle[1]"<<angle[1]<<"angle[2]"<<angle[2]);
}

void checkCallback(const std_msgs::Int16& pos)
{
      revify_sign=1;
//角度校准
 if(theta>=0&&theta<0.742)//right
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
  double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
  double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0,99:acos(cos_angle));
   ans=cal_angle-1.57;
 ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
  }
  else if(theta>=0.742&&theta<2.05)//42.5-117.5
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
    double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
    double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0.99:acos(cos_angle));
     ans=3.14-cal_angle;
    ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
  }
  else if(theta>=2.05&&theta<3.14)
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
    double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
    double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0.99:acos(cos_angle));
     ans=cal_angle+1.57;
    ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
  }
  else if(theta>=3.14&&theta<3.88)//180-222.5
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
    double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
    double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0.99:acos(cos_angle));
     ans=4.71-cal_angle;
    ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
  }
   else if(theta>=3.88&&theta<4.71)//222.5-270
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
    double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
    double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0.99:acos(cos_angle));
     ans=3.14+cal_angle;
    ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
    return;
  }
     else if(theta>=4.71&&theta<5.45)//270-312.5
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
    double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
    double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0.99:acos(cos_angle));
    ans=6.28-cal_angle;
    ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
  }
  else if(theta>=5.45&&theta<6.284)
  {
    double slide=sqrt(angle[0]*angle[0]+angle[2]*angle[2]-2*angle[0]*angle[2]*cos(0.174));
    double cos_angle=(angle[0]*angle[0]+slide*slide-angle[2]*angle[2])/(2*angle[0]*slide);
    double cal_angle=(cos_angle>0.99?0.99:(cos_angle<-0.99)?-0.99:acos(cos_angle));
     ans=4.71+cal_angle;
    ROS_INFO_STREAM("theta"<<theta<<"slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans*180/M_PI);
  }
//坐标校准
  //以地图x轴方向为零，红方步兵。
  if(ans<0)
    ans=-ans;
if(ans<0.48&&ans>=0)
{
//后侧朝后，中心在正向，需要相加，符号为减
x_wheel_pos =(laser_.back-back_x_offset)*cos(ans);
y_wheel_pos =(laser_.right-right_x_offset*tan(ans))*cos(ans);
}
else if(ans>=0.48&&ans<1.091)
{
x_wheel_pos=(laser_.left-left_x_offset*tan(1.57-ans))*cos(1.57-ans);
ROS_INFO_STREAM("x="<<x_wheel_pos);
y_wheel_pos=(laser_.right-right_x_offset*tan(ans))*cos(ans);
}
else if(ans>=1.091&&ans<1.57)
{
x_wheel_pos=(laser_.left-left_x_offset*tan(1.57-ans))*cos(1.57-ans);
//后侧偏差为负，实际是大值，符号相减
y_wheel_pos=(laser_.back-back_x_offset)*cos(1.57-ans);
}
else if(ans>=1.57&&ans<2.05)
{
x_wheel_pos=(laser_.left+left_x_offset*tan(ans-1.57))*cos(ans-1.57);
//后侧偏差为负，实际是大值，符号相减
y_wheel_pos=(laser_.back-back_x_offset)*cos(ans-1.57);
}
else if(ans>=2.05&&ans<2.66)
{
x_wheel_pos=(laser_.front+front_x_offset*tan(ans-1.57))*cos(ans-1.57);
//后侧偏差为负，实际是大值，符号相减
y_wheel_pos=(laser_.back-back_x_offset)*cos(ans-1.57);
}
else if(ans>=2.66&&ans<3.14)
{
x_wheel_pos=(laser_.front+front_x_offset)*cos(3.14-ans);
y_wheel_pos=(laser_.left-left_x_offset*tan(3.14-ans))*cos(3.14-ans);
}
else if(ans>=3.14&&ans<3.62)
{
x_wheel_pos=(laser_.front+front_x_offset)*cos(ans-3.14);

y_wheel_pos=(laser_.left+left_x_offset*tan(ans-3.14))*cos(ans-3.14);
}
else if(ans>=3.62&&ans<4.23)
{
x_wheel_pos=(laser_.right+right_x_offset*tan(4.71-ans))*sin(ans-3.14);
y_wheel_pos=(laser_.left+left_x_offset*tan(ans-3.14))*cos(ans-3.14);
}
else if(ans>=4.23&&ans<4.71)
{
x_wheel_pos=(laser_.right+right_x_offset*tan(4.71-ans))*cos(4.71-ans);
y_wheel_pos=(laser_.front+front_x_offset)*cos(4.71-ans);
}
else if(ans>=4.71&&ans<5.19)
{
x_wheel_pos=(laser_.right-right_x_offset*tan(ans-4.71))*cos(ans-4.71);
y_wheel_pos=(laser_.front+front_x_offset)*cos(ans-4.71);
}
else if(ans>=5.19&&ans<5.80)
{
  //后侧偏差为负，实际是大值，符号相减
x_wheel_pos=(laser_.back-back_x_offset)*sin(ans-4.71);
y_wheel_pos=(laser_.front+front_x_offset)*cos(ans-4.71);
}
else if(ans>=5.80&&ans<6.28)
{
  //后侧偏差为负，实际是大值，符号相减
x_wheel_pos=(laser_.back-back_x_offset)*cos(6.28-ans);
y_wheel_pos=(laser_.right+right_x_offset*tan(6.28-ans))*cos(6.28-ans);
}
		x_base_pos = x_wheel_pos + 0.10*cos(ans);
		y_base_pos = y_wheel_pos + 0.10*sin(ans);
ROS_INFO_STREAM("y_base_pos"<<y_base_pos<<"ans"<<ans);

// ROS_INFO_STREAM("angle[0]"<<angle[0]<<"angle[1]"<<angle[1]<<"angle[2]"<<angle[2]);
 ROS_INFO_STREAM("ans"<<ans*180/M_PI);
 /*map_trans.header.frame_id = "map";
 map_trans.child_frame_id = "odom";
 geometry_msgs::Quaternion map_quat = tf::createQuaternionMsgFromYaw(ans);
map_trans.header.stamp = ros::Time::now();
map_trans.transform.translation.x = x_base_pos;
map_trans.transform.translation.y = y_base_pos;
map_trans.transform.translation.z = 0.0;
map_trans.transform.rotation = map_quat;
map_broadcaster.sendTransform(map_trans);
//还原里程计
x_base_pos=0;
y_base_pos=0;
x_wheel_pos=-0.1;
y_wheel_pos=0;
*/
}

int main(int argc, char** argv){
  geometry_msgs::Twist base_vel;
  ros::init(argc, argv, "base_controller");
  ros::NodeHandle n;
 tf::TransformBroadcaster map_broadcaster;
 geometry_msgs::TransformStamped map_trans;
angle.push_back(0);
angle.push_back(0);
angle.push_back(0);
 ros::Time current_time_now = ros::Time::now();
  ros::Subscriber sub = n.subscribe("raw_msg", 50, velCallback);
  ros::Subscriber uwb_pose_sub_=n.subscribe("uwb_msg",2,uwbCallback);
  ros::Subscriber sub_scan = n.subscribe("scan",1000,sendCallback);//位置？
//初始定位话题
  ros::Subscriber recharify_pose_sub_=n.subscribe("check",2,checkCallback);
  ros::Publisher pub = n.advertise<geometry_msgs::Twist>("raw_vel", 1000);
  ros::Publisher pos_pub = n.advertise<geometry_msgs::Twist>("raw_pos", 1000);
  ros::Publisher pose_pub=n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 1000);
  tf::TransformBroadcaster odom_broadcaster;
  ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
ros::Rate rate(20);
  while (n.ok()) {
    if(revify_sign)
    {
      //ROS_INFO_STREAM("x_base_pos="<<x_base_pos<<"y_base_pos"<<y_base_pos<<"angle"<<ans*180/M_PI);
//      map_trans.header.frame_id = "map";
//       map_trans.child_frame_id = "odom";
// geometry_msgs::Quaternion map_quat = tf::createQuaternionMsgFromYaw(ans);
//map_trans.header.stamp = ros::Time::now();
//map_trans.transform.translation.x = x_base_pos;
//map_trans.transform.translation.y = y_base_pos;
//map_trans.transform.translation.z = 0.0;
//map_trans.transform.rotation = map_quat;
//map_broadcaster.sendTransform(map_trans);
      revify_pose.header.stamp=ros::Time::now();
revify_pose.header.frame_id="map";
revify_pose.pose.pose.position.x=x_base_pos;
revify_pose.pose.pose.position.y=y_base_pos;
revify_pose.pose.pose.orientation = tf::createQuaternionMsgFromYaw(ans);
revify_pose.pose.covariance[0] = 0.25;
revify_pose.pose.covariance[7] = 0.25;
//revify_pose.pose.covariance[14] = FLT_MAX;
//revify_pose.pose.covariance[21] = FLT_MAX;
//revify_pose.pose.covariance[28] =FLT_MAX;
revify_pose.pose.covariance[35] = 0.06;
pose_pub.publish(revify_pose);
 ROS_INFO_STREAM("global coordinate x="<<revify_pose.pose.pose.position.x<<"global coordinate y="<<revify_pose.pose.pose.position.y <<"rotation angle:"<<ans*180/M_PI);
//还原里程计
x_base_pos=0;
y_base_pos=0;
x_wheel_pos=-0.1;
y_wheel_pos=0;
revify_sign=0;
    }
	  ros::spinOnce();
	  ros::Time current_time_now = ros::Time::now();

	  //odom_trans：odom-正交轮
	  geometry_msgs::TransformStamped odom_trans, base_center,uwb_pos,change_trans;
	  
	  //方向四元数
	  geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(0);
	  odom_trans.header.frame_id = "odom";
	  odom_trans.child_frame_id = "orthgonality_wheel";
	  //robot's position in x,y, and z
	  odom_trans.transform.translation.x = x_wheel_pos;
	  odom_trans.transform.translation.y = y_wheel_pos;
//ROS_INFO_STREAM("wheel:x="<<x_wheel_pos<<"y="<<y_wheel_pos<<"theta="<<theta);
	  odom_trans.transform.translation.z = 0.0;
	  //robot's heading in quaternion
	  odom_trans.transform.rotation = odom_quat;
	  odom_trans.header.stamp = current_time_now;
	  //publish robot's tf using odom_trans object
	  odom_broadcaster.sendTransform(odom_trans);
	  //base_center：odom-底盘中心
	  geometry_msgs::Quaternion base_quat = tf::createQuaternionMsgFromYaw(theta);
	  base_center.header.frame_id = "odom";
	  base_center.child_frame_id = "base_footprint";
	  base_center.header.stamp = current_time_now;
	  base_center.transform.translation.x = x_base_pos;
	  base_center.transform.translation.y = y_base_pos;
	  base_center.transform.translation.z = 0.0;
	  base_center.transform.rotation = base_quat;
	  odom_broadcaster.sendTransform(base_center);
//ROS_INFO_STREAM("base:x="<<x_base_pos<<"y="<<y_base_pos<<"theta="<<theta);
//ROS_INFO_STREAM("slide"<<slide<<"cos_angle"<<cos_angle<<"cal_angle"<<cal_angle<<"angle"<<ans);
	  //小车位置
	  geometry_msgs::Twist base_pos;
	  base_pos.linear.x = x_base_pos;
	  base_pos.linear.y = y_base_pos;
	  base_pos.angular.z = theta;
pos_pub.publish(base_pos);
	  //小车中心速度
	  geometry_msgs::Twist base_vel;
	  base_vel.linear.x = robot_x_base_vel;
	  base_vel.linear.y = robot_y_base_vel;
	  base_vel.angular.z = theta_vel;
	  pub.publish(base_vel);
	  /*change_trans.header.frame_id = "map";
	  change_trans.child_frame_id = "odom";
	  //robot's position in x,y, and z
	  change_trans.transform.translation.x = 0;
	  change_trans.transform.translation.y = 0;
	  change_trans.transform.translation.z = 0.0;
	  change_trans.transform.rotation = odom_quat;
	  change_trans.header.stamp = current_time_now;
	  odom_broadcaster.sendTransform(change_trans);*/
 //导航模块
    nav_msgs::Odometry odom;
    odom.header.stamp = current_time_now;
    odom.header.frame_id = "odom";
    //robot's position in x,y, and z
    odom.pose.pose.position.x = x_base_pos;
    odom.pose.pose.position.y = y_base_pos;
    odom.pose.pose.position.z = 0.0;
    //robot's heading in quaternion
    odom.pose.pose.orientation = base_quat;

    odom.child_frame_id = "base_footprint";
    //linear speed from encoders
    odom.twist.twist.linear.x = robot_x_base_vel;
    odom.twist.twist.linear.y = robot_y_base_vel;
    odom.twist.twist.linear.z = 0.0;

    odom.twist.twist.angular.x = 0.0;
    odom.twist.twist.angular.y = 0.0;
    //angular speed from IMU
    odom.twist.twist.angular.z = theta_vel;

	//TODO: include covariance matrix here
	odom.pose.covariance[0] = 20;
	odom.pose.covariance[7] = 20;
	odom.pose.covariance[14] = FLT_MAX;
	odom.pose.covariance[21] = FLT_MAX;
	odom.pose.covariance[28] =FLT_MAX;
	odom.pose.covariance[35] = 50;

	odom.twist.covariance[0] = .1; 
	odom.twist.covariance[7] = .1; 
	odom.twist.covariance[14] = 1000000000;
	odom.twist.covariance[21] = 1000000000;
	odom.twist.covariance[28] = 1000000000;
	odom.twist.covariance[35] = .1; 

    odom_pub.publish(odom);
                //uwbweizhi
	  geometry_msgs::Quaternion uwb_quat = tf::createQuaternionMsgFromYaw(uwb_angular);    
	  uwb_pos.header.frame_id = "odom";
     uwb_pos.child_frame_id = "uwb";
    uwb_pos.transform.translation.x = uwb_x_pos;
    uwb_pos.transform.translation.y = uwb_y_pos;
    uwb_pos.transform.translation.z = 0.0;
    uwb_pos.transform.rotation = uwb_quat;
    uwb_pos.header.stamp = current_time_now;
    //publish robot's tf using odom_trans object
    odom_broadcaster.sendTransform(uwb_pos);
	  rate.sleep();
  }
}
