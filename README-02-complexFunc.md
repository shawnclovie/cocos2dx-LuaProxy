##Complex Function in Lua#

###Listener App enter Background/Foreground#
In default cocos2d-x project, in AppDelegate.cpp, applicationDidEnterBackground() and applicationDidEnterForeground() should contains SimpleAudioEngine control.
You may replace it with those lines:
> // enter background  
if(LuaEventHandler::app){LuaEventHandler::app->applicationStateChange(kLuaEventAppEnterBackground);}  
// enter foreground  
if(LuaEventHandler::app){LuaEventHandler::app->applicationStateChange(kLuaEventAppEnterForeground);}

And add the listener in your lua initialize function:
> LuaEventHandler:createAppHandler(function(e)  
  	if e == kLuaEventAppEnterBackground then -- Do something like pause music  
		elseif e == kLuaEventAppEnterForeground then -- Do something like resume music when music option on  
		end  
end)
