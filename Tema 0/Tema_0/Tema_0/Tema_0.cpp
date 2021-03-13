#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <chrono>
#include <typeinfo>

using namespace std;

int nrParam = 4;
long long nrIncercari = 1e5;
double epsilon = 0.01;
double left_ = -10, right_ = 10;

#define min(x, y) (x < y ? x : y)

double randomFloat()
{
    double val = abs(left_) + abs(right_);
    return ((float)rand() / (float)(RAND_MAX)) * val - abs(left_);
}

// Sum Squares Function : [-10, 10] -> R
// f(X) = S_i=1 ^ n (i * x_i ^ 2)
double f(vector <double> vec) 
{
    double rez = 0;
    for (int i = 0; i < vec.size(); i++)
        rez += i * vec[i] * vec[i];
    return rez;
}

void deterministSolve(int i, vector <double>& vec, double& minim)
{
    if (i == vec.size())
    {
        double value = f(vec);
        minim = min(minim, value);
    }
    else
    {
        for (double x = left_; x <= right_; x += epsilon) 
        {
            vec[i] = x;
            deterministSolve(i + 1, vec, minim);
        }
    }
}

double euristicSearch(int nrParam)
{
    vector <double> vec(nrParam);
    double minim = DBL_MAX;
    for (int ind = 0; ind < nrIncercari; ++ind)
    {
        for (int i = 0; i < vec.size(); i++)
            vec[i] = randomFloat();
        double value = f(vec);
        minim = min(minim, value);
    }
    vec.clear();
    return minim;
}

int main()
{
    srand(time(NULL));
    vector <double> vec(nrParam);

    double minim = DBL_MAX;
    auto t1 = chrono::high_resolution_clock::now();
    cout <<t1;
    return 0;
    deterministSolve(0, vec, minim);
    auto t2 = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    cout << "Determinist: \nMinimul: " << minim << "\n" << duration / (1e6) << " s"; // 1.000.000
    
    double minim_ = DBL_MAX;
    auto t1_ = chrono::high_resolution_clock::now();
    for (int ind = 0; ind < 30; ind++) 
    {
        double value = euristicSearch(nrParam);
        minim_ = min(minim_, value);
    }
    auto t2_ = chrono::high_resolution_clock::now();
    auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(t2_ - t1_).count();
    cout << "\n\nEuristic: \nMinimul: " << minim_ << "\n" << duration_ / (1e6) << " s"; // 1.000.000

    return 0;
}