#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctime>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>

using namespace std;


#define PI 3.14//159265359

#define pop_size 100
#define N 30
#define prec 4
float PC = 0.1;      // probabilitate cross_over // 0.1 0.2 
float PM = 0.001;  // probabilitate mutatie    // 0.001 0.01/0.005
// pt dimensiuni mici: cromozomul e scurt si necesita o prob_m mai mare
int size_, length;
float A, B;
bool pop[pop_size][1000];
bool newPop[pop_size][1000];
float popFit[pop_size + 1]; // population fitness
float probS[pop_size + 1]; // selection_probability


void initialisation(int func)
{
    if (func == 1 || func == 3)
    {
        A = -5.12;
        B = 5.12;
    }
    else if (func == 2)
    {
        A = -500;
        B = 500;
    }
    else
    {
        A = 0;
        B = PI;
    }
}

float dejong(float* res)
{
    float result = 0;
    for (int i = 0; i < N; i++)
        result += res[i] * res[i];
    return result;
}

float schwefel(float* res)
{
    float result = 0;
    for (int i = 0; i < N; i++)
        result -= res[i] * sin(sqrt(abs(res[i])));
    return result;
}

float rastrigin(float* res)
{
    float result = 10 * N;
    for (int i = 0; i < N; i++)
        result += res[i] * res[i] - 10 * (float)cos(2 * PI * res[i]);
    return result;
}
float michalewics(float* res)
{
    float result = 0;
    for (int i = 0; i < N; i++)
        result += sin(res[i]) * pow(sin(((double)i * res[i] * res[i]) / PI), 20); // 10 20
    return -result;
}



void f(bool* a, float* res)
{
    // gray -> binar
    bool* res2 = new bool[size_];
    res2[0] = a[0];
    for (int i = 1; i < size_; i++) // 100
        if (a[i] == 0)
            res2[i] = a[i - 1];
        else
            res2[i] = !a[i - 1];
    //a = res2; // !/

    for (int i = 0; i < N; i++)
    {
        int p = 1;
        res[i] = 0;
        for (int j = 0; j < length; j++)
        {
            res[i] = res[i] + p * res2[i * length + j]; // res2 -> a
            p = p * 2;
        }
        res[i] = res[i] * (B - A) / p + A;
    }
}

void evaluationFitness(int func)
{
    float resFit[1000];
    if (func == 1)
        for (int i = 0; i < pop_size; i++)
        {
            f(pop[i], resFit);
            popFit[i] = 1 / dejong(resFit);
        }
    else if (func == 2)
        for (int i = 0; i < pop_size; i++)
        {
            f(pop[i], resFit);
            //popFit[i] = 1 / (schwefel(resFit) + 418.9829 * N);
            auto aux = schwefel(resFit) + 418.9829 * N;
            popFit[i] = pow(aux, -2); //schwefel(resFit) - C;
        }
    else if (func == 3)
        for (int i = 0; i < pop_size; i++)
        {
            f(pop[i], resFit);
            auto aux = 1 / rastrigin(resFit);
            popFit[i] = pow(aux, 2);
            //popFit[i] = aux;
        }
    else // func = 4
        for (int i = 0; i < pop_size; i++)
        {
            f(pop[i], resFit);
            //auto aux = -michalewics(resFit);
                auto aux = 1 / (michalewics(resFit) + N + 3);
            popFit[i] = pow(aux, 32);// 
        }
}


float evaluation(int func)
{
    float rez[1000];
    float minim = FLT_MAX;
    for (int i = 0; i < pop_size; i++)
    {
        f(pop[i], rez);
        if (func == 1)
            if (dejong(rez) < minim)
                minim = dejong(rez);
        if (func == 2)
            if (schwefel(rez) < minim)
            {
                //cout << "___" << schwefel(rez) << " " << minim << "\n";
                minim = schwefel(rez);
            }
        if (func == 3)
            if (rastrigin(rez) < minim)
                minim = rastrigin(rez);
        if (func == 4)
            if (michalewics(rez) < minim)
            {
                //cout << "_";
                //cout << "___" << michalewics(rez) << " " << minim << "\n";
                minim = michalewics(rez);
            }
    }
    return minim;
}

