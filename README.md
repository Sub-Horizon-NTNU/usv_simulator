# USV SIMULATOR (GUIDE IS STILL WORK IN PROGRESS)
The main goal of this project is to be able to simulate Sub-Horizon/Selene in a environment similar to Autodrone.


## Dependencies



### ROS2 Humble 
#### Distrobox
 If you're using a version of Ubuntu that is not equal to 22.04, we highly recommend using Distrobox. To setup distobox just use the following commands:
 install : 

 > sudo apt install distrobox

Non NVIDIA GPU

 > distrobox create -n ubuntu_2204 --image ubuntu:22.04

NVIDIA GPU:

> distrobox create -n ubuntu_2204 --image ubuntu:22.04 --nvidia

After the distrobox is created you can enter the container by using the following command:
> distrobox enter ubuntu_2204


#### ROS2 Humble
Follow the guide at: [ROS2 Humble Installation](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debs.html)

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
* GCC 13
> sudo apt install libsdl2-dev libglm-dev libasio-dev 

> git clone "https://github.com/patrykcieslak/stonefish.git"

> cd stonefish

> mkdir build

> cd build

> cmake ..

> make -jX

> sudo make install

Please refer to [Stonefish installation guide](https://stonefish.readthedocs.io/en/latest/install.html) if experiencing any issues.

### Ardupilot SITL

Clone ardupilot into src/
> git clone --recurse-submodules https://github.com/ArduPilot/ardupilot.git

> cd ardupilot && ./Tools/environment_install/install-prereqs-ubuntu.sh -y && cd ../../


### Micro XRCEDDS GEN

> vcs import --recursive --input  https://raw.githubusercontent.com/ArduPilot/ardupilot/master/Tools/ros2/ros2.repos src

> sudo rosdep init 

> rosdep update

> source /opt/ros/humble/setup.bash

> rosdep install --from-paths src --ignore-src -r -y

#### Installing the MicroXRCEDDSGen build dependency:

> sudo apt install default-jre

> git clone --recurse-submodules https://github.com/ardupilot/Micro-XRCE-DDS-Gen.git

> cd Micro-XRCE-DDS-Gen && ./gradlew assemble

> echo "export PATH=\$PATH:$PWD/scripts" >> ~/.bashrc

#### Test it by running:
> source ~/.bashrc && microxrceddsgen -help
You should expect to get some output

### MAVProxy 
> sudo apt-get install python3-dev python3-opencv python3-wxgtk4.0 python3-pip python3-matplotlib python3-lxml python3-pygame

> python3 -m pip install PyYAML mavproxy --user

> echo 'export PATH="$PATH:$HOME/.local/bin"' >> ~/.bashrc


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

