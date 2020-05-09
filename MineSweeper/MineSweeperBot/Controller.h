#pragma once



#include "Model.h"
#include <opencv2/opencv.hpp>



class Controller
{
public:
	Controller(Model* model);

	void		Run					(void);

protected:
	cv::Mat		MakeScreenShot		(void);

	PtrFields	Recognise			(cv::Mat Image);

	cv::Rect	FindWorkinArea		(cv::Mat Image);

	int			FieldIdentification	(cv::Mat Image);

	void		PlaceBombs		(std::shared_ptr<std::list<FieldCoords>> BombsList);
	void		PlaceFreeFields	(std::shared_ptr<std::list<FieldCoords>> FreeFieldsList);

	void		PlaceBomb		(FieldCoords Bomb);
	void		PlaceFreeField	(FieldCoords Field);



	bool isPlaying();
	bool isWinningGame();

	PtrFields ParseImg(cv::Mat Image);

protected:
	Model* m_Model;

	cv::Rect m_WorkingArea;
};

