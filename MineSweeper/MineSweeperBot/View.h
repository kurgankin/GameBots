#pragma once

#include "Model.h"
#include "Observer.h"



class View : public Observer
{

public:
	View(Model *model);

public:

	void Update()override;

protected:
	void PrintField(Type) const;

protected:
	Model* m_Model;
};

