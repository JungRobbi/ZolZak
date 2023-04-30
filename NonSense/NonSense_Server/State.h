#pragma once
template <class onwer_class>
class State
{
public:
	virtual ~State() {}
	virtual void Enter(onwer_class* pOwner) = 0;
	virtual void Execute(onwer_class* pOwner) = 0;
	virtual void Exit(onwer_class* pOwner) = 0;
};
