#include "Recuit.h"

#define T_INIT 6666666
#define T_STEP 0.9
#define T_STOP 0.00001

Recuit::Recuit(char* name, cv::Point2d point, char* output)
{
	this->image = cv::imread(name,CV_LOAD_IMAGE_COLOR);
	this->pt = point;
	std::uniform_int_distribution<int> dist(-5,5);
	std::default_random_engine generator;
	for (int i = 0; i < this->image.cols; i++)
	{
		if (i == pt.y)
			this->line.push_back(pt.x);
		else
		{
			cv::Vec3b pix = image.at<cv::Vec3b>(pt.x, i);
			if (pix.val[0] != 255 && pix.val[1] != 255 && pix.val[2] != 255)
                this->line.push_back(getWhitePoint(i));
			else
				this->line.push_back(pt.x);
		}
	}
    this->marge = max(getLineMarge(),20);
	this->sol = this->line;
	this->out = output;
}

int Recuit::getLineMarge()
{
    int marge = 0;
    bool cont = true;
    int margeup = 0;
    int margedown = 0;
    int count = 0;
    for (int i = pt.x-3; i > 0 && cont; i--)
    {
        count++;
        cv::Vec3b pix = image.at<cv::Vec3b>(i, pt.y);
        if (pix.val[0] == 255 && pix.val[1] == 255 && pix.val[2] == 255)
        {
            cont = false;
            margeup = count;
        }
    }
    cont = true;
    count = 0;
    for (int i = pt.x+3; i < image.rows && cont; i++)
    {
        count++;
        cv::Vec3b pix = image.at<cv::Vec3b>(i, pt.y);
        if (pix.val[0] == 255 && pix.val[1] == 255 && pix.val[2] == 255)
        {
            cont = false;
            margedown = count;
        }
    }
    if (abs(pt.x-margeup) > abs(pt.x-margedown))
        marge = margeup;
    else
        marge = margedown;
    return marge;
}

int Recuit::getWhitePoint(int col)
{
    int line = pt.x;
    bool cont = true;
    int lineup = 0;
    int linedown = 0;
    for (int i = pt.x; i > 0 && cont; i--)
    {
        cv::Vec3b pix = image.at<cv::Vec3b>(i, col);
        if (pix.val[0] == 255 && pix.val[1] == 255 && pix.val[2] == 255)
        {
            cont = false;
            lineup = i;
        }
    }
    cont = true;
    for (int i = pt.x; i < image.rows && cont; i++)
    {
        cv::Vec3b pix = image.at<cv::Vec3b>(i, col);
        if (pix.val[0] == 255 && pix.val[1] == 255 && pix.val[2] == 255)
        {
            cont = false;
            linedown = i;
        }
    }
    if (abs(pt.x-lineup) < 100 || abs(pt.x-linedown) < 100)
    {
        if (abs(pt.x-lineup) < abs(pt.x-linedown))
            line = lineup;
        else
            line = linedown;
    }
    return line;
}


Recuit::~Recuit()
{
}

double Recuit::cost(double* col, double* decalage, bool firstpart)
{
	double cost = 0;
    int param = 3;
    *col = 0;
    *decalage = 0;
    if (firstpart)
    {
        for (unsigned int i = 0; i < pt.y; i++)
        {
            cv::Vec3b pix = image.at<cv::Vec3b>(this->line.at(i), i);
            //std::cout << "(" << pix.val[0] << "," << pix.val[1] << "," << pix.val[2] << ")" << std::endl;
            if (pix.val[0] != 255 && pix.val[1] != 255 && pix.val[2] != 255)
                *col = *col + 7;
        }
        cost = cost + *col;

        for (unsigned int i = param+1; i < pt.y-param; i++)
        {
            for (int j = -param; j <= param; j++)
                *decalage = *decalage + /*4**/abs(this->line.at(i-j-1)-this->line.at(i));
        }
        cost = cost + *decalage;
        cost = cost + 5*abs(this->pt.x - this->line.at(0));
    }
    else
    {
        for (unsigned int i = pt.y; i < this->line.size(); i++)
        {
            cv::Vec3b pix = image.at<cv::Vec3b>(this->line.at(i), i);
            //std::cout << "(" << pix.val[0] << "," << pix.val[1] << "," << pix.val[2] << ")" << std::endl;
            if (pix.val[0] != 255 && pix.val[1] != 255 && pix.val[2] != 255)
                *col = *col + 7;
        }
        cost = cost + *col;

        for (unsigned int i = pt.y+param+1; i < this->line.size()-param; i++)
        {
            for (int j = -param; j <= param; j++)
                *decalage = *decalage + 4*abs(this->line.at(i-j-1)-this->line.at(i));
        }
        cost = cost + *decalage;
        cost = cost + 5*abs(this->pt.x - this->line.at(this->line.size()-1));
    }
	return cost;
}

void Recuit::swp(int var, int valeur)
{
	int val = this->line.at(var);
	if (val + valeur < this->image.rows && val + valeur >= 0)
		this->line.at(var) = val + valeur;
}

void Recuit::draw()
{
	cv::Mat temp;
	this->image.copyTo(temp);
	for (unsigned int i = 0; i < this->sol.size(); i++)
	{
		cv::Vec3b pix(0,0,255);
		temp.at<cv::Vec3b>(this->sol.at(i),i) = pix;
	}
	cv::imwrite(this->out, temp);
}

