#include "LuaTableView.h"

LuaTableView * LuaTableView::createWithHandler(LuaEventHandler *h, CCSize s, CCNode *c){
	LuaTableView *r = new LuaTableView();
	CC_SAFE_RETAIN(h);
	r->_handler = h;
	r->setDataSource(h);
	r->initWithViewSize(s, c);
	r->setDelegate(h);
	r->autorelease();
	r->_updateContentSize();
	return r;
}
LuaTableView::LuaTableView(){
	_handler = NULL;
	_hScroller = NULL;
	_vScroller = NULL;
}
LuaTableView::~LuaTableView(){
	CC_SAFE_RELEASE(_handler);
	CC_SAFE_RELEASE(_hScroller);
	CC_SAFE_RELEASE(_vScroller);
}
/*
CCSize LuaTableView::cellSizeForTable(CCTableView *t){
	return _handler->cellSizeForTable(t);
}
CCTableViewCell * LuaTableView::tableCellAtIndex(CCTableView *t, unsigned int i){
	return _handler->tableCellAtIndex(t, i);
}
unsigned int LuaTableView::numberOfCellsInTableView(CCTableView *t){
	return _handler->numberOfCellsInTableView(t);
}
void LuaTableView::tableCellTouched(CCTableView *t, CCTableViewCell *c){
	_handler->tableCellTouched(t, c);
}
*/
void LuaTableView::setScroller(CCScale9Sprite *h, CCScale9Sprite *v){
	CC_SAFE_RELEASE(_hScroller);
	CC_SAFE_RELEASE(_vScroller);
	_hScroller = h;
	_vScroller = v;
	CC_SAFE_RETAIN(h);
	CC_SAFE_RETAIN(v);
}
