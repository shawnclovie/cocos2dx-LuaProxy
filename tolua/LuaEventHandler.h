#pragma once
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

class LuaEventHandler : public CCLayer, public CCBAnimationManagerDelegate, public CCTableViewDelegate, public CCTableViewDataSource{
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
	LuaEventHandler * handle(int handler, bool multiTouches = false, int priority = 0, bool swallows = false);
	LuaEventHandler * handle(CCBAnimationManager *m, int handler = 0);
	inline void unhandle();
	int getHandler();
	LuaEventHandler * setTypename(const char *n);
	void executeHandler(unsigned int argNum = 0);
	void applicationStateChange(LuaEvents e);
	void controlAction(CCObject *sender, CCControlEvent e);
	void action(CCObject *sender);
	void completedAnimationSequenceNamed(const char *n);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	virtual void editBoxEditingDidBegin(CCEditBox *eb);
	virtual void editBoxEditingDidEnd(CCEditBox *eb);
	virtual void editBoxTextChanged(CCEditBox *eb);
	virtual void editBoxReturn(CCEditBox *eb);
	void editBoxEvent(const char *e, CCEditBox *eb);
#endif
	virtual CCSize cellSizeForTable(CCTableView *t);
	virtual CCTableViewCell * tableCellAtIndex(CCTableView *t, unsigned int i);
	virtual unsigned int numberOfCellsInTableView(CCTableView *t);
	virtual void tableCellTouched(CCTableView *t, CCTableViewCell *c);
	virtual void scrollViewDidScroll(CCScrollView *s);
	virtual void scrollViewDidZoom(CCScrollView *s);
	virtual void keyBackClicked();
	virtual void keyMenuClicked();
	void onIAPProductList(CCDictionary *prods);
	void onIAPBuy(bool success, const char *key, const char *errMsg = NULL, int errCode = 0);
	void onIAPRestore(CCArray *prodIds);
};

#endif