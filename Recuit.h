#ifndef RECUIT_H
#define RECUIT_H

#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

class Recuit
{
    public:
        Recuit(char* name, cv::Point2d point, char* output);
        virtual ~Recuit();
        void recuit(double tau0);
        double getInitialTemp(double tau0);
        void swp(int var, int valeur);
        double cost();
        void draw();
    protected:
    private:
    	vector<int> line;
    	cv::Mat image;
    	vector<int> sol;
    	cv::Point2d pt;
    	char* out;

};

#endif // RECUIT_H