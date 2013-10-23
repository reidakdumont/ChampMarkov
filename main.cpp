#include "Recuit.h"

std::vector<cv::Point2d> readfile(char* name)
{
	std::vector<cv::Point2d> vect;
	fstream fichier(name);
	if ( !fichier ) {               // si le fichier n'est pas trouvé
		cout << "fichier inexistant";
	} else {                        // sinon
		bool continuer = true;      // indicateur de fin de fichier
		while( continuer ) {
			std::string ch;              // chaine contenant une ligne du fichier
			fichier >> ch;	    // ranger une ligne dans ch
			if ( ch != "" )         // si la ligne n'est pas vide faire
			{
				ch = ch.substr(1, ch.size()-1);
				ch = ch.substr(0, ch.size()-1);
				int pos = ch.find(",");
				int line = atoi(ch.substr(0, pos).c_str());
				int col = atoi(ch.substr(pos+1, ch.size()-pos).c_str());
				//std::cout << line << " " << col << std::endl;
				cv::Point2d pt(line,col);
				vect.push_back(pt);
			}
			else                   // sinon
				continuer = false;  //   sortir de la boucle de lecture
        }
	}
	return vect;
}

int rand_a_b(int a, int b){
	srand(time(NULL));
	return rand()%(b-a) +a;
}

int main(int argc, char** argv)
{
	std::vector<cv::Point2d> tab = readfile(argv[2]);
	std::uniform_int_distribution<int> dist(0,tab.size());
	std::default_random_engine generator;
	int nb = rand_a_b(0,tab.size());
	std::cout << tab.size() << " " << nb << std::endl;
	Recuit r(argv[1], tab.at(nb));
	r.draw();
	r.recuit(0.5);
}