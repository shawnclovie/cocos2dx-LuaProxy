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
LuaTableView::LuaTableView():_scrollOffset(0), _handler(0), _scrollBar(0), _scrollTrack(0), _scrollTrackDelta(0){}
LuaTableView::~LuaTableView(){
	CC_SAFE_RELEASE(_handler);
	setScrollBar(0, 0);
}
void LuaTableView::setScrollBar(CCScale9Sprite *s, CCScale9Sprite *st){
	if(_scrollBar){		_scrollBar->removeAllChildrenWithCleanup(true);}
	if(_scrollTrack){	_scrollTrack->removeAllChildrenWithCleanup(true);}
	_scrollBar = s;
	_scrollTrack = st;
	if(s){
		if(st){
			addChild(st, 255);
		}
		addChild(s, 255);
		resetScroll();
	}
}
void LuaTableView::setScrollOffset(float o){
	_scrollOffset = o;
}
void LuaTableView::updateScroll(){
	if(!_scrollBar || !_scrollBar->isVisible()){
		return;
	}
	bool vert = getDirection() == kCCScrollViewDirectionVertical;
	CCSize vs = getViewSize(),
		cs = getContentSize(),
		ss = _scrollBar->getPreferredSize();
	CCPoint cp = getContentOffset(),
		p = _scrollBar->getPosition();
	float rate = 1 - (vert? abs(cp.y) / (cs.height - vs.height) : abs(cp.x) / (cs.width - vs.width));
	if(vert){	p.y = cs.height - (cs.height - ss.height) * rate - ss.height - _scrollTrackDelta;
	}else{		p.x = cs.width - (cs.width - ss.width) * rate - ss.width - _scrollTrackDelta;
	}
	_scrollBar->setPosition(p);
	if(_scrollTrack){
		p = _scrollTrack->getPosition();
		if(vert){	p.y = abs(cp.y);
		}else{		p.x = abs(cp.x);
		}
		_scrollTrack->setPosition(p);
	}
//CCLog("LTV.up ss=%d,%d vh=%d cy=%d ch=%d y=%d", (int)ss.width, (int)ss.height, (int)vs.height, (int)p.y, (int)cs.height, (int)_scrollBar->getPositionY());
}
void LuaTableView::scrollViewDidScroll(CCScrollView *s){
	CCTableView::scrollViewDidScroll(s);
	updateScroll();
}
void LuaTableView::scrollViewDidZoom(CCScrollView *s){
	CCTableView::scrollViewDidZoom(s);
}
void LuaTableView::reloadData(){
	CCTableView::reloadData();
	resetScroll();
}
void LuaTableView::resetScroll(){
	if(!_scrollBar){ return;}
	CCSize vs = getViewSize(), cs = getContentSize();
	bool vert = getDirection() == kCCScrollViewDirectionVertical;
	bool v = vert? vs.height < cs.height : vs.width < cs.height;
	_scrollBar->setVisible(v);
	CCPoint p = ccp(0, 0), ap = vert? ccp(1,0) : CCPointZero;
	if(v){
		CCSize s = _scrollBar->getPreferredSize(),
			st = _scrollTrack? _scrollTrack->getPreferredSize() : CCSizeZero;
		if(vert){
			p.x = vs.width + _scrollOffset - (st.width > 0? (st.width - s.width) / 2 : 0);
			_scrollTrackDelta = st.height > 0? (st.height - s.height) / 2 : 0;
			s.height = vs.height / cs.height * vs.height - _scrollTrackDelta * 2;
		}else{
			p.y = vs.height + _scrollOffset - (st.height > 0? (st.height - s.height) / 2 : 0);
			_scrollTrackDelta = st.width > 0? (st.width - s.width) / 2 : 0;
			s.width = vs.width / cs.width * vs.width - _scrollTrackDelta * 2;
		}
		_scrollBar->setAnchorPoint(ap);
		_scrollBar->setPreferredSize(s);
		_scrollBar->setPosition(p);
//CCLog("LTV vScroll.size=%d,%d vh=%d ch=%d", (int)s.width, (int)s.height, (int)vs.height, (int)cs.height);
	}
	if(_scrollTrack){
		_scrollTrack->setVisible(v);
		if(v){
			CCSize s = _scrollTrack->getPreferredSize();
			if(vert){
				p.x = vs.width + _scrollOffset;
				s.height = vs.height;
			}else{
				p.y = vs.height + _scrollOffset;
				s.width = vs.width;
			}
			_scrollTrack->setAnchorPoint(ap);
			_scrollTrack->setPreferredSize(s);
			_scrollTrack->setPosition(p);
		}
	}
//CCLog("LTV reload vscr=%x visible=%d", _scroller, _scrollBar->isVisible());
	updateScroll();
}
