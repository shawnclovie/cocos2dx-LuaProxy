-- Create a autorelease LuaProxy.
-- 建立自动释放的LuaProxy。
local proxy = LuaProxy:create()
-- Read a layer from ccbi file.
-- 从ccbi文件中读一个层。
local n = proxy:readCCBFromFile"ALayer.ccbi"
-- Pick up a node named "Label", cast to label, set the string.
-- 取得名为Label的节点，转换为CCLabelTTF，设定字符串内容。
tolua.cast(proxy:getNode"Label", "CCLabelTTF"):setString"This is a label."
-- Create a scene and run.
-- 建立场景并显示。
local s = CCScene:create()
s:addChild(tolua.cast(n, "CCLayer"))
CCDirector:sharedDirector():runWithScene(s)

-- Play another timeline (sequence).
-- 播放另一个时间线动画。
-- "n" is returned from LuaProxy:readCCBFromFile()
-- "n" 是前面从文件读取的节点。
local am = CCBAnimationManager:pickFromNode(n)
am:runAnimations"2nd Timeline"
-- Handle timeline complate event.
-- "proxy" must be retained.
-- 监听时间线完成事件，"proxy" 必须执行retain函数。
-- Callback: CCBAnimationManager *am, string timelineName, LuaEventHandler *hnd
proxy:handleAnimationComplate(am, function(am, timelineName, hnd)
	-- do something after timeline complate
end)
