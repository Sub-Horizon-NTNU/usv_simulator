



local LIGHTS = {
  ORANGE = 0, --Script9 servo 9
  GREEN = 1,--Script10 servo 10
  RED = 2,--Script11 servo 11

}



 -- https://ardupilot.org/sub/docs/common-lua-scripts.html
function update()

      relay:toggle(LIGHTS.ORANGE)
      relay:toggle(LIGHTS.GREEN)
      relay:toggle(LIGHTS.RED)
      relay:toggle(4)
      relay:toggle(5)
      relay:toggle(6)

       
  
    
  return update, 1000
end



gcs:send_text(6, "lights_script.lua is running")
return update()