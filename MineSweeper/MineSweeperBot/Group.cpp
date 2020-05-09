#include "Group.h"

#include <cmath>
#include <iomanip>




Group::Group(PtrFields Field, Type FieldRow, Type FieldCol):
	m_BombNumber(-1)
{
	if (!Field)
	{
		return;
	}

	if ((*Field)[FieldRow][FieldCol] == 9 || (*Field)[FieldRow][FieldCol] == 10)
	{
		return;
	}

	m_BombNumber = (*Field)[FieldRow][FieldCol];

	Type limitLeft		= (FieldCol == 0)				? 0 : -1;
	Type limitRight		= (FieldCol + 1 == ColsCount)	? 1 :  2;
	Type limitTop		= (FieldRow == 0)				? 0 : -1;
	Type limitBottom	= (FieldRow + 1 == RowsCount)	? 1 :  2;

	for (Type row = limitTop; row < limitBottom; row++)
	{
		for (Type col = limitLeft; col < limitRight; col++)
		{
			if (row == 0 && col == 0)
			{
				continue;
			}

			if ((*Field)[row + FieldRow][col + FieldCol] == 9)
			{
				m_BombNumber--;
			}

			if ((*Field)[row + FieldRow][col + FieldCol] == 10)
			{
				m_Fields.push_back(std::make_pair(row + FieldRow, col + FieldCol));
			}
		}
	}

	SortList();
}



bool Group::operator==(const Group& grp)const
{
	return (m_BombNumber == grp.m_BombNumber) && (m_Fields == grp.m_Fields);
}



bool Group::operator!=(const Group& grp) const
{
	return !(operator==(grp));
}



bool Group::operator<(const Group& grp) const
{
	if (m_BombNumber != grp.m_BombNumber)
	{
		return m_BombNumber < grp.m_BombNumber;
	}

	if (m_Fields.size() != grp.m_Fields.size())
	{
		return m_Fields.size() < grp.m_Fields.size();
	}

	return m_Fields < grp.m_Fields;
}



Group& Group::operator-=(const Group& grp)
{
	if (!includes(grp))
	{
		return *this;
	}

	m_BombNumber -= grp.m_BombNumber;

	for (auto& Field : grp.m_Fields)
	{
		m_Fields.erase(std::remove(m_Fields.begin(), m_Fields.end(), Field), m_Fields.end());
	}

	SortList();

	return *this;
}

Group::operator bool()
{
	return !isEmpty();
}



bool Group::includes(const Group& grp) const
{
	return std::includes(m_Fields.cbegin(), m_Fields.cend(), grp.m_Fields.cbegin(), grp.m_Fields.cend());
}



bool Group::isEmpty() const
{
	return m_BombNumber < 0 || m_Fields.empty();
}



bool Group::isAllBombs() const
{
	return m_BombNumber == m_Fields.size() && m_Fields.size() > 0;
}



bool Group::isAllFreeFields() const
{
	return m_BombNumber == 0 && m_Fields.size() > 0;
}



void Group::AddField(const FieldCoords& Field)
{
	m_Fields.push_back(Field);

	SortList();
}

std::shared_ptr<const std::list<FieldCoords>> Group::GetFieldsPtr()
{
	return std::make_shared<const std::list<FieldCoords>>(m_Fields);
}



void Group::SortList()
{
	m_Fields.sort();
	m_Fields.unique();
}



Group FindIntersection(const Group& grp1, const Group& grp2)
{

	if (grp1.isEmpty() || grp2.isEmpty())
	{
		return Group();
	}

	if (grp1 == grp2)
	{
		return grp1;
	}

	decltype(grp1.m_Fields) Result;

	std::set_intersection(	grp1.m_Fields.begin(), grp1.m_Fields.end(),
							grp2.m_Fields.begin(), grp2.m_Fields.end(),
							std::back_inserter(Result)
						 );

	if (Result.empty())
	{
		return Group();
	}



	auto b1 = grp1.m_BombNumber + static_cast<Type>(Result.size()) - static_cast<Type>(grp1.m_Fields.size());
	auto b2 = grp2.m_BombNumber + static_cast<Type>(Result.size()) - static_cast<Type>(grp2.m_Fields.size());

	auto b = std::max(b1, b2);

	if (b == grp1.m_BombNumber || b == grp2.m_BombNumber)
	{
		Group Res;
		Res.m_BombNumber = b;
		Res.m_Fields = std::move(Result);

		return Res;
	}

	return Group();
}


std::ostream& operator<<(std::ostream& out, const Group& group)
{
	for (auto& Field : group.m_Fields)
	{
		out << "(" << std::setw(2) << Field.first << ", " << std::setw(2) << Field.second << ")  ";
	}

	out << "   b = " << std::setw(2) << group.m_BombNumber;

	return out;
}


