-- Create a autorelease CCBProxy.
local proxy = LuaProxy:create()
-- Read a layer from ccbi file.
local n = proxy:readCCBFromFile"ALayer.ccbi"
-- Pick up a node named "Label", cast to label, set the string.
tolua.cast(proxy:getNode"Label", "CCLabelTTF"):setString"This is a label."
-- Create a scene and run
local s = CCScene:create()
s:addChild(tolua.cast(n, "CCLayer"))
CCDirector:sharedDirector():runWithScene(s)

-- Play another timeline (sequence)
local am = CCBAnimationManager:pickFromNode(n) -- "n" is returned from LuaProxy:readCCBFromFile()
am:runAnimations"2nd Timeline"
-- Handle timeline complate event.
-- "proxy" must be retained.
-- Callback: CCBAnimationManager *am, string timelineName, LuaEventHandler *hnd
proxy:handleAnimationComplate(am, function(am, timelineName, hnd)
	-- do something after timeline complate
end)
