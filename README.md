# USV SIMULATOR (README TO BE UPDATED)
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

> cd ardupilot && ./Tools/environment_install/install-prereqs-ubuntu.sh -y 

Add Ålesund into locations.txt
> echo 'AALESUND=62.469467785628,6.2369782900802155,0,270' >> Tools/autotest/locations.txt

Also run the line below. It fixes a issue that is making the ardupilot sitl dds launch arducopter instead of the desired ardurover.
> mv ../temp_files/launch.py Tools/ros2/ardupilot_sitl/src/ardupilot_sitl/launch.py

> cd ../../

> 

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


### MAVROS
Head to src/ and clone the following repo:
> git clone https://github.com/mavlink/mavros.git


## Build
After all the tiresome steps are completed it is time to build:

> colcon build


## Run

Then inside the usv simulator workspace run:
> ros2 launch usv_simulator usv_simulator.launch.py



> mavlink-routerd -e 127.0.0.1:14557 -e 127.0.0.1:14558 127.0.0.1:14550
