-- CCControlButton
proxy = LuaProxy:create()
proxy:retain() -- hold the proxy, while the button gone, release the proxy.
local button = ... -- got from ccbi or create with code.
proxy:handleButtonEvent(button, function(button, event)
	-- do something
end, CCControlEventTouchUpInside)

-- CCMenuItem, needn't LuaProxy, event support by cocos2d-x
local item = ... -- got from ccbi or create with code.
item:registerScriptTapHandler(function()
	-- do something
end)
