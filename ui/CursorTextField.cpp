#include "CursorTextField.h"

static int _calcCharCount(const char *t){
	int n = 0;
	char ch = 0;
	while(ch = *t){
		CC_BREAK_IF(!ch);
		if(0x80 != (0xC0 & ch))
			++ n;
		++ t;
	}
	return n;
}
const static float DELTA = 20.0f;

CursorTextField::CursorTextField()
: _cursor(NULL)
, _cursorAction(NULL)
, _password(false)
, _maxLength(30){
	CCTextFieldTTF();
}
CursorTextField::~CursorTextField(){
	CC_SAFE_RELEASE(this->_cursor);
	CC_SAFE_RELEASE(this->_cursorAction);
}
CursorTextField * CursorTextField::create(const char *fontName, float fontSize){
	return createWithPlaceHolder("", fontName, fontSize);
}
CursorTextField * CursorTextField::createWithPlaceHolder(const char *ph, const char *fontName, float fontSize){
	CursorTextField *o = new CursorTextField();
	if(o && o->initWithString("", fontName, fontSize)){
		o->autorelease();
		if(ph)
			o->setPlaceHolder(ph);
		o->initCursorSprite(fontSize);
		return o;
	}
	CC_SAFE_DELETE(o);
	return NULL;
}
void CursorTextField::initCursorSprite(int height){
	int column = 4;
	int *pixels = new int[height * column];
	for(int i = 0; i < height * column; i ++)
		pixels[i] = 0xffffffff;
	CCTexture2D *ttr = new CCTexture2D();
	ttr->initWithData(pixels, kCCTexture2DPixelFormat_RGB888, 1, 1, CCSizeMake(column, height));
	delete pixels;
	CCSize s = getContentSize();
	_cursorPos = ccp(0, s.height / 2);
	_cursor = CCSprite::createWithTexture(ttr);
	_cursor->setPosition(_cursorPos);
	_cursor->setVisible(false);
	addChild(_cursor);
	_cursorAction = CCRepeatForever::create((CCActionInterval *)
		CCSequence::create(CCFadeOut::create(0.25f), CCFadeIn::create(0.25f), NULL));
	_cursorAction->retain();
}
void CursorTextField::onEnter(){
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	UIEventDispatcher::sharedDispatcher()->addListener(this);
#endif
	CCTextFieldTTF::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -128, false);
	this->setDelegate(this);
	_cursor->runAction(_cursorAction);
}
void CursorTextField::onExit(){
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	UIEventDispatcher::sharedDispatcher()->removeListener(this);
#endif
	_cursor->stopAction(_cursorAction);
	this->detachWithIME();
	CCTextFieldTTF::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}
bool CursorTextField::ccTouchBegan(CCTouch *t, CCEvent *e){
	this->_touchBeginPos = CCDirector::sharedDirector()->convertToGL(t->getLocationInView());
	return true;
}
void CursorTextField::ccTouchEnded(CCTouch *t, CCEvent *e){
	CCPoint ep = CCDirector::sharedDirector()->convertToGL(t->getLocationInView());
	if(abs(ep.x - this->_touchBeginPos.x) > DELTA || abs(ep.y - this->_touchBeginPos.y) > DELTA){
		this->_touchBeginPos.x = this->_touchBeginPos.y = -1;
	}else
		isInTextField(t)? openIME() : closeIME();
}
CCRect CursorTextField::getRect(){
	CCSize s = &this->_designedSize != NULL? this->_designedSize : getContentSize();
	return CCRectMake(0 - s.width * getAnchorPoint().x, 0 - s.height * getAnchorPoint().y, s.width, s.height);
}
CCSize CursorTextField::getDesignedSize(){return this->_designedSize;}
void CursorTextField::setDesignedSize(CCSize s){this->_designedSize = s;}
bool CursorTextField::isInTextField(CCTouch *t){
	return getRect().containsPoint(convertTouchToNodeSpaceAR(t));
}
// ############################# Delegate functions ###############################
bool CursorTextField::onTextFieldAttachWithIME(CCTextFieldTTF *s){
	if(!this->m_pInputText->empty())
		this->_cursor->setPositionX(getContentSize().width);
	return false;
}
bool CursorTextField::onTextFieldInsertText(CCTextFieldTTF *s, const char *t, int len){
	if(m_pInputText->length() + len > _maxLength)
		return true;
//std::cout<<"CTF.onInsTxt()"<<t<<"\n";
	m_pInputText->append(t);
	this->updateDisplay();
	_cursor->setPositionX(getTextureRect().size.width);
//	this->_cursor->setPositionX(getContentSize().width);
	return true;
}
bool CursorTextField::onTextFieldDeleteBackward(CCTextFieldTTF *s, const char *delText, int len){
	if((int)m_pInputText->length() < len)
		return false;
	for(int i = 0; i < len; i ++)m_pInputText->pop_back();
	this->updateDisplay();
	this->_cursor->setPositionX(m_pInputText->empty()? 0 : getContentSize().width);
	return true;
}
bool CursorTextField::onTextFieldDetachWithIME(CCTextFieldTTF *s){return false;}

void CursorTextField::openIME(){
	_cursor->setVisible(true);
	this->attachWithIME();
}
void CursorTextField::closeIME(){
	_cursor->setVisible(false);
	this->detachWithIME();
}
bool CursorTextField::isPassword(){return _password;}
void CursorTextField::setPassword(bool b){_password = b;}
unsigned int CursorTextField::getMaxLength(){return _maxLength;}
void CursorTextField::setMaxLength(unsigned int n){_maxLength = n;}
void CursorTextField::setString(const char *t){
	m_pInputText->replace(0, m_pInputText->length(), t? t : "");
	this->updateDisplay();
}
void CursorTextField::updateDisplay(){
	const char *s = m_pInputText->length() == 0? m_pPlaceHolder->c_str() :
		(_password? NULL : m_pInputText->c_str());
	if(_password){
		std::string ts;
		for(unsigned int i = 0; i < m_pInputText->size(); i ++)
			ts.append("*");
		s = ts.c_str();
	}
//std::cout<<"CTF.upDisp()lbl.setString(s)"<<s;
	CCLabelTTF::setString(s);
}
void CursorTextField::setColor(const ccColor3B& c){
	m_sColor = m_sColorUnmodified = c;
	if(m_bOpacityModifyRGB){
		m_sColor.r = c.r * m_nOpacity/255.0f;
		m_sColor.g = c.g * m_nOpacity/255.0f;
		m_sColor.b = c.b * m_nOpacity/255.0f;
	}
	updateColor();
	_cursor->setColor(c);
}
void CursorTextField::keyEvent(UINT m, WPARAM w, LPARAM l){
	if(!_cursor->isVisible())
		return;
	if(m == WM_CHAR){
//CCLOG("CursorTextField.keyEvent %d %d %c", m, w, w);
		if(VK_BACK == w){
			this->onTextFieldDeleteBackward(this, NULL, 1);
		}else if(VK_RETURN == w){
			
		}else{
			char s[2];
			sprintf(s, "%c", w);
			this->onTextFieldInsertText(this, s, 1);
		}
	}
	
}
