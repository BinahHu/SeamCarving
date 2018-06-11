#include "pic.h"
#include "lodepng.h"
#include <vector>
#include <cmath>
#include <omp.h>
#include <iostream>
using std::cout;
using std::endl;
using std::cin;
using std::fabs;
using std::abs;

const double INF = 1e9;

Pic::Pic(int _m, int _n, Dir _dir)
{
    m = _m;
    n = _n;
	dir = _dir;
    if(m!= 0 && n!= 0)
    {
		data.resize(m);
        for(int i = 0; i < m; i++)
			data[i].resize(n);
    }
}

Color Pic::GetColor(int i, int j) const
{
	if(i < 0 || i >= m || j < 0 || j >= n)
		return Color();
	return data[i][j].c;
}

void Pic::Remove(int i, int j)
{
	if(dir == X)
		for(int k = j; k < n - 1; k++)
			data[i][k]  = data[i][k + 1];
	else
		for(int k = i; k < m - 1; k++)
			data[k][j] = data[k + 1][j];
}

void Pic::Copy(int i, int j)
{
	if(dir == X)
		for(int k = n - 1; k > j; k--)
			data[i][k] = data[i][k-1];
	else
		for(int k = m - 1; k > i; k--)
			data[k][j] = data[k-1][j];
}

void Pic::Shrink()
{
	if(dir == X)n--;
	else m--;
}

void Pic::Expand()
{
	if(dir == X)
	{
		n++;
		for(int i = 0; i < m; i++)
			data[i].push_back(pix());
	}
	else
	{
		m++;
		data.push_back(std::vector<pix>());
	}
}

void Pic::Recover(Color cx, Color cy)
{
	int S = Rempos.size();
	for(int k = S - 1; k >= 0; k--)
	{
		if(Remdir[k] == X)
		{
			n++;
#pragma omp parallel for
			for(int i = 0; i < m; i++)
			{
				for(int j = n - 1; j > Rempos[k][i]; j--)
					data[i][j] = data[i][j-1];
				data[i][Rempos[k][i]].c = cx;
			}
		}
		else
		{
			m++;
#pragma omp parallel for
			for(int j = 0; j < n; j++)
			{
				for(int i = m - 1; i > Rempos[k][j]; i--)
					data[i][j] = data[i-1][j];
				data[Rempos[k][j]][j].c = cy;
			}	
		}
	}
}

void Pic::Addrem()
{
	Rempos.push_back(std::vector<int>(std::max(m, n)));
	Remdir.push_back(dir);
}

void Pic::GetSeam()
{
	UpdateEnergy();
	if(dir == X)
	{
		for(int i = 1; i < m; i++)
#pragma omp parallel for
			for(int j = 0; j < n; j++)
			{
				double e1,e2,e3;
				if(j == 0)		e1 = INF;
				else			e1 = data[i-1][j-1].e;
				if(j == n-1)	e3 = INF;
				else			e3 = data[i-1][j+1].e;
				e2 = data[i-1][j].e;
				if(e1 < e2)
				{
					if(e1 < e3)	{data[i][j].e += e1; data[i][j].pre = j-1;}
					else		{data[i][j].e += e3; data[i][j].pre = j+1;}
				}
				else
				{
					if(e2 < e3)	{data[i][j].e += e2; data[i][j].pre = j;}
					else		{data[i][j].e += e3; data[i][j].pre = j+1;}
				}
			}
		double mine = INF;
		for(int j = 0; j < n; j++)
			if(mine > data[m-1][j].e)
			{
				mine = data[m-1][j].e;
				seamhead = j;
			}

	}
	else
	{
		for(int j = 1; j < n; j++)
#pragma omp parallel for
			for(int i = 0; i < m; i++)
			{
				double e1,e2,e3;
				if(i == 0)		e1 = INF;
				else			e1 = data[i-1][j-1].e;
				if(i == m-1)	e3 = INF;
				else			e3 = data[i+1][j-1].e;
				e2 = data[i][j-1].e;
				if(e1 < e2)
				{
					if(e1 < e3)	{data[i][j].e += e1; data[i][j].pre = i-1;}
					else		{data[i][j].e += e3; data[i][j].pre = i+1;}
				}
				else
				{
					if(e2 < e3)	{data[i][j].e += e2; data[i][j].pre = i;}
					else		{data[i][j].e += e3; data[i][j].pre = i+1;}
				}
			}
		double mine = INF;
		for(int i = 0; i < m; i++)
			if(mine > data[i][n-1].e)
			{
				mine = data[i][n-1].e;
				seamhead = i;
			}			
	}
}

void Pic::Cutting()
{
	Addrem();
	int num = Rempos.size() - 1;
	int ind = -1, next = seamhead;
	if(dir == X)
	{
		for(int i = m-1; i >= 0; i--)
		{
			ind = next;
			next = data[i][ind].pre;
			Remove(i, ind);
			Rempos[num][i] = ind;
		}
	}
	else
	{	
		for(int j = n-1; j >= 0; j--)
		{
			ind = next;
			next = data[ind][j].pre;
			Remove(ind, j);
			Rempos[num][j] = ind;
		}	
	}
}

void Pic::Booming()
{}

double Pic::Sobel(int i0, int j0) const
{
	double ex[3] = {0.0}, ey[3] = {0.0};
	for(int i = -1; i <= 1; i++)
		for(int j = -1; j <= 1; j++)
		{
			Color c =  GetColor(i+i0, j+j0);
			ex[0] += c.r * i * (2 - abs(j));
			ex[1] += c.g * i * (2 - abs(j));
			ex[2] += c.b * i * (2 - abs(j));
			ey[0] += c.r * j * (2 - abs(i));
			ey[1] += c.g * j * (2 - abs(i));
			ey[2] += c.b * j * (2 - abs(i));
		}
	return fabs(ex[0]) + fabs(ex[1]) + fabs(ex[2]) + fabs(ey[0]) + fabs(ey[1]) + fabs(ey[2]);
}

void Pic::UpdateEnergy()
{
#pragma omp parallel for
	for(int i = 0; i < m; i++)
		for(int j = 0; j < n; j++)
		{
			data[i][j].e = Sobel(i,j);	
		}
}

void Pic::Input(string file)
{
    file = "../input/" + file;
    std::vector<unsigned char> image;
    unsigned w, h;
    unsigned error = lodepng::decode(image, w, h, file, LCT_RGB);
    M = m = h;N = n = w;
	data.clear();
    data.resize(m);
    for(int i = 0; i < m; i++)
    {
		data[i].resize(n);
        for(int j = 0; j < n; j++)
        {
            data[i][j].c.r = image[(i * n + j) * 3 + 0];
            data[i][j].c.g = image[(i * n + j) * 3 + 1];
            data[i][j].c.b = image[(i * n + j) * 3 + 2];
        }
    }
}

void Pic::Output(string file)
{
	file = "../output/"  + file;
    std::vector<unsigned char> image(m * n * 3);
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
        {
            image[(i * n + j) * 3 + 0] = data[i][j].c.r;
            image[(i * n + j) * 3 + 1] = data[i][j].c.g;
            image[(i * n + j) * 3 + 2] = data[i][j].c.b;
        }
    unsigned error = lodepng::encode(file, image, n, m, LCT_RGB);
}
