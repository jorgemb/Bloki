#include "StateManager.h"

///
// State
///

State::State():
m_pParent( NULL )
{
}

State::~State()
{
}

///
// StateManager
///

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
	// Delete all managed states pointers
	while( !m_States.empty() )
	{
		StateDef currentState = m_States.top();

		currentState.pState->onEnd();
		killState( currentState );
		m_States.pop();
	}

	// Delete all states flagged to be deleted
	while( !m_ToDeleteStates.empty() )
	{
		StateDef currentState = m_ToDeleteStates.front();
		killState( currentState );
		m_ToDeleteStates.pop();
	}
}

BOOL StateManager::pushState( State *newState, BOOL managePointer )
{
	// Validate
	if( !newState )
		return FALSE;

	// Initialize state
	newState->m_pParent = this;
	if( !newState->onStart() )
		return FALSE;

	// Push the new state
	StateDef SDef;
	SDef.pState			= newState;
	SDef.managePointer	= managePointer;
	m_States.push( SDef );

	return TRUE;
}

BOOL StateManager::popState()
{
	// Validate
	if( m_States.empty() )
		return FALSE;

	// Get the state and clean up
	StateDef SDef = m_States.top();
	BOOL ret = SDef.pState->onEnd();
	SDef.pState->m_pParent = NULL;

	// Push the state to the ToDelete queue
	m_ToDeleteStates.push( SDef );

	// Pop the state
	m_States.pop();

	return ret;
}

BOOL StateManager::popAndPushState( State* newState, BOOL managePointer )
{
	// Validate
	if( m_States.empty() )
		return FALSE;

	popState();
	return pushState( newState, managePointer );
}

BOOL StateManager::Update( DWORD timeDelta )
{
	// Validate
	if( !isValid() )
		return FALSE;

	// Kill any state flagged to be deleted
	while( !m_ToDeleteStates.empty() )
	{
		StateDef SDef = m_ToDeleteStates.front();
		killState( SDef );
		m_ToDeleteStates.pop();
	}

	// Update the current state
	return m_States.top().pState->onStep( timeDelta );
}

BOOL StateManager::isValid()
{
	return !m_States.empty();
}

VOID StateManager::killState( StateDef& SDef )
{
	if( SDef.managePointer )
		S_DELETE( SDef.pState );
}
