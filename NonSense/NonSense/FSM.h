#pragma once
#include "State.h"
template <class owner_class>
class FSM
{
private:
	owner_class* m_pOwner;
	State<owner_class>* m_pCurrentState;
	State<owner_class>* m_pPreviousState;
public:
	FSM(owner_class* owner)
	{
		m_pOwner = owner;
		m_pCurrentState = m_pPreviousState = NULL;
	}
	~FSM() {};

	void Update()
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Execute(m_pOwner);
		}
	}

	void ChangeState(State<owner_class>* NewState)
	{
		m_pPreviousState = m_pCurrentState;
		m_pCurrentState->Exit(m_pOwner);
		m_pCurrentState = NewState;
		m_pCurrentState->Enter(m_pOwner);
	}

	void SetCurrentState(State<owner_class>* NewState)
	{
		m_pCurrentState = NewState;
		m_pCurrentState->Enter(m_pOwner);
	}
};