#include "Cut.h"
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

Pic* LoadPic(string file)
{
	Pic* p = new Pic();
	p->Input(file);
	return p;
}

void SeamCarving(Pic* p, double prec, string name)
{
	int CutX = p->GetN() * prec;
	int CutY = p->GetM() * prec;
	cout << "Begin X Cutting..." << endl;
	p->SetDir(X);
	for(int i = 0; i < CutX; i++)
	{
		p->GetSeam();
		p->Cutting();
		p->Shrink();
	}
	cout << "Begin Y Cutting..." << endl;
	p->SetDir(Y);
	for(int i = 0; i < CutY; i++)
	{
		p->GetSeam();
		p->Cutting();
		p->Shrink();
	}
	cout << "Cutting over, begin output..." << endl;
	p->Output(name + "Cut.png");
	cout << "Output over, begin recover..." << endl;
	p->Recover(Color(255, 0, 0), Color(0, 255, 0));
	p->Output(name + "Recover.png");
	cout << "SeamCarving All Complete..." << endl;
}	 
