#pragma once



#include "GameSettings.hpp"


#include <list>
#include <memory>



class Group
{
public:
	Group(PtrFields Field = nullptr, Type FieldRow = 0, Type FieldCol = 0);


	bool	operator==		(const Group& grp)	const;
	bool	operator!=		(const Group& grp)	const;
	bool	operator<		(const Group& grp)	const;

	Group&	operator-=		(const Group& grp);

	operator bool();

	bool	includes		(const Group& grp)	const;
	bool	isEmpty			(void)				const ;

	bool	isAllBombs		()					const;
	bool	isAllFreeFields	()					const;

	void	AddField		(const FieldCoords &Field);

	std::shared_ptr<const std::list<FieldCoords>> GetFieldsPtr();


	friend Group	FindIntersection(const Group&, const Group&);

	friend std::ostream& operator<< (std::ostream& out, const Group& group);

protected:
	void SortList();

protected:
	std::list<FieldCoords> m_Fields;
	Type m_BombNumber;
};