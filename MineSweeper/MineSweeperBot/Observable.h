#pragma once

#include "Observer.h"


class Observable
{
public:
	void addObserver(Observer* observer)
	{
		m_Observer = observer;
	}
	void notifyUpdate()
	{
		m_Observer->Update();
	}
private:
	Observer* m_Observer;
};
