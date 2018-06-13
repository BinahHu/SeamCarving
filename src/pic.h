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

enum Mode
{
	normal = 0,
	del = 1,
	prot = 2,
};

struct Color
{
	byte r,g,b;
	Color(byte _r = 0, byte _g = 0, byte _b = 0) : r(_r), g(_g), b(_b)	{}
};

struct pix
{
	pix(Color _c = Color(), double _e = 0.0, int _p = -1, Mode _m = normal) : c(_c), e(_e), pre(_p), mode(_m)	{}
	Color c;
	double e;
	int pre;
	Mode mode;
};

class Pic
{
	public:
		Pic(int _m = 0, int _n = 0, Dir _d = X);
		void Input(string file);
		void Output(string file);
		void Remove(int i, int j);
		void Copy(int i, int j, bool set = false, Color c = Color());
		void Shrink();
		void Expand();
		Pic* clone();
		void SetColor(int i, int j, Color c)	{data[i][j].c = c;}
		Color GetColor(int i, int j) const;
		void SetDir(Dir d)						{dir = d;}
		Dir GetDir()	const					{return dir;}
		int GetM()	const						{return M;}
		int GetN()	const						{return N;}
		double GetEnergy() const;
		void Addrem();
		void Recover(Color cx, Color cy);
		void GetSeam();
		void Cutting();
		void Booming(Color cx, Color cy, Pic* org);
		void SetDel(string name);
		void SetProt(string name);
	private:
		int m, n, M, N;
		Dir dir;
		void UpdateEnergy();
		double Sobel(int i, int j) const;
		std::vector<std::vector<pix>> data;
		std::vector<std::vector<int>> Rempos;
		std::vector<Dir> Remdir;
		int seamhead;
};


#endif
