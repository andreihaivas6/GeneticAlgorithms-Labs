#include <iostream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <random>
#include <fstream>

using namespace std;
int stop;

#define lung 17

ifstream fin("br17.atsp");
ofstream fout("bt17.txt");
/*
	1 17 39
	2 53 6.9k
	5 323 1.2k 
	6 403 2465
	5:
	0.999
		1000it -> 1430
		500 it -> 1455 1478 3min
	0.9995
		500 it -> - || -
		1000it ->
*/

int vc[500];
int vn[500];
int graf[lung][lung];

std::mt19937 generator(time(NULL));
std::uniform_real_distribution<float> dis(0, 0.999);
std::mt19937 generator1(generator);

int randomInt(int x, int y);
double randomFloat(double x, double y);
void f(int vn[], int vc[]);
float evaluate(int vec[]);

int main() 
{
	srand(time(0));
	string s;
	for (int i = 0; i < 7; ++i)
		getline(fin, s);
	for (int i = 0; i < lung; i++) 
		for (int j = 0; j < lung; j++) 
			fin >> graf[i][j];

	if (lung < 35) stop = 300;
	else if (lung < 100) stop = 500;
	else stop = 1000;


	vector <int> timp_;
	for (int i = 0; i < 5; ++i)
	{

		time_t t1 = time(NULL);
		int v1[lung + 1], v2[lung + 1];

		int it;
		double T = 1000;
		float vc_, vn_;

		for (int i = 0; i < lung; ++i)
		{
			vc[i] = randomInt(0, lung - 1);
			vn[i] = randomInt(0, lung - 1);
		}

		do {
			it = 0;
			do {
				f(vn, vc);
				vn_ = evaluate(vn);
				vc_ = evaluate(vc);

				if (vn_ < vc_)
					for (int i = 0; i < lung; ++i)
						vc[i] = vn[i];
				else
				{
					float randF = randomFloat(0, 1);
					float val = exp(-abs(vn_ - vc_) / T);
					if (randF < val)
						for (int i = 0; i < lung; i++)
							vc[i] = vn[i];
				}
			} while (++it < stop); // 1k

			if (lung < 30) T = T * 0.99;
			else if (lung < 120) T = T * 0.999;
			else T = T * 0.9995;
			//cout << T << "\n";
		} while (T > 10e-8);
		int k = evaluate(vc);

		//
		fout <<k << " ";
		time_t t2 = time(NULL);
		cout << k << " " << (t2 - t1) << "\n";
		timp_.push_back(t2 - t1);
	}
	fout << "\n\n";
	for (int i = 0; i < 5; ++i)
		fout << timp_[i] << " ";
	return 0;
}

int randomInt(int x, int y)
{
	std::random_device generator;
	std::uniform_int_distribution<int> distribution(x, y);
	return distribution(generator);
}

double randomFloat(double x, double y)
{
	std::random_device generator;
std:uniform_real_distribution<double> distribution(x, y);
	return distribution(generator);
}

void f(int vn[], int vc[])
{
	int r1 = randomInt(0, lung - 1);
	int r2 = randomInt(0, lung - 1);
	for (int i = 0; i < lung; i++)
		vn[i] = vc[i];

	int aux = vn[r1];
	vn[r1] = r2;
	for (int i = 0; i < lung; i++)
		if (i != r1 && vn[i] == r2)
			vn[i] = aux;
}

float evaluate(int vec[]) {
	float rez = 0;
	for (int i = 0; i < lung - 1; i++)
		rez += graf[vec[i]][vec[i + 1]];
	return rez + graf[vec[lung - 1]][vec[0]];
}
