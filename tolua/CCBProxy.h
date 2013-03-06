#pragma once
#ifndef __CCBPROXY_H_
#define __CCBPROXY_H_
#include "cocos2d.h"
#include "CCLuaEngine.h"
#include "SimpleAudioEngine.h"
#include "cocos-ext.h"
#include "LuaEventHandler.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCBProxy : public CCLayer, public CCBScriptOwnerProtocol, public CCBSelectorResolver, public CCBMemberVariableAssigner{
protected:
	lua_State *_lua;
	CCDictionary *_memVars;
	CCArray *_handlers;
	LuaEventHandler *_selectorHandler;
public:
	CCBProxy();
	~CCBProxy();
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(CCBProxy, create);
	void releaseMembers();
	CCBProxy * initProxy(lua_State *l);

	//interface methods
	virtual SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName);
	virtual SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode);
	virtual void onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader);

	//create LuaEventHandler contains a lua function(handler), for handler control's event
	void handleEvent(CCControlButton *n, const int handler, bool multiTouches = false, CCControlEvent e = CCControlEventTouchUpInside);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	void handleEvent(CCEditBox *n, const int handler);
#endif
	void handleEvent(CCBAnimationManager *m, const int handler);
	void handleKeypad(const int handler);
	LuaEventHandler * addHandler(const int handler, bool multiTouches = false);
	LuaEventHandler * getHandler(const int handler);
	// Remove LuaEventHandler for lua function handler, and return it
	LuaEventHandler * removeHandler(LuaEventHandler *h);
	LuaEventHandler * removeFunction(int handler);
	LuaEventHandler * removeKeypadHandler(int handler);

	inline LuaEventHandler * getSelectorHandler(){ return _selectorHandler;}
	void setSelectorHandler(LuaEventHandler *h);
	LuaEventHandler * handleSelector(const int handler);
	
	virtual CCBSelectorResolver * createNew();
	void menuItemCallback(CCObject *pSender);
	void controlCallback(CCObject *pSender, CCControlEvent event);
	void setCallbackProperty(const char *prop);
	
	CCDictionary *getMemberVariables();
	const char * getMemberName(CCObject *n);
	//get member variable in node for key
	CCNode * getNode(const char *n);
	static void nodeToTypeForLua(lua_State *l, CCObject *o, const char *t);
	//read ccbi
	CCNode * readCCBFromFile(const char *, float resolutionScale = 1);
//	void changeResolutionScale(CCNode *n, CCBAnimationManager *m, float resolutionScale);

	//fix label text blur when scaled
	void fixLabel(CCNode *o, const float rate, bool withChild, const char *font = NULL);
	void duplicate(CCScale9Sprite *n, CCScale9Sprite *o);
	void duplicate(CCSprite *n, CCSprite *o);
	void duplicate(CCLabelBMFont *n, CCLabelBMFont *o);
	void duplicate(CCLabelTTF *n, CCLabelTTF *o);
	void duplicate(CCNode *n, CCNode *o);
	void duplicate(CCParticleSystemQuad *n, CCParticleSystemQuad *o);
	void duplicate(CCParticleSystem *n, CCParticleSystem *o);
};

class ProxyLayerLoader : public CCLayerLoader {
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ProxyLayerLoader, loader);
protected:
	CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCBProxy);
};
#endif // __CCBPROXY_H_