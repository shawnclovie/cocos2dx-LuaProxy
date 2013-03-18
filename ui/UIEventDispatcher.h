#ifndef __UIEVENTDISPATCHER_H__
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define __UIEVENTDISPATCHER_H__
#include "cocos2d.h"
USING_NS_CC;
using namespace std;
class UIEventDelegate{
public:
	virtual void keyEvent(UINT m, WPARAM w, LPARAM l) = 0;
};

class UIEventDispatcher : public CCObject{
protected:
	vector<UIEventDelegate *> _listener;
	static UIEventDispatcher *_instance;
public:
	static UIEventDispatcher * sharedDispatcher();
	UIEventDispatcher();
	~UIEventDispatcher(){}
	void win32Key(UINT m, WPARAM w, LPARAM l);
	void addListener(UIEventDelegate *l);
	void removeListener(UIEventDelegate *l);
};
void UIEventDispatcherHook(UINT m, WPARAM w, LPARAM l);

#endif
#endif //__UIEVENTDISPATCHER_H__