double Recuit::getInitialTemp(double tau0)
{
    double res = 0;
    std::uniform_int_distribution<int> distrib(0,this->line.size()-1);
    std::uniform_int_distribution<int> dist(-5,5);
    std::default_random_engine generator;
    double col = 0;
    double decalage = 0;
    for (unsigned int i = 0; i < 100; i++)
    {
        double value = dist(generator);
        double var = 0;
        do
        {
        		var = distrib(generator);
        }
		  while (pt.y == var) ;
        double firstcost = this->cost(&col, &decalage,true);
        this->swp(var, value);
        double secondcost = this->cost(&col, &decalage,true);
        res = res + abs(firstcost-secondcost);
    }
    res = res / 100;
    double T0 = - res / log(tau0);
    std::cout << "res = " << res << " , log = " << 100 * log(tau0) << std::endl;
    return T0;
}

void recuitThread(Recuit* rec, bool firstpart)
{
    cv::Point2d pt = rec->getPoint();
    double best_cost = 0;
    double col = 0;
    double decalage = 0;
    best_cost = rec->cost(&col, &decalage,firstpart);
    rec->setSol(rec->getLine());
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    std::uniform_int_distribution<int> distrib;
    int size = 0;
    if (firstpart)
    {
        std::uniform_int_distribution<int> d(0,pt.y);
        distrib = d;
        size = pt.y;
    }
    else
    {
        std::uniform_int_distribution<int> d(pt.y,rec->getLine().size()-1);
        distrib = d;
        size = rec->getLine().size() - pt.y;
    }
    std::uniform_int_distribution<int> dist(-rec->getMarge(),rec->getMarge());
    bool changeInBest = false;
    double best_col = 0;
    double best_decalage = 0;

    double T0 = /*this->getInitialTemp(tau0)*/10;
    double T = T0;
    std::cout << "T = " << T << std::endl;
    unsigned int t = 0, nbiter = 0 ;
    double cost_i, cost_j;
    int delta;
    unsigned int var, value;
    srand (time(NULL));
    bool cont = true;
    int accept = 0;
    int acceptdelta = 0;
    int palierSansAccept = 0;
    double rnd = 0;
    while (T > T_STOP && cont)
    {
        while (t != 100*size && (accept+acceptdelta) != 12*size && cont)
        {
            t++ ;
            nbiter++ ;
            //Compute inititial cost
            cost_i = rec->cost(&col, &decalage,firstpart);
            //Pick up two aleatory pieces
            value = dist(generator);
            do
            {
                var = distrib(generator);
            }
            while (pt.y == var);
            rec->swp(var, value);
            //Compute new cost
            cost_j = rec->cost(&col, &decalage,firstpart);
            if (cost_j < best_cost)
            {
                best_cost = cost_j;
                best_col = col;
                best_decalage = decalage;
                rec->setSol(rec->getLine());
                changeInBest = true;
            }
            // Back up best solution
            delta = cost_j - cost_i;
            if (delta < 0)
            {
                //std::cout << "accept" << std::endl;
                accept++; //Accept the swap
                //std::cout << delta << " " << cost_j << " " << cost_i << " " << best_cost << " T = " << T << std::endl;
            }
            else if (delta > 0)
            {
                double p = distribution(generator);
                double e = exp(-delta / T);
                rnd = rnd + e;
                if (p < e)
                {
                    //std::cout << "accept" << std::endl;
                    acceptdelta++; //Accept the swap
                    //std::cout << delta << " " << cost_j << " " << cost_i << " " << best_cost << " T = " << T << std::endl;
                }
                else
                    rec->swp(var, -value);; //Refuse the swap, so we reput the last conﬁguration
            }
        }
        if (changeInBest)
            rec->draw();
        std::cout << "T : " << T << " , t : " << t << " , nbiter : " << nbiter << " , accept : " << accept << " , acceptdelta : " << acceptdelta << " , best_col : " << best_col << " , best_decalage : "<< best_decalage << " , best_cost : " << best_cost << std::endl;
        if (accept+acceptdelta == 0)
            palierSansAccept++;
        else
            palierSansAccept = 0;
        T *= T_STEP ;
        t = 0 ;
        accept = 0;
        acceptdelta = 0;
        rnd = 0;
        changeInBest = false;
        if (palierSansAccept == 5)
            cont = false;
    }
    if (cont)
        std::cout << best_cost << " true" << std::endl;
    else
        std::cout << best_cost << " false" << std::endl;
}

void Recuit::setLine(vector<int> line)
{
    this->line = line;
}

void Recuit::setSol(vector<int> sol)
{
    this->sol = sol;
}

void Recuit::setPoint(cv::Point2d point)
{
    this->pt = point;
}

vector<int> Recuit::getLine()
{
    return this->line;
}

vector<int> Recuit::getSol()
{
    return this->sol;
}

cv::Point2d Recuit::getPoint()
{
    return this->pt;
}

int Recuit::getMarge()
{
    return this->marge;
}

void Recuit::recuit(double tau0)
{
    std::thread t1(recuitThread, this, true);
    std::thread t2(recuitThread, this, false);
    t1.join();
    t2.join();
    this->line = this->sol;
    std::cout << "end recuit" << std::endl;
    this->draw();
    for(unsigned int i = 0; i < this->line.size(); i++)
    	std::cout << "(" << this->line.at(i) << "," << i << ") ";
    std::cout << std::endl;
}
