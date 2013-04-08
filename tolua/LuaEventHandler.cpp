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

#include "LuaEventHandler.h"
#include "CCLuaEngine.h"
#include "tolua++.h"
#include "tolua_fix.h"

#if COCOS2D_VERSION < 0x00020100
#define LuaStack sharedEngine()
#else
#define LuaStack sharedEngine()->getLuaStack()
#endif

CCLuaEngine * sharedEngine(){
	return (CCLuaEngine *)CCScriptEngineManager::sharedManager()->getScriptEngine();
}

lua_State * luaStateForEngine(CCLuaEngine *le = NULL){
	if(!le){ le = sharedEngine();}
	return LuaStack->getLuaState();
}

bool pushLuaFunction(int nHandler){
	lua_State *l = luaStateForEngine();
    toluafix_get_function_by_refid(l, nHandler);          /* stack: ... func */
    if(!lua_isfunction(l, -1)){
        CCLOG("[LUA ERROR] function refid '%d' does not reference a Lua function", nHandler);
        lua_pop(l, 1);
        return false;
    }
    return true;
}

// Run lua function but do not get return value.
// If (retInt) is true, try to return int and bool value, and auto finish function call.
// Else, should manual call finishRunLuaFunction()
int runLuaFunction(int h, int numArgs, bool retInt = false){
	lua_State *l = luaStateForEngine();
	if(pushLuaFunction(h)){                                         /* stack: ... arg1 arg2 ... func */
		if(numArgs > 0){
			lua_insert(l, -(numArgs + 1));                        /* stack: ... func arg1 arg2 ... */
		}
        
		int traceback = 0;
		lua_getglobal(l, "__G__TRACKBACK__");                     /* stack: ... func arg1 arg2 ... G */
		if (!lua_isfunction(l, -1)){
			lua_pop(l, 1);                                        /* stack: ... func arg1 arg2 ... */
		}else{
			traceback = -(numArgs + 2);
			lua_insert(l, traceback);                             /* stack: ... G func arg1 arg2 ... */
		}
        
		int error = 0;
		error = lua_pcall(l, numArgs, 1, traceback);              /* stack: ... ret */
		if (error){
			if (traceback == 0){
				CCLOG("[LUA ERROR] %s", lua_tostring(l, - 1));    /* stack: ... error */
				lua_pop(l, 1); // remove error message from stack
			}
			return 0;
		}
        
		// get return value
		int ret = 1;
		if(retInt){
			if (lua_isnumber(l, -1)){
				ret = lua_tointeger(l, -1);
			}else if (lua_isboolean(l, -1)){
				ret = lua_toboolean(l, -1);
			}
			lua_pop(l, 1); // remove return value from stack
		}
		return ret;
	}else{
		lua_pop(l, numArgs); // remove args from stack
		return 0;
	}
}

// Remove lua function return value from stack
void finishRunLuaFunction(lua_State *l){
	lua_pop(l, 1);
}

LuaEventHandler * LuaEventHandler::app = NULL;

LuaEventHandler * LuaEventHandler::createAppHandler(lua_State *l, int handler){
	if(!app){
		app = new LuaEventHandler();
	}
	app->_lua = l;
	app->handle(handler);
	return app;
}

LuaEventHandler * LuaEventHandler::create(lua_State *l){
	LuaEventHandler *h = new LuaEventHandler();
	h->_lua = l;
	h->autorelease();
	return h;
}

LuaEventHandler * LuaEventHandler::handle(int handler, bool multiTouches, int priority, bool swallows){
	this->unhandle();
	this->_handler = handler;
	this->_multiTouches = multiTouches;
	this->_priority = priority;
	this->_swallows = swallows;
	return this;
}

LuaEventHandler * LuaEventHandler::handle(CCBAnimationManager *m, int handler){
	if(handler > 0){
		this->unhandle();
		_handler = handler;
	}
	_aniMGR = m;
	m->setDelegate(this);
	return this;
}

