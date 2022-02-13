#ifndef _MYAPPSTATEMACHINE_H_
#define _MYAPPSTATEMACHINE_H_

#include "suloki.h"
#include "myhandler.h"

//global statemachine
class MyAppStateMachine : public Suloki::AppStateMachine
{
public:
	MyAppStateMachine();
	virtual ~MyAppStateMachine();
	virtual Suloki::Ret Init(void);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Run(void);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
private:
	MyAppStateMachine(MyAppStateMachine& ref) {}
	MyAppStateMachine& operator=(MyAppStateMachine& ref) { return *this; }
};
typedef Suloki::Singleton<MyAppStateMachine> MyAppStateMachineSingleton;

#endif