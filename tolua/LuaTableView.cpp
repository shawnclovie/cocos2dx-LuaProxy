/****************************************************************************
Copyright (c) 2013 Shawn Clovie

http://mcspot.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

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