LuaEventHandler * LuaEventHandler::handle(CCHttpRequest *req, int handler){
	if(req){
		if(handler > 0){ handle(handler);}
		req->setResponseCallback(this, callfuncND_selector(LuaEventHandler::onHttpResponse));
		CCHttpClient::getInstance()->send(req);
		req->release();
	}
	return this;
}

void LuaEventHandler::unhandle(){
	this->_handler = 0;
}
int LuaEventHandler::getHandler(){
	return _handler;
}
LuaEventHandler * LuaEventHandler::setTypename(const char *n){
	_typename = n;
	return this;
}
void LuaEventHandler::executeHandler(unsigned int argNum){
	LuaStack->executeFunctionByHandler(_handler, argNum);
}

void LuaEventHandler::applicationStateChange(LuaEvents e){
	if(_handler){
		LuaStack->pushInt(e);
		LuaStack->executeFunctionByHandler(_handler, 1);
	}
}

void LuaEventHandler::controlAction(CCObject *sender, CCControlEvent e){
	if(this->_handler){
		LuaStack->pushCCObject(sender, _typename.empty()? "CCControl" : _typename.c_str());
		LuaStack->pushInt(e);
		LuaStack->executeFunctionByHandler(_handler, 2);
	}
}
void LuaEventHandler::action(CCObject *sender){
	if(this->_handler){
		CCLuaEngine *le = sharedEngine();
		LuaStack->pushCCObject(sender, _typename.empty()? "CCObject" : _typename.c_str());
		LuaStack->executeFunctionByHandler(_handler, 1);
	}
}
void LuaEventHandler::completedAnimationSequenceNamed(const char *n){
	if(_handler){
		LuaStack->pushCCObject(_aniMGR, "CCBAnimationManager");
		LuaStack->pushString(n);
		LuaStack->pushCCObject(this, "LuaEventHandler");
		LuaStack->executeFunctionByHandler(_handler, 3);
	}
}
#ifdef LUAPROXY_CCEDITBOX_ENABLED
void LuaEventHandler::editBoxEditingDidBegin(CCEditBox *eb){
	this->editBoxEvent("begin", eb);
}
void LuaEventHandler::editBoxEditingDidEnd(CCEditBox *eb){
	this->editBoxEvent("end", eb);
}
void LuaEventHandler::editBoxTextChanged(CCEditBox *eb){
	this->editBoxEvent("change", eb);
}
void LuaEventHandler::editBoxReturn(CCEditBox *eb){
	this->editBoxEvent("return", eb);
}
void LuaEventHandler::editBoxEvent(const char *e, CCEditBox *eb){
	if(this->_handler && e){
		lua_pushstring(this->_lua, e);
		lua_pushlightuserdata(this->_lua, eb);
		this->executeHandler(2);
	}
}
#endif
CCSize LuaEventHandler::cellSizeForTable(CCTableView *t){
	CCSize r = CCSizeZero;
	if(_handler){
		LuaStack->pushString("cellSize");
		LuaStack->pushCCObject(t, "CCTableView");
		runLuaFunction(_handler, 2);
		lua_State *l = luaStateForEngine();
		tolua_Error err;
		if(tolua_isusertype(l, -1, "CCSize", 0, &err)){
			CCSize *v = (CCSize *)tolua_tousertype(l, -1, NULL);
			if(v)r = *v;
		}
		finishRunLuaFunction(l);
	}
	return r;
}
CCTableViewCell * LuaEventHandler::tableCellAtIndex(CCTableView *t, unsigned int i){
	CCTableViewCell *cell = t->dequeueCell();
	if(_handler){
		int argNum = 3;
		LuaStack->pushString("cellAtIndex");
		LuaStack->pushCCObject(t, "CCTableView");
		LuaStack->pushInt(i);
		if(cell){
			LuaStack->pushCCObject(cell, "CCTableViewCell");
			argNum ++;
		}
		runLuaFunction(_handler, argNum);
		lua_State *l = luaStateForEngine();
		tolua_Error err;
		if(tolua_isusertype(l, -1, "CCTableViewCell", 0, &err)){
			cell = (CCTableViewCell *)tolua_tousertype(l, -1, NULL);
		}
		finishRunLuaFunction(l);
	}
	return cell;
}
unsigned int LuaEventHandler::numberOfCellsInTableView(CCTableView *t){
	int r = 0;
	if(_handler){
		LuaStack->pushString("numberOfCells");
		LuaStack->pushCCObject(t, "CCTableView");
		r = runLuaFunction(_handler, 2, true);
	}
	return r;
}
void LuaEventHandler::tableCellTouched(CCTableView *t, CCTableViewCell *c){
	if(_handler){
		LuaStack->pushString("cellTouched");
		LuaStack->pushCCObject(t, "CCTableView");
		LuaStack->pushCCObject(c, "CCTableViewCell");
		runLuaFunction(_handler, 3, true);
	}
}
void LuaEventHandler::scrollViewDidScroll(CCScrollView *s){
	if(_handler){
		LuaStack->pushString("scrollViewDidScroll");
		runLuaFunction(_handler, 1, true);
	}
}
void LuaEventHandler::scrollViewDidZoom(CCScrollView *s){
	if(_handler){
		LuaStack->pushString("scrollViewDidZoom");
		runLuaFunction(_handler, 1, true);
	}
}
void LuaEventHandler::keyBackClicked(){
	if(_handler){
		LuaStack->pushInt(kLuaEventKeyBack);
		LuaStack->executeFunctionByHandler(_handler, 1);
	}
}
void LuaEventHandler::keyMenuClicked(){
	if(_handler){
		LuaStack->pushInt(kLuaEventKeyMenu);
		LuaStack->executeFunctionByHandler(_handler, 1);
	}
}

