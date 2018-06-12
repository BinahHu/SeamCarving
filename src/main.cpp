#include "Cut.h"

int main(int argc, char** argv)
{
	string name = "p3";
	if(argc > 1)
		name = argv[1];
	double xcoef = 0.2, ycoef = 0.2;
	if(argc > 2)
		xcoef = atof(argv[2]);
	if(argc > 3)
		ycoef = atof(argv[3]);
	SeamCarving(name + ".png", xcoef, ycoef, name);
	return 0;
}
