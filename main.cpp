#include "Recuit.h"

int main(int argc, char** argv)
{
	cv::Point2d pt(atoi(argv[2]),atoi(argv[3]));
	Recuit r(argv[1], pt, argv[4]);
	r.draw();
	r.recuit(0.5);
}