void LuaEventHandler::onHttpResponse(CCNode *sender, void *data){
	if(_handler){
		CCHttpResponse *res = (CCHttpResponse*)data;
		LuaStack->pushCCObject(res, "CCHttpResponse");
		LuaStack->pushCCObject(this, "LuaEventHandler");
		LuaStack->executeFunctionByHandler(_handler, 2);
	}
}

void LuaEventHandler::onIAPProductList(CCDictionary *prods){
	if(_handler){
		if(prods){	LuaStack->pushCCObject(prods, "CCDictionary");}
		else{		LuaStack->pushNil();}
		LuaStack->executeFunctionByHandler(_handler, 1);
	}
}
void LuaEventHandler::onIAPBuy(bool success, const char *key, const char *errMsg, int errCode){
	if(_handler){
		LuaStack->pushBoolean(success);
		LuaStack->pushString(key);
		LuaStack->pushString(errMsg? errMsg : "");
		LuaStack->pushInt(errCode);
		LuaStack->executeFunctionByHandler(_handler, 4);
	}
}
void LuaEventHandler::onIAPRestore(CCArray *pids){
	if(_handler){
		if(pids){	LuaStack->pushCCObject(pids, "CCArray");}
		else{		LuaStack->pushNil();}
		LuaStack->executeFunctionByHandler(_handler, 1);
	}
}

LuaCallFuncInterval * LuaCallFuncInterval::create(float dur, int handler){
	LuaCallFuncInterval *o = new LuaCallFuncInterval();
	o->initWithDuration(dur);
	o->_handler = handler;
	o->autorelease();
	return o;
}

void LuaCallFuncInterval::startWithTarget(CCNode *tar){
	CCActionInterval::startWithTarget(tar);
	if(_handler){
		LuaStack->pushString("start");
		LuaStack->pushCCObject(m_pTarget, "CCNode");
		LuaStack->executeFunctionByHandler(_handler, 2);
	}
}

void LuaCallFuncInterval::update(float time){
	if(_handler){
		LuaStack->pushFloat(time);
		LuaStack->pushCCObject(m_pTarget, "CCNode");
		LuaStack->executeFunctionByHandler(_handler, 2);
	}
}
