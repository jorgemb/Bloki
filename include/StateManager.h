#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <stack>
#include <queue>
#include "Window.h"

class State{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	State();
	virtual ~State();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	virtual BOOL onStep( DWORD timeDelta ) = 0;
	
	virtual BOOL onStart() = 0;
	virtual BOOL onEnd() = 0;

	friend class StateManager;
protected:
	StateManager*		m_pParent;
};

class StateManager{
public:
// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR AND DESTRUCTOR ////////////////////////////////////////////////////////

	StateManager();
	~StateManager();

	// UTILITIES /////////////////////////////////////////////////////////////////////////

	BOOL pushState( State* newState, BOOL managePointer = TRUE );
	BOOL popState();
	BOOL popAndPushState( State* newState, BOOL managePointer = TRUE );

	BOOL isValid();
	BOOL Update( DWORD timeDelta );

private:
// TYPES /////////////////////////////////////////////////////////////////////////////////
	struct StateDef
	{
		BOOL		managePointer;
		State*		pState;
	};

// FUNCTIONS /////////////////////////////////////////////////////////////////////////////

	VOID killState( StateDef& SDef );

// VARIABLES /////////////////////////////////////////////////////////////////////////////
	
	std::stack<StateDef>	m_States;
	std::queue<StateDef>	m_ToDeleteStates;
};


#endif // STATEMANAGER_H

