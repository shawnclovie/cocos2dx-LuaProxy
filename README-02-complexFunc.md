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

###CCScale9Sprite and CCScrollView#
Simple add those and add they into your layer.
> local s9 = CCScale9Sprite:createWithSpriteFrame(spFrameWasLoaded, CCReckMake(5,5,3,3))  
local sv = CCScrollView:create()  
sv:isBounceable(false)

###CCTableView#
You may not control CCTableView in Lua, so I created a sub class LuaTableView, it can add a LuaEventHandler with delegate and data source.
Firstly, create a handler:  

> -- @param fn string Callback type  
-- @param table LuaTableView  
-- @param a1 & a2 mixed Difference means for every "fn"  
local h = LuaEventHandler:createWithFunction(function(fn, table, a1, a2)  
	local r  
	if fn == "cellSize" then  
		-- Return cell size  
		r = CCSizeMake(320,100)  
	elseif fn == "cellAtIndex" then  
		-- Return CCTableViewCell, a1 is cell index, a2 is dequeued cell (maybe nil)  
		-- Do something to create cell and change the content  
		if not a2 then  
			a2 = CCTableViewCell:create()  
			-- Build cell struct, just like load ccbi or add sprite  
		end  
		-- Change content
	elseif fn == "numberOfCells"  
		-- Return number of cells  
		r = 100  
	elseif fn == "cellTouched" then  
		-- A cell was touched, a1 is cell that be touched. This is not necessary.  
	end  
	return r  
end

Next we can create the table:
> local table = LuaTableView:createWithHandler(h, CCSizeMake(320,480))  
-- Set table properties, referrence CCTableView API, and add into layer.

###Http Get/Post#
Just add those lines in lua:
> CCHttpRequest:open("https://google.com/search?q=LuaProxy&safe=strict", kHttpGet):sendWithHandler(function(res, hnd)  
print(res:getResponseData(), res:getResponseCode())  
end)

If you want do a post http request, change arguments in open:
> CCHttpRequest:open("http://google.com/search?q=LuaProxy&safe=strict", kHttpPost, "query=param1&other=params"):sendWithHandler(function(res, hnd) end)

Arguments to the call back function, first is CCHttpResponse, 2nd is LuaEventHandler.

### LuaCallFuncInterval ###
Call function multi times in duration seconds, useful just like animation change contentSize or preferredSize or other.
> local s = layer:getContentSize()
> layer:runAction(LuaCallFunction:create(1, function(t, n)  
> -- Change layer width larger 100 pixel  
> n:setContentSize(CCSizeMake(s.width + 100 * t, s.height))  
> end))
