#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include <algorithm>  // Include per std::remove_if
#include <std_msgs/Empty.h>
#include <iostream>


serial::Serial ser;

std::string removeSpecialChars(const std::string& input) {
    std::string output = input;
    output.erase(std::remove_if(output.begin(), output.end(), [](char c) {
        return c == '\n' || c == '\r';
    }), output.end());
    return output;
}


std::string searchSerialPort(const std::string& sensor_name)
{
    std::vector<serial::PortInfo> ports = serial::list_ports();

    for (const auto& port : ports)
    {
        if (port.description.find(sensor_name) != std::string::npos)
        {
            std::cout << "Porta seriale trovata: " << port.port << std::endl;
            return port.port;
        }
    }

    std::cout << "Nessuna porta seriale trovata per il sensore: " << sensor_name << std::endl;
    return "";
}

/*void write_callback(const std_msgs::String::ConstPtr& msg){
    ROS_INFO_STREAM("Writing to serial port" << msg->data);
    ser.write(msg->data);
}*/

int main (int argc, char** argv){
    ros::init(argc, argv, "read_sensor_gesture_rec");
    ros::NodeHandle nh;


    ros::Publisher read_pub = nh.advertise<std_msgs::String>("distance", 1000);
    ros::Publisher gesture_pub = nh.advertise<std_msgs::String>("gesture_rec", 1000);
   	


   
    std::string sensor_name = "Seeed XIAO nRF52840 Sense";

    // Ricerca della porta seriale per il sensore specificato
    std::string serial_port = searchSerialPort(sensor_name);
    
    try
    {

        ser.setPort(serial_port);
        ser.setBaudrate(1000000);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
        
        /* Set low latency option */
      	char command[100] = {};
      	strcat(command, "setserial ");
      	strcat(command, serial_port.c_str()); //converte la stringa in char
      	strcat(command, " low_latency"); 
      	system(command);
       
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

    ros::Rate loop_rate(10);
    
    while(ros::ok()){

        ros::spinOnce();
 	    
 	    std::string result_dis;



            result_dis = ser.readline();
            
            if (!result_dis.empty()) {
            	if(result_dis[0]=='m'){
              std_msgs::String msg;
              msg.data = removeSpecialChars(result_dis.substr(1));  // Rimuovi il primo carattere 'm' e i caratteri speciali

              //msg.data = result_dis.substr(1);;
              read_pub.publish(msg);
              
             ros::Duration(0.1).sleep();             
              }
	    std::string result_gest;


            result_gest = ser.readline();
            
            if (!result_gest.empty()) {
              if (result_gest == "gtap\r\n" || result_gest == "gUpDown\r\n" || result_gest == "gLeftRight\r\n") {

                std_msgs::String msg;
                msg.data = removeSpecialChars(result_gest.substr(1));  // Rimuovi il primo carattere 'm' e i caratteri speciali

                //msg.data = result_gest;
                gesture_pub.publish(msg);
              
                ros::Duration(0.1).sleep();        
              }     
        }              
              
              
        }


           
            //ROS_INFO_STREAM("Starting sensor readings");
            
            loop_rate.sleep();
            
	     ser.flush();
    	
    }
    ser.close();
}
