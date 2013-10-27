#include "Recuit.h"

#define T_INIT 6666666
#define T_STEP 0.9
#define T_STOP 0.00001

Recuit::Recuit(char* name, cv::Point2d point, char* output)
{
	this->image = cv::imread(name,CV_LOAD_IMAGE_COLOR);
	this->pt = point;
	std::uniform_int_distribution<int> dist(-4,4);
	std::default_random_engine generator;
	for (int i = 0; i < this->image.cols; i++)
	{
		if (i == pt.y)
			this->line.push_back(pt.x);
		else
		{
			cv::Vec3b pix = image.at<cv::Vec3b>(pt.x, i);
			if (pix.val[0] != 255 && pix.val[1] != 255 && pix.val[2] != 255)
				this->line.push_back(pt.x+ dist(generator));
			else
				this->line.push_back(pt.x);
		}
	}
	this->sol = this->line;
	this->out = output;
}


Recuit::~Recuit()
{
}

double Recuit::cost()
{
	double cost = 0;
	int param = 3;
	for (unsigned int i = 0; i < this->line.size(); i++)
	{
		cv::Vec3b pix = image.at<cv::Vec3b>(this->line.at(i), i);
		//std::cout << "(" << pix.val[0] << "," << pix.val[1] << "," << pix.val[2] << ")" << std::endl;
		if (pix.val[0] != 255 && pix.val[1] != 255 && pix.val[2] != 255)
			cost = cost + 1;
	}
	
	for (unsigned int i = param; i < this->line.size()-param; i++)
	{
		for (int j = -param; j < param; j++)
			cost = cost + 0.5*abs(this->line.at(i-j-1)-this->line.at(i));
	}
	cost = cost + 0.3*abs(this->pt.x - this->line.at(1));
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
    std::uniform_int_distribution<int> dist(-2,2);
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 100; i++)
    {
        double value = dist(generator);
        double var = 0;
        do
        {
        		var = distrib(generator);
        }
		  while (pt.y == var) ;
        double firstcost = this->cost();
        this->swp(var, value);
        double secondcost = this->cost();
        res = res + abs(firstcost-secondcost);
    }
    res = res / 100;
    double T0 = - res / log(tau0);
    std::cout << "res = " << res << " , log = " << 100 * log(tau0) << std::endl;
    return T0;
}

void Recuit::recuit(double tau0)
{
    double best_cost = 0;
    best_cost = this->cost();
    this->sol = this->line;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    std::uniform_int_distribution<int> distrib(0,this->line.size()-1);
    std::uniform_int_distribution<int> dist(-2,2);
    bool changeInBest = false;
    /*for (int h = 0; h < 10; h++)
    {*/
        double T0 = this->getInitialTemp(tau0);
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
            while (t != 100*this->line.size() && (accept+acceptdelta) != 12*this->line.size() && cont)
            {
                t++ ;
                nbiter++ ;
                //Compute inititial cost
                cost_i = this->cost();
                //Pick up two aleatory pieces
                value = dist(generator);
                do
                {
                    var = distrib(generator);
                }
                while (pt.x == var) ;
                this->swp(var, value);
                //Compute new cost
                cost_j = this->cost();
                if (cost_j < best_cost)
                {
                    best_cost = cost_j;
                    this->sol = this->line;
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
                        this->swp(var, -value);; //Refuse the swap, so we reput the last conﬁguration
                }
            }
            if (changeInBest)
            	draw();
            std::cout << "T : " << T << " , t : " << t << " , nbiter : " << nbiter << " , accept : " << accept << " , acceptdelta : " << acceptdelta << " , moyrnd : " << rnd/(t-accept) << " , best_cost : " << best_cost << std::endl;
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
    //}
    this->line = this->sol;
    std::cout << "end recuit" << std::endl;
    this->draw();
    for(unsigned int i = 0; i < this->line.size(); i++)
    	std::cout << "(" << this->line.at(i) << "," << i << ") ";
    std::cout << std::endl;
}