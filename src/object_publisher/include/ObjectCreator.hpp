



#pragma once

#include <string>
#include <vector>
#include <cmath>

//#include <pugixml.hpp>


struct Object {
    double x{};
    double y{};
    double z{};
    std::string color{};
};





class ObjectCreator
{
public:

    ObjectCreator(const double &max_radius, const double &min_radius, const double &field_of_view) 
    :  max_radius_(max_radius), min_radius_(min_radius), field_of_view_(field_of_view)
    {
    }
bool try_get_viewed_objects_relative_position(std::vector<Object> &objects, const double &usv_x, const double &usv_y, const double &usv_heading){
        std::vector<Object> detected_objects;
        for(const auto &object : objects_){
            double distance = std::hypot(object.x-usv_x,object.y-usv_y);
            double object_angle = std::atan2(object.y-usv_y,object.x-usv_x);
            double angle_diff = object_angle-usv_heading;

            if(distance <= max_radius_ && distance > min_radius_ && angle_diff <= field_of_view_*0.5*M_PI/180){
                Object detected_object;
                detected_object.x = std::cos(angle_diff)*distance;
                detected_object.y = std::sin(angle_diff)*distance;
                detected_object.z = distance;
                detected_object.color = object.color;

                detected_objects.push_back(detected_object);
            }
            
        }
        if(detected_objects.size()>0){
            objects = detected_objects;
            return true;
        }
        return false;
    }


    //For creating multiple objects in a line with a given spacing and angle (0 deg is North).
    void add_objects_on_line(double start_x, double start_y, double z, std::string color, double heading, double space, int amount){
        double spacing_x = space*std::cos(heading);
        double spacing_y = space*std::sin(heading);
        for(int i = 0; i < amount; i++){
            add_object(start_x+spacing_x*static_cast<double>(i), start_y+spacing_y*static_cast<double>(i), z, color);
        }
    }
    void set_object_position(int index, double x, double y){

        if(objects_.size()>=index){
            objects_[index].x = x;
            objects_[index].y = y;            

        }
    }
    void set_object_position_y(int index, double y){
        if(objects_.size()>=index){
            objects_[index].y = y;
        }
    }

    //create a single buoy and add it to the list of objetcs.
    void add_object(double x, double y, double z, std::string color){
        Object object;

        object.x = x; object.y = y; object.z = z;
        object.color = color;

        objects_.push_back(object);
        object_count_ +=1;
    }


private:
    double max_radius_;
    double min_radius_;
    double field_of_view_;

    std::vector<Object> objects_;
    int object_count_;
    
    



};
