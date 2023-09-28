#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>

#include <std_msgs/Empty.h>
#include <iostream>


serial::Serial ser;

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
    ros::init(argc, argv, "read_sensor");
    ros::NodeHandle nh;

    //ros::Subscriber write_sub = nh.subscribe("write", 1000, write_callback);
    //ros::Publisher read_pub = nh.advertise<std_msgs::String>("read", 1000);
    ros::Publisher read_pub = nh.advertise<std_msgs::String>("distance", 1000);
    std::string resolution_value;
    std::string sens;
    //nh.getParam("resolution", resolution_value); //prima di ros run devo settare questo parametro con rosparam set /nome_del_nodo/my_param q oppure o in base a se voglio la risoluzione a 4 oppure ad 8
    std::string sensor_name = "Seeed XIAO nRF52840 Sense";

    // Ricerca della porta seriale per il sensore specificato
    std::string serial_port = searchSerialPort(sensor_name);
    
    try
    {
        //ser.setPort("/dev/ttyACM0");
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

    ros::Rate loop_rate(8);
    
    while(ros::ok()){

        ros::spinOnce();
	

            std_msgs::String result;
            size_t bytes_wrote = ser.write("b");
            
	    //scegli la risoluzione
            //bytes_wrote = ser.write("o");
            
            if (argc>2){ //QUANDO FACCIO ROSRUN DEVO SPECIFICARE 1, 2 OPPURE 3 IN BASE A SE VOGLIO LA MISURA DEL SENSORE UP, UNGHIA O ENTRAMBI         
            sens=argv[1];
            resolution_value=argv[2];           
            }
                
                        
            bytes_wrote = ser.write(resolution_value);
            bytes_wrote = ser.write(sens);

            result.data = ser.read(ser.available());
            
            //ROS_INFO_STREAM("Starting sensor readings");
            
            ROS_INFO_STREAM(result);
            read_pub.publish(result);       
            loop_rate.sleep();
            
	     ser.flush();
    	
    }
    ser.close();
}
