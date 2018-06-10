#ifndef PIC_H
#define PIC_H

#include <string>
#include <vector>
using std::string;
extern const double INF;
typedef unsigned char byte;

enum Dir
{
	X = 0,
	Y = 1,
};

struct Color
{
	byte r,g,b;
	Color(byte _r = 0, byte _g = 0, byte _b = 0) : r(_r), g(_g), b(_b)	{}
};

struct pix
{
	pix(Color _c = Color(), double _e = 0.0, int _p = -1) : c(_c), e(_e), pre(_p)	{}
	Color c;
	double e;
	int pre;
};

class Pic
{
	public:
		Pic(int _m = 0, int _n = 0, Dir _d = X);
		void Input(string file);
		void Output(string file);
		void Remove(int i, int j);
		void Copy(int i, int j);
		void Shrink();
		void Expand();
		void SetColor(int i, int j, Color c)	{data[i][j].c = c;}
		Color GetColor(int i, int j)			{return data[i][j].c;}
		void SetDir(Dir d)						{dir = d;}
		Dir GetDir()							{return dir;}
		int GetM()								{return M;}
		int GetN()								{return N;}
		double GetEnergy();
		void Initrem();
		void Recover(Color cx, Color cy);
		void GetSeam();
		void Cutting();
		void Booming();
	private:
		int m, n, M, N;
		Dir dir;
		void UpdateEnergy();
		std::vector<std::vector<pix>> data;
		std::vecter<std::vector<int>> Rempos;
		std::vector<std::vector<Dir>> Remdir;
		int seamhead;
};


#endif
