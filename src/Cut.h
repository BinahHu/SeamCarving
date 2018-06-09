#ifndef CUT_H
#define CUT_H
#include "pic.h"
Pic* LoadPic(string file);
void SeamCarving(Pic* p, double prec, string name);
#endif
