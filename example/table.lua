local data = {}
for i = 1, 40 do
	table.insert(data, "Data "..i) end

-- @param fn string Callback type 此次回调的目的
-- @param table LuaTableView 表格对象
-- @param a1 & a2 mixed Difference means for every "fn" 因fn参数而有不同的意义
local h = LuaEventHandler:create(function(fn, table, a1, a2)
	local r
	if fn == "cellSize" then
		-- Return cell size
		-- a1 is cell index (-1 means default size, in cocos2d-x version below 2.1.3, it's always -1)
		-- 请求每一格的尺寸
		-- a1是格的索引，-1为默认尺寸，在cocos2d-x 2.1.3之前，只会取得默认尺寸。
		r = CCSizeMake(320,30)
	elseif fn == "cellAtIndex" then
		-- Return CCTableViewCell, a1 is cell index (zero based), a2 is dequeued cell (maybe nil)
		-- Do something to create cell and change the content
		-- 请求“格”对象，a1是格索引（从0开始），a2是缓存的格对象（可能为空）
		-- 在此建立“格”对象并填充其要显示的内容。
		if not a2 then
			a2 = CCTableViewCell:create()
			-- Build cell struct, just like load ccbi or add sprite
			-- 建立“格”对象的结构，不要在此设定其显示的内容。
			a2:addChild(CCLabelTTF:create("", "Arial", 20), 0, 1)
		end
		-- Change content
		-- 修改“格”对象的内容
		tolua.cast(a2:getChildByTag(1), "CCLabelTTF"):setString(data[a1 + 1])
		r = a2
	elseif fn == "numberOfCells" then
		-- Return number of cells
		-- 请求格数
		r = #data
	-- Cell events:
	-- 表格事件：
	elseif fn == "cellTouched" then			-- A cell was touched, a1 is cell that be touched. This is not necessary.
	elseif fn == "cellTouchBegan" then		-- A cell is touching, a1 is cell, a2 is CCTouch
	elseif fn == "cellTouchEnded" then		-- A cell was touched, a1 is cell, a2 is CCTouch
	elseif fn == "cellHighlight" then		-- A cell is highlighting, coco2d-x 2.1.3 or above
	elseif fn == "cellUnhighlight" then		-- A cell had been unhighlighted, coco2d-x 2.1.3 or above
	elseif fn == "cellWillRecycle" then		-- A cell will be recycled, coco2d-x 2.1.3 or above
	end
	return r
end)

local t = LuaTableView:createWithHandler(h, CCSizeMake(320,480))
t:setBounceable(false)
--[[
-- Temporary unusable, please skip these lines to the comment section's end.
-- Add scroll bar and scroll track (track is optional), they'll be placed right or bottom in table.
-- The function LuaTableView:setScrollOffset(float) can be used for adjust scroll bar's position (bigger to right or bottom)
-- Assume those two sprite frames were loaded.
local sfc = CCSpriteFrameCache:sharedSpriteFrameCache()
local bar = CCScale9Sprite:createWithSpriteFrame(sfc:spriteFrameByName("scrollBar.png"))
bar:setOpacity(80)
bar:setPreferredSize(CCSizeMake(10, 10))
local trk = CCScale9Sprite:createWithSpriteFrame(sfc:spriteFrameByName("scrollTrack.png"))
trk:setOpacity(128)
trk:setPreferredSize(CCSizeMake(15, 15))
t:setScrollBar(bar, trk)
]]

-- Create a scene and run
local l = CCLayer:create()
l:addChild(t)
local s = CCScene:create()
s:addChild(l)
CCDirector:sharedDirector():runWithScene(s)
