#pragma once
#ifndef __LUATABLEVIEW__
#define __LUATABLEVIEW__

#include "LuaEventHandler.h"

class LuaTableView : public CCTableView{
protected:
	LuaEventHandler *_handler;
	LuaTableView();
	CCScale9Sprite *_hScroller;
	CCScale9Sprite *_vScroller;
public:
	~LuaTableView();
	static LuaTableView * createWithHandler(LuaEventHandler *h, CCSize s, CCNode *c);
/*	virtual CCSize cellSizeForTable(CCTableView *t);
	virtual CCTableViewCell * tableCellAtIndex(CCTableView *t, unsigned int i);
	virtual unsigned int numberOfCellsInTableView(CCTableView *t);
	virtual void tableCellTouched(CCTableView *t, CCTableViewCell *c);
*/
	virtual void scrollViewDidScroll(CCScrollView *s){
		CCTableView::scrollViewDidScroll(s);
	}
	virtual void scrollViewDidZoom(CCScrollView *s){
		CCTableView::scrollViewDidZoom(s);
	}
	void setScroller(CCScale9Sprite *h, CCScale9Sprite *v);
};

#endif //__LUATABLEVIEW__