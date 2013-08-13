#cocos2dx-LuaProxy#
-------
LuaProxy for cocos2d-x, include CocosBuilder support for lua, and other cocos2d-extension support for lua, and easy to use function.

##For cocos2d-x 3.0 alpha#
Sorry, I've no time to upgrade LuaProxy to fit 3.0 alpha, in a short time.

##For cocos2d-x 2.1.4#
Don't add tolua_extensions_ccb_open(lua_State) to AppDelegate.cpp.  
For iOS: And remove CCBProxy.*, Lua_extensions_CCB.* at lib/cocos2d/lua/cocos2dx_support (4 files).

##Change Log#
2013-6-19 Moved some functions about ui to ui/UIUtil from CCBProxy. Moved UIEventDispatcher into UIUtil.h/.cpp no longer stand alone. Add CameraEyeAction to do some 3d effect.  
2013-6-14 Add CCGLProgram support for lua.  
2013-6-9 Support cocos2d-x 2.1.3.  
2013-5-29 Add two LuaTableView events, and ui/UIUtil for useful functions.  
2013-5-16 Add static functions to LuaEventHandler to direct call lua function.  
2013-4-8 Add scroll bar support for LuaTableView. Temporary remove support for CCEditBox.  
2013-3-28 Fix for support cocos2d-x 2.1.1.  
2013-3-22 Bug fix: LuaTableView is "null" userdata in lua. Add LuaCallFuncInterval for call lua function every frame in duration seconds.  
2013-3-19 Change main include file name to luaopen_LuaProxy.h - All function useful for lua will be add, not only CC_Extension.

##Step 1 Add LuaProxy files to your project#
Create a Lua based cocos2d-x project, or open your project that added Lua support.  
In Visual Studio, open Project Property, in C/C++ - Additional Include Directories, add LuaProxy source code directory, and add all source files.  
In Xcode, add all source files. You may safety remove ui/UIEventDispatcher.cpp(and .h), it's just design for win32.

##Step 2 Add C++ Code#
Open CCAppDelegate.cpp, add include like this:  
>  #include "tolua/luaopen_LuaProxy.h"

Move to the line that got CCLuaEngine, add this:  
> luaopen_LuaProxy(pEngine->getLuaState());  

Now you can do those things in lua, but we must create a CCBProxy at first, I wrote this in lua:  

> local proxy = CCBProxy:create()  
-- !!  
-- If you want to handle CCControlButton, you must retain the proxy,  
-- And kept variable "proxy" to global or global table.  
-- While the button won't be use, just like scene poped, you'd batter release the proxy.  
-- proxy:retain()

##About CCBI#

###Load ccbi made by CocosBuilder#
Maybe you have a ccbi file named "ui-start.ccbi", root node typed with "CCLayer", place it in "Resources/ccb", call this in lua:

> local n = proxy:readCCBFromFile("ccb/ui-start.ccbi") -- Got a CCNode  
local l = tolua.cast(n, "CCLayer") -- Cast the node into CCLayer  

Now you can add the "l" into any scene.

###Referrence any node in ccbi#
We'd loaded a ccbi as layer, but you want access a CCLabelTTF, marked "owner" and named "label" in CocosBuilder, and a CCControlButton named "button". Just do this:

> local lbl = tolua.cast(proxy:getNode"label", "CCLabelTTF")  
lbl:setString("some string")  
local btn = proxy:getNodeWithType("button", "CCControlButton")

When CCBProxy read ccbi, it has saved all member variable into a CCDictionary.  
So if you'd loaded other ccbi and any variable defined same name, the old one will be lost in the CCDictionary.

###Proxy CCControlButton event with any function#
There are two way to do the thing.  
>  -- Handle single button event, you must kept the proxy (retain it, release after the button never use.)  
proxy:handleButtonEvent(btn, function(s, e) print(s, e) end, 0) -- 0 means UpInside

[Continue Read] (README-02-complexFunc.md)
