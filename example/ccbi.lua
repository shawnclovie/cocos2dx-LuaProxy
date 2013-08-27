-- Create a autorelease CCBProxy.
local p = LuaProxy:create()
-- Read a layer from ccbi file.
local n = p:readCCBFromFile"ALayer.ccbi"
-- Pick up a node named "Label", cast to label, set the string.
tolua.cast(p:getNode"Label", "CCLabelTTF"):setString"This is a label."
-- Create a scene and run
local s = CCScene:create()
s:addChild(tolua.cast(n, "CCLayer"))
CCDirector:sharedDirector():runWithScene(s)
