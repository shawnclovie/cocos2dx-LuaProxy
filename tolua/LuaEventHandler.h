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

#ifndef __LUAEVENTHANDLER_H_
#define __LUAEVENTHANDLER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

typedef enum {
	kLuaEventKeyBack,
	kLuaEventKeyMenu,
	kLuaEventAppEnterBackground,
	kLuaEventAppEnterForeground
} LuaEvents;

class LuaEventHandler : public CCLayer, public CCBAnimationManagerDelegate,
public CCTableViewDelegate, public CCTableViewDataSource
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
,public CCEditBoxDelegate
#endif
{
protected:
	lua_State *_lua;
	std::string _typename;
	int _handler;
	bool _multiTouches;
	int _priority;
	bool _swallows;
	CCBAnimationManager *_aniMGR;
	LuaEventHandler(){
		_aniMGR = NULL;
		_lua = NULL;
	}
public:
	static LuaEventHandler * app;
	~LuaEventHandler(){}
	static LuaEventHandler * createAppHandler(lua_State *l, int handler);
	static LuaEventHandler * create(lua_State *l);
	// Handle with function, for layer event, allow other three options
	LuaEventHandler * handle(int handler, bool multiTouches = false, int priority = 0, bool swallows = false);
	// Handle for CCBAnimationManager with optional function
	LuaEventHandler * handle(CCBAnimationManager *m, int handler = 0);
	// Handle for CCHttpRequest with optional function
	LuaEventHandler * handle(CCHttpRequest *req, int handler = 0);
	inline void unhandle();
	int getHandler();
	LuaEventHandler * setTypename(const char *n);
	void executeHandler(unsigned int argNum = 0);

	// Application enter background or foreground
	void applicationStateChange(LuaEvents e);
	// CCControl event
	void controlAction(CCObject *sender, CCControlEvent e);
	// Default event recever
	void action(CCObject *sender);
	// An animation of ccb complated
	void completedAnimationSequenceNamed(const char *n);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	virtual void editBoxEditingDidBegin(CCEditBox *eb);
	virtual void editBoxEditingDidEnd(CCEditBox *eb);
	virtual void editBoxTextChanged(CCEditBox *eb);
	virtual void editBoxReturn(CCEditBox *eb);
	void editBoxEvent(const char *e, CCEditBox *eb);
#endif
	// CCTableView events
	virtual CCSize cellSizeForTable(CCTableView *t);
	virtual CCTableViewCell * tableCellAtIndex(CCTableView *t, unsigned int i);
	virtual unsigned int numberOfCellsInTableView(CCTableView *t);
	virtual void tableCellTouched(CCTableView *t, CCTableViewCell *c);
	virtual void scrollViewDidScroll(CCScrollView *s);
	virtual void scrollViewDidZoom(CCScrollView *s);
	virtual void keyBackClicked();
	virtual void keyMenuClicked();

	// Http Event
	void onHttpResponse(CCNode *sender, void *data);
	
	void onIAPProductList(CCDictionary *prods);
	void onIAPBuy(bool success, const char *key, const char *errMsg = NULL, int errCode = 0);
	void onIAPRestore(CCArray *prodIds);
};

class LuaCallFuncInterval : public CCActionInterval{
public:
	int _handler;
	LuaCallFuncInterval() : _handler(0){}
	~LuaCallFuncInterval(){}
	static LuaCallFuncInterval * create(float dur, int handler);
	virtual void startWithTarget(CCNode *tar);
	virtual void update(float time);
};
#endif