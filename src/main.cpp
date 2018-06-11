#include "Cut.h"

int main(int argc, char** argv)
{
	string name = argv[1];
	Pic* p = LoadPic(name + ".png");
	SeamCarving(p, 0.2, name);
	return 0;
}
