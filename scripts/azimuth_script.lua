
local CONTROL_OUTPUT_THROTTLE = 3
local CONTROL_OUTPUT_YAW = 4

local SERVO_MIDPOINT = 1500
local THRUSTER_MIDPOINT = 1500

-- Parameters:

local MOTOR = {
  M1 = 94, --script1
  M2 = 95, --script2
  M3 = 96, --script3
  M4 = 97 --script4
}

local SERVO = {
  S1 = 98, -- script5
  S2 = 99, -- script6
  S3 = 100, -- script7
  S4 = 101 -- script8
}


local ROVER_FUNCTION = {
  THROTTLE = 70,
  THROTTLE_LEFT = 73,
  THROTTLE_RIGHT = 74,
  STEERING = 26
}

-- Utilities:1

function scale(x, in_min, in_max,  out_min,  out_max)
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
end
--

local throttle_scaled  = 0
local steering_angle = 0

local omni_angle = math.floor(scale(45,0,360,500.0,2500.0))-500

local steering = 0
local throttle = 0

local servo_angle = 0

local yaw = 0
local throttle_in = 0
local steering_in = 0

 -- https://ardupilot.org/sub/docs/common-lua-scripts.html
function update() 
  if not arming:is_armed() then
    --SRV_Channels:set_output_pwm(MOTOR.M1, THRUSTER_MIDPOINT)
    --SRV_Channels:set_output_pwm(MOTOR.M2, THRUSTER_MIDPOINT)
    --SRV_Channels:set_output_pwm(MOTOR.M3, THRUSTER_MIDPOINT)
    --SRV_Channels:set_output_pwm(MOTOR.M4, THRUSTER_MIDPOINT)
    SRV_Channels:set_output_pwm(SERVO.S1, SERVO_MIDPOINT)
    SRV_Channels:set_output_pwm(SERVO.S2, SERVO_MIDPOINT)
    SRV_Channels:set_output_pwm(SERVO.S3, SERVO_MIDPOINT)
    SRV_Channels:set_output_pwm(SERVO.S4, SERVO_MIDPOINT)
    yaw = ahrs:get_yaw_rad()
    throttle_in = SRV_Channels:get_output_scaled(ROVER_FUNCTION.THROTTLE)
    steering_in = SRV_Channels:get_output_scaled(ROVER_FUNCTION.STEERING)
  else
    -- retrieve steering and throttle control outputs from vehicle in -1 to +1 range
    steering = vehicle:get_control_output(CONTROL_OUTPUT_YAW)
    throttle = vehicle:get_control_output(CONTROL_OUTPUT_THROTTLE)
    throttle_scaled = math.floor(scale(throttle,0,1.0, 0, 100))
    steering_angle =  math.floor(scale(steering,-1,1, -25, 25))
  

    SRV_Channels:set_output_pwm(SERVO.S1, SERVO_MIDPOINT+omni_angle) 
    SRV_Channels:set_output_pwm(SERVO.S2, SERVO_MIDPOINT-omni_angle) 
    SRV_Channels:set_output_pwm(SERVO.S3, SERVO_MIDPOINT-omni_angle) 
    SRV_Channels:set_output_pwm(SERVO.S4, SERVO_MIDPOINT+omni_angle) 


  end
  
  --gcs:send_text(6, string.format("Yaw:%5.3f", math.deg(yaw)))
--gcs:send_text(6, string.format("Str:%d", math.floor(scale(omni_angle+SERVO_MIDPOINT,500,2500,0,360))))

  return update, 20 
end
--yaw = ahrs:get_yaw_rad() 




gcs:send_text(6, "azimuth_script.lua is running")
return update()