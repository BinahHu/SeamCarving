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
    M = _m;
    N = _n;
	dir = _dir;
    if(M!= 0 && N!= 0)
    {
		data.resize(M);
        for(int i = 0; i < M; i++)
			data[i].resize(N);
    }
}

Pic* Pic::clone()
{
	Pic* p = new Pic(m, n, dir);
	p->m = m;
	p->n = n;
	p->data = data;
	p->Rempos = Rempos;
	p->Remdir = Remdir;
	p->seamhead = seamhead;
	return p;
}

void Pic::SetDel(string name)
{
    string file = "../input/" + name + "del.png";
    std::vector<unsigned char> image;
    unsigned w, h;
    unsigned error = lodepng::decode(image, w, h, file, LCT_GREY);
    if(error != 0)return;
    cout << "Load delete file" << endl;
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            if(image[i * n + j] < 250)
            {
                data[i][j].mode = del;
                data[i][j].e = -INF;
            }
}

void Pic::SetProt(string name)
{
    string file = "../input/" + name + "prot.png";
    std::vector<unsigned char> image;
    unsigned w, h;
    unsigned error = lodepng::decode(image, w, h, file, LCT_GREY);
    if(error != 0)return;
    cout << "Load protect file" << endl;
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            if(image[i * n + j] < 250)
            {
                data[i][j].mode = prot;
                data[i][j].e = INF;
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

void Pic::Copy(int i, int j, bool set, Color c)
{
	if(dir == X)
	{
		for(int k = n - 1; k > j; k--)
			data[i][k] = data[i][k-1];
		if(set)
			data[i][j].c = c;
		else
		{
			Color c1 = GetColor(i, j-1);
			Color c2 = data[i][j].c;
			Color c3 = GetColor(i, j+1);
			int ar = c1.r + c2.r + c3.r;
			int ag = c1.g + c2.g + c3.g;
			int ab = c1.b + c2.b + c3.b;
			data[i][j].c = Color(ar / 3, ag / 3, ab / 3);
		}
	}
	else
	{
		for(int k = m - 1; k > i; k--)
			data[k][j] = data[k-1][j];
		if(set)
			data[i][j].c = c;
		else
		{
			Color c1 = GetColor(i-1, j);
			Color c2 = data[i][j].c;
			Color c3 = GetColor(i+1, j);
			int ar = c1.r + c2.r + c3.r;
			int ag = c1.g + c2.g + c3.g;
			int ab = c1.b + c2.b + c3.b;
			data[i][j].c = Color(ar / 3, ag / 3, ab / 3);
		}

	}
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
		data.push_back(std::vector<pix>(n));
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
/*
void Pic::ExpMark(Color cx, Color cy, Pic* p)
{
	int S = p->Rempos.size();
	for(int k = 0; k < S; k++)
	{
		dir = p->Remdir[k];
		Expand();
		if(dir == X)
#pragma omp parallel for
			for(int i = 0; i < m; i++)
				Copy(i, p->Rempos[k][i], true, cx);
		else
#pragma omp parallel for
			for(int j = 0; j < n; j++)
				Copy(p->Rempos[k][j], j, true, cy);
	}
}
*/
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

void Pic::Booming(Color cx, Color cy, Pic* org)
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
			Copy(i, ind, true, cx);
			org->Copy(i, ind);
			Rempos[num][i] = ind;
		}
	}
	else
	{
		for(int j = n-1; j >= 0; j--)
		{
			ind = next;
			next = data[ind][j].pre;
			Copy(ind, j, true, cy);
			org->Copy(ind, j);
			Rempos[num][j] = ind;
		}
	}
}
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
            if(data[i][j].mode == normal)
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
