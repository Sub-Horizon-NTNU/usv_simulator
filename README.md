# USV SIMULATOR
The main goal of this project is to be able to simulate Sub-Horizon/Selene in a environment similar to Autodrone.


## Dependencies

### SITL

### MAVROS

> sudo apt install ros-humble-mavros

Then install GeographicLib datasets by running the install_geographiclib_datasets.sh script:
> ros2 run mavros install_geographiclib_datasets.sh

> #Alternative:

> wget https://raw.githubusercontent.com/mavlink/mavros/ros2/mavros/scripts/install_geographiclib_datasets.sh 

> ./install_geographiclib_datasets.sh

### Stonefish (Simulator software)
Stonefish is the framework used for creating the simulation, it provides a physicssimulator using Bullet Physics and visualization using OpenGL.

#### Installation

##### Dependencies

> sudo apt install libsdl2-dev libglm-dev libasio-dev 

> git clone "https://github.com/patrykcieslak/stonefish.git"

> cd stonefish

> mkdir build

> cd build

> cmake ..

> make -jX

> sudo make install

Please refer to [Stonefish installation guide](https://stonefish.readthedocs.io/en/latest/install.html) if experiencing any issues.


#### Stonefish ROS2

## How to install

## How to run simulation


Then inside the usv simulator workspace run:
> ros2 launch usv_simulator usv_simulator.launch.py



> mavlink-routerd -e 127.0.0.1:14557 -e 127.0.0.1:14558 127.0.0.1:14550

You can then connect to a QGC.

python3 sim_vehicle.py -v Rover --model JSON -f rover --no-mavproxy -L AALESUND -A "--serial0=udpclient:127.0.0.1:14550"



ROS2 
ros2 launch ardupilot_sitl sitl_dds_udp.launch.py transport:=udp4 synthetic_clock:=True wipe:=False model:=JSON speedup:=1 slave:=0 instance:=0 sim_address:=127.0.0.1 master:=tcp:127.0.0.1:5760 defaults:=$(ros2 pkg prefix ardupilot_sitl)/share/ardupilot_sitl/config/default_params/rover.parm


refs:=$(ros2 pkg prefix ardupilot_sitl)/share/ardupilot_sitl/config/dds_xrce_profile.xml 

