#pragma once



#include "GameSettings.hpp"
#include "Observable.h"

#include "Group.h"

#include <array>
#include <memory>



class Model : public Observable
{
public:
	Model();

	PtrFields  GetFields() const;

	void UpdateFields(PtrFields ptrFields);


	std::shared_ptr<std::list<FieldCoords>> FindBombs();
	std::shared_ptr<std::list<FieldCoords>> FindFreeFields();

	std::shared_ptr<std::list<FieldCoords>> FindAnything();


protected:
	void DeleteDuplicates(std::shared_ptr<std::list<Group>> List);
	void DeleteEmptyGroups(std::shared_ptr<std::list<Group>> List);
	bool ReCalcIncludes(std::shared_ptr<std::list<Group>> List);

	bool CalcIntersections(std::shared_ptr<std::list<Group>> List);

	std::shared_ptr<std::list<Group>>	FindGroups();
	std::shared_ptr<std::list<Group>>	CalcGroups();



protected:
	PtrFields m_PtrField;

	std::shared_ptr <std::list<Group>> m_FieldsGroups;
};