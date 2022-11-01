require('luapa')
require('luapm')

luapm.Pm_Initialize();
print("Num of device=",luapm.GetNumDevices())
for i=0,luapm.GetNumDevices()-1 do 
    print(i,luapm.GetDeviceName(i))
end

