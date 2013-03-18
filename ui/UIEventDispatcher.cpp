#include "UIEventDispatcher.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
UIEventDispatcher * UIEventDispatcher::_instance = NULL;
UIEventDispatcher * UIEventDispatcher::sharedDispatcher(){
	if(!_instance){
		_instance = new UIEventDispatcher();
		_instance->autorelease();
		_instance->retain();
	}
	return _instance;
}
UIEventDispatcher::UIEventDispatcher(){
}
void UIEventDispatcher::win32Key(UINT m, WPARAM w, LPARAM l){
	for(vector<UIEventDelegate *>::iterator i = _listener.begin(); i != _listener.end(); i ++)
		(*i)->keyEvent(m, w, l);
}
void UIEventDispatcher::addListener(UIEventDelegate *l){
	_listener.push_back(l);
}
void UIEventDispatcher::removeListener(UIEventDelegate *l){
	for(vector<UIEventDelegate *>::iterator i = _listener.begin(); i != _listener.end(); i ++)
		if(*i == l){
			_listener.erase(i);
			break;
		}
}
void UIEventDispatcherHook(UINT m, WPARAM w, LPARAM l){
	UIEventDispatcher::sharedDispatcher()->win32Key(m, w, l);
}
#endif