void switch_cross_over(int i1, int i2)
{
    int x = rand() % size_;
    for (int i = x; i < size_; i++)
        swap(pop[i1][i], pop[i2][i]);
}

void cross_over()
{
    bool x = 0;
    int ind1;
    for (int i = 0; i < pop_size; i++)
        if (((float)rand() / RAND_MAX) < PC)
        {
            if (x == 0) {
                x = !x;
                ind1 = i;
            }
            else if (x == 1) { // else
                x = !x;
                switch_cross_over(ind1, i);
            }
        }
}

void mutation() // //mutation and/or cross_over
{
    for (int i = 0; i < pop_size; i++)
        for (int j = 0; j < size_; j++)
            if (((float)rand() / RAND_MAX) < PM)
                pop[i][j] = !pop[i][j];
}

float GA(int func)
{
    float Vector[100];
    float best = FLT_MAX;
    int nr = 0;
    //generate()
    for (int i = 0; i < pop_size; i++)
    {
        for (int j = 0; j < size_; ++j)
            pop[i][j] = rand() % 2;

        // binar -> gray
        bool* res2 = new bool[size_];
        res2[0] = pop[i][0];
        for (int j = 1; j < size_; ++j)
            res2[j] = pop[i][j - 1] ^ pop[i][j];
        for (int j = 0; j < size_; ++j)
            pop[i][j] = res2[j];
    }

    if (evaluation(func) < best)
        best = evaluation(func);
    for (int t = 0; t < 10000; ++t)
    {
        //selection
        evaluationFitness(func); //evaluating pop
        probS[0] = 0; //initializing selection probability

        float total = 0;
        for (int i = 0; i < pop_size; i++)
            total = total + popFit[i];
        for (int i = 0; i < pop_size; i++)
            popFit[i] = popFit[i] / total;
        for (int i = 0; i < pop_size; i++)
            probS[i + 1] = probS[i] + popFit[i];


        for (int i = 0; i < pop_size; i++)
        {
            float r = (float)rand() / RAND_MAX; //r \in (0, 1)
            //ce supravietuieste
            for (int j = 0; j < pop_size; j++)
                if (probS[j] < r && r <= probS[j + 1]) //
                    for (int k = 0; k < size_; k++)
                        newPop[i][k] = pop[j][k];
        }
        for (int i = 0; i < pop_size; i++) // populatie noua
            for (int j = 0; j < size_; j++)
                pop[i][j] = newPop[i][j];
        cross_over();
        mutation();
        if (evaluation(func) < best)
            best = evaluation(func);
        if (t % 1000 == 0)
        {
            /*if (t == 5000)
            {
                PC = 0.005;
                PM = 0.001;
            }*/
            PM += 0.001;
            PC += 0.01;
            cout << t << " " << best << endl;
        }
        //cout << t << " " << best << "\n";
    }
    cout << "\n";
    //cout << "\nRezultat final: " << best;
    return best;
}

int main()
{
    srand(time(0));
    int func;
    cout << N << " " << PM << "\n";
    cout << "1. dejong\n2. schwefel\n3. rastrigin\n4. michalewicz\n";
    cin >> func;
    if (!(func >= 1 && func <= 4))
        cout << "Esti nebun\n";
    else
        initialisation(func);

    vector <float> rez(30);
    vector <float> timp(30);

    length = (int)ceil(log2((pow(10, prec)) * (B - A)));
    size_ = (N * length);

    for (int i = 0; i < 1; ++i)
    {
        auto t1 = chrono::high_resolution_clock::now();
        float val = GA(func);
        auto t2 = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
        rez[i] = val;
        timp[i] = duration / 1e6;
        cout << "Iteratia: " << i + 1 << " Rezultatul: " << val << "\n";
    }
    /*
    string filename = "2";
    switch (func) {
    case 1: filename += "D_"; break;
    case 2: filename += "S_"; break;
    case 3: filename += "R_"; break;
    case 4: filename += "M_"; break;
    }
    int x = N;
    filename += to_string(x) + "_";
    float x1 = PM;
    filename += to_string(x1) + ".txt";

    ofstream fout(filename);

    for (float a : rez)
        fout << a << " ";
    fout << "\n\n";
    for (float a : timp)
        fout << a << " ";
        */
    return 0;
}