cocos2dx-LuaProxy
=================

LuaProxy for cocos2d-x, include CocosBuilder support for lua, and other cocos2d-extension support for lua, and easy to use function.

Step 1 Add LuaProxy files to your project
-
Create a Lua based cocos2d-x project, or open your project that added Lua support.  
In Visual Studio, open Project Property, in C/C++ - Additional Include Directories, add LuaProxy source code directory, and add all source files.  
In Xcode, add all source files.

Step 2 Add C++ Code
-
Open CCAppDelegate.cpp, add include like this:  
>  #include "tolua/tolua_CC_Extension.h"

Move to the line that got CCLuaEngine, add this:  
> tolua_CC_Extension_open(pEngine->getLuaState());  

Now you can do those things in lua, but we must create a CCBProxy at first, I wrote this in lua:  
local proxy = CCBProxy:create()  

Load ccbi made by CocosBuilder
-
Maybe you have a ccbi file named "ui-start.ccbi", root node typed with "CCLayer", place it in "Resources/ccb", call this in lua:

> local n = proxy:readCCBFromFile("ccb/ui-start.ccbi") -- Got a CCNode  
local l = tolua.cast(n, "CCLayer") -- Cast the node into CCLayer  

Now you can add the "l" into any scene.

Referrence any node in ccbi
-
We'd loaded a ccbi as layer, but you want access a CCLabelTTF, marked "owner" and named "label" in CocosBuilder, and a CCControlButton named "button". Just do this:

> local lbl = proxy:getNodeWithType("label", "CCLabelTTF")  
lbl:setString("some string")  
local btn = proxy:getNodeWithType("button", "CCControlButton")

When CCBProxy read ccbi, it has saved all member variable into a CCDictionary.

Proxy CCControlButton event with any function
-
There are two way to do the thing.  
>  -- Handle single button event  
proxy:handleButtonEvent(btn, function(s, e) print(s, e) end, 0) -- 0 means UpInside

or

>  -- Handle all CCControlButton and CCMenuItem event  
proxy:handleSelector(function(s, e)  
  if s == btn then print("some thing") end  
end)

todo...
