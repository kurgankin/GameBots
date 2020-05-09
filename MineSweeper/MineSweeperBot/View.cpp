#include "View.h"

#include <iostream>
#include <iomanip>



View::View(Model* model)
	:m_Model(model)
{
	m_Model->addObserver(this);
}



void View::Update()
{
	auto Fields = m_Model->GetFields();

	if (Fields)
	{
		system("CLS");

		for (std::remove_const<decltype(RowsCount)>::type i { 0 }; i < RowsCount; i++)
		{
			for (std::remove_const<decltype(RowsCount)>::type j{ 0 }; j < ColsCount; j++)
			{
				PrintField((*Fields)[15 - i][j]);
			}
			std::cout << "|" << std::endl;
		}

		std::cout << "-------------------------------------------------------------" << std::endl;
	}
}



void View::PrintField(Type field) const
{
	if (field == 9)
	{
		std::cout << " b";
	}
	else if (field == 10)
	{
		std::cout << "  ";
	}
	else
	{
		std::cout << std::setw(2) << field;
	}
}
