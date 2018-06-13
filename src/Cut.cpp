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

void Cutting(Pic* p, int CutX, int CutY, string name)
{
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
}

void Booming(Pic* p, Pic* org, int CutX, int CutY, string name)
{
	cout << "Begin X Booming.." << endl;
	p->SetDir(X);
	org->SetDir(X);
	for(int i = 0; i < CutX; i++)
	{
		p->GetSeam();
		p->Expand();
		org->Expand();
		p->Booming(Color(255, 0, 0), Color(0, 255, 0), org);
	}

	cout << "Begin Y Booming.." << endl;
	p->SetDir(Y);
	org->SetDir(Y);
	for(int i = 0; i < CutY; i++)
	{
		p->GetSeam();
		p->Expand();
		org->Expand();
		p->Booming(Color(255, 0, 0), Color(0, 255, 0), org);
	}

	cout << "Booming over, begin output..." << endl;
	org->Output(name + "Boom.png");
	
	cout << "Output over, begin recover..." << endl;
	p->Output(name + "BoomRecover.png");
}

void SeamCarving(string file, double xcoef, double ycoef, string name)
{
	Pic* p = LoadPic(file);
	p->SetDel(name);
	p->SetProt(name);
	Pic* tmp = p->clone();
	Pic* org = p->clone();
	int CutX = p->GetN() * xcoef;
	int CutY = p->GetM() * ycoef;
	Cutting(p, CutX, CutY, name);
	delete p;

	Booming(tmp, org, CutX, CutY, name);
	delete tmp, org;


	cout << "SeamCarving All Complete..." << endl;
}
