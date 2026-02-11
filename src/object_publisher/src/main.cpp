


#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "ObjectPublisher.hpp"

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ObjectPublisher>());
    rclcpp::shutdown();
    return 0;
}
