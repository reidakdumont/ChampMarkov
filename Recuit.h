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
#include <thread>
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
        double cost(double *col, double *decalage, bool firstpart);
        void draw();
        int getWhitePoint(int col);
        int getLineMarge();
        void setLine(vector<int> line);
        void setSol(vector<int> sol);
        void setPoint(cv::Point2d point);
        vector<int> getLine();
        vector<int> getSol();
        cv::Point2d getPoint();
        int getMarge();
    protected:
    private:
    	vector<int> line;
    	cv::Mat image;
    	vector<int> sol;
        int marge;
    	cv::Point2d pt;
    	char* out;

};

#endif // RECUIT_H
