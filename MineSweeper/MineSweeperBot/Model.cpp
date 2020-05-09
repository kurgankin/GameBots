#include "Model.h"

#include <iostream>

#include <random>
#include <ctime>


Model::Model()
{
}



PtrFields  Model::GetFields() const
{
	return m_PtrField;
}



void Model::UpdateFields(PtrFields ptrFields)
{
	m_PtrField = ptrFields;
	m_FieldsGroups = CalcGroups();
	notifyUpdate();
}



std::shared_ptr<std::list<FieldCoords>>  Model::FindBombs()
{
	auto List = std::make_shared<std::list<FieldCoords>>();

	for (auto &grp : *m_FieldsGroups)
	{
		if (grp.isAllBombs())
		{
			auto res = grp.GetFieldsPtr();

			List->insert(List->end(), res->begin(), res->end());
		}
	}

	List->sort();
	List->unique();

	return 	List;
}



std::shared_ptr<std::list<FieldCoords>>  Model::FindFreeFields()
{
	auto List = std::make_shared<std::list<FieldCoords>>();

	if (m_FieldsGroups->empty())
	{
		List->push_back(FieldCoords(7, 15));
		return List;
	}


	for (auto& grp : *m_FieldsGroups)
	{
		if (grp.isAllFreeFields())
		{
			auto res = grp.GetFieldsPtr();

			List->insert(List->end(), res->begin(), res->end());
		}
	}


	List->sort();
	List->unique();

	return 	List;
}

std::shared_ptr<std::list<FieldCoords>> Model::FindAnything()
{
	int BombNumber = 0;
	auto Unknown = std::make_shared<std::list<FieldCoords>>();

	if (!m_PtrField)
	{
		return Unknown;
	}


	for (Type row = 0; row < RowsCount; row++)
	{
		for (Type col = 0; col < ColsCount; col++)
		{
			if ((*m_PtrField)[row][col] == 9)
			{
				BombNumber++;
			}

			if ((*m_PtrField)[row][col] == 10)
			{
				Unknown->push_back(std::make_pair(row, col));
			}
		}
	}

	std::random_device rd;
	std::mt19937 g(rd());

	std::srand(unsigned(std::time(0)));
	int random_variable = std::rand() % Unknown->size();
	for (int i = 0; i < random_variable; i++)
	{
		std::rotate(Unknown->begin(), std::next(Unknown->begin()), Unknown->end());
	}

	//std::shuffle(Unknown->begin(), Unknown->end(), g);

	int unknownBombs = 99 - BombNumber;

	//

	//std::random_shuffle(Unknown->begin(), Unknown->end());

	Unknown->resize(1);

	return Unknown;
}



void Model::DeleteDuplicates(std::shared_ptr<std::list<Group>> List)
{
	List->sort();
	List->unique();
}



void Model::DeleteEmptyGroups(std::shared_ptr<std::list<Group>> List)
{
	auto Lambda = [](const Group& grp) {return grp.isEmpty(); };
	List->erase(std::remove_if(List->begin(), List->end(), Lambda), List->end());
}



bool Model::ReCalcIncludes(std::shared_ptr<std::list<Group>> List)
{
	bool result = false;

	auto FirstItem = List->begin();
	auto SecondItem = List->begin();

	while (FirstItem != List->end())
	{
		while (SecondItem != List->end())
		{
			if (FirstItem == SecondItem)
			{
				++SecondItem;
				continue;
			}

			if (FirstItem->includes(*SecondItem))
			{
				FirstItem->operator-=(*SecondItem);

				FirstItem = --(List->begin());
				SecondItem = List->begin();
				result = true;
				break;
			}
			++SecondItem;
		}

		++FirstItem;
	}

	return result;
}



bool Model::CalcIntersections(std::shared_ptr<std::list<Group>> List)
{
	for (auto FirstItem = List->begin(); FirstItem != List->end(); ++FirstItem)
	{
		for (auto SecondItem = std::next(FirstItem); SecondItem != List->end(); ++SecondItem)
		{
			auto intersection = FindIntersection(*FirstItem, *SecondItem);

			if (intersection)
			{
				*FirstItem -= intersection;
				*SecondItem -= intersection;
				List->push_back(intersection);

				return true;
			}
		}
	}

	return false;
}



std::shared_ptr<std::list<Group>> Model::FindGroups()
{
	auto GroupContainer = std::make_shared<std::list<Group>>();

	if (!m_PtrField)
	{
		return GroupContainer;
	}

	for (Type row = 0; row < RowsCount; row++)
	{
		for (Type col = 0; col < ColsCount; col++)
		{
			Group grp(m_PtrField, row, col);

			if (!grp.isEmpty())
			{
				GroupContainer->push_back(grp);
			}
		}
	}

	return GroupContainer;
}



std::shared_ptr<std::list<Group>> Model::CalcGroups()
{
	auto Result = FindGroups();

	do
	{
		DeleteDuplicates(Result);

		DeleteEmptyGroups(Result);

		if (ReCalcIncludes(Result))
		{
			continue;
		}

		if (CalcIntersections(Result))
		{
			continue;
		}

		break;
	}
	while (1);

	return Result;
}