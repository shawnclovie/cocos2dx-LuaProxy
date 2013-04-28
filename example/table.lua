local data = {}
for i = 1, 40 do
	table.insert(data, "Data "..i) end

-- @param fn string Callback type
-- @param table LuaTableView
-- @param a1 & a2 mixed Difference means for every "fn"
local h = LuaEventHandler:create(function(fn, table, a1, a2)
	local r
	if fn == "cellSize" then
		-- Return cell size
		r = CCSizeMake(320,30)
	elseif fn == "cellAtIndex" then
		-- Return CCTableViewCell, a1 is cell index (zero based), a2 is dequeued cell (maybe nil)
		-- Do something to create cell and change the content
		if not a2 then
			a2 = CCTableViewCell:create()
			-- Build cell struct, just like load ccbi or add sprite
			a2:addChild(CCLabelTTF:create("", "Arial", 20), 0, 1)
		end
		-- Change content
		tolua.cast(a2:getChildByTag(1), "CCLabelTTF"):setString(data[a1 + 1])
	elseif fn == "numberOfCells" then
		-- Return number of cells
		r = #data
	elseif fn == "cellTouched" then
		-- A cell was touched, a1 is cell that be touched. This is not necessary.
	end
	return r
end)

local t = LuaTableView:createWithHandler(h, CCSizeMake(320,480))
t:setBounceable(false)
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

-- Create a scene and run
local l = CCLayer:create()
l:addChild(t)
local s = CCScene:create()
s:addChild(l)
CCDirector:sharedDirector():runWithScene(s)
