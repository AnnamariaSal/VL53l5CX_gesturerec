
#!/usr/bin/env python
from geometry_msgs.msg import Pose, Point, Quaternion
from pilz_robot_programming.robot import Robot
from pilz_robot_programming.commands import Ptp, Lin
import math
import rospy
from std_msgs.msg import String

__REQUIRED_API_VERSION__ = "1"  # API version

planning_group = "left_manipulator"
target_link = "leftarm_tool0"
velocity_scale= 0.05 
# Funzione di callback per il topic dei gesti
gesture2="inizio"
home = Pose(position = Point(0.552595, -0.191486, 0.336480), orientation = Quaternion(0.134, -0.355, -0.840, -0.389))
base = "leftarm_base"

def gesture_callback(data):
    gesture = data.data
    #rospy.loginfo("Ricevuto gesto: %s", gesture)
    global gesture2
    if gesture == "tap" and gesture2!=gesture:
    
        rospy.loginfo("rilevato tap ")
        try:
            r.move(Ptp(goal = home,
                target_link = target_link,
                reference_frame = base,
                planning_group = planning_group,
                vel_scale = velocity_scale,
                acc_scale= 0.1,
                relative = False
                ))
        except:
            print("failed")
        gesture2="tap"
        pass

    elif gesture == "UpDown" and gesture2!= gesture:
        rospy.loginfo("rilevato UpDown ")
        try:
            r.move(Ptp(goal = Pose(position = Point(0.0, 0.0, -0.15), orientation = Quaternion(0,0,0,1)),
                target_link = target_link,
                reference_frame = target_link,
                planning_group = planning_group,
                vel_scale = velocity_scale,
                acc_scale= 0.1,
                relative = False
                ))
        except:
            print("failed")
        gesture2= "UpDown" 
        pass
        
    elif gesture == "LeftRight" and gesture2!=gesture:
        rospy.loginfo("rilevato LeftRight ")
        try:
            r.move(Ptp(goal = Pose(position = Point(-0.1, 0.1, 0.0), orientation = Quaternion(0,0,0,1)), 
                target_link = target_link,
                reference_frame = target_link,
                planning_group = planning_group,
                vel_scale = velocity_scale,
                acc_scale= 0.1,
                relative = False
                ))
        except:
            print("failed")
        gesture2="LeftRight"
        pass 
    
    else:
        rospy.logwarn("Gesto ripetuto")

# main program
def start_program():
    #print(r.get_current_pose()) # print the current position of the robot in the terminal
    
    # Sottoscrizione al topic dei gesti
    rospy.Subscriber("gesture_rec", String, gesture_callback)
    
    rospy.spin()

if __name__ == "__main__":
    # init a rosnode
    rospy.init_node('robot_program_node')

    # initialisation
    r = Robot(__REQUIRED_API_VERSION__)  # instance of the robot

    # start the main program
    
    start_program()
