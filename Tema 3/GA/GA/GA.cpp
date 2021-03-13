#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

ifstream fin("ftv70.atsp");
ofstream fout("ftv70.txt");
#define lung 71
#define pop_size 100 ///
float cp; // 0.5 pt 1     0.3 pt 2    0.25 pt 5
float mp; // 0.1 pt 1;   0.01 pt 2     0.005 pt 5
int pas; // 250 pt 5, 750 pt 2
bool ok = true;

//! 5: 
// 2k -> 4.2k rez
// 4l -> 4.1k

// 1 17 39
// 2 53 6.9k
// 3 34 1.3k
// 5 323 1.2k
// 323 -> 3k it 4.7k rez de 2 ori cp = 0.25
// 3k it 4.6k rez
// 4k it 4.4k rez -> cp = 0.2
// 4k,5k it 4.5k rez -> cp = 0.25
//5k it 4.5k rez, cp = 0.2


// cu oprirea: 3k it 4.6k rez     
// 5k it 4.4k rez
// 7k it 4.3k rez
// 10k it 4.3k rez

int population[pop_size][lung];
int newPopulation[pop_size][lung];
int map[lung][lung];

float populationFitness[pop_size];
float selectionProbability[1 + pop_size];

void init()
{
    if (lung < 35){
        cp = 0.5;
        mp = 0.1;
        pas = 500; // 
    }
    else if(lung < 100){
        cp = 0.3;
        mp = 0.015;
        pas = 750; // 
    }
    else { // f mare
        cp = 0.25;
        mp = 0.005;
        pas = 250;
    }
}
long long int evaluate(int* vec);
void decode(int* vec, int* rez);
long long int evaluatePopulation();
void evaluateFitness();
void mutate();
void crossOver();
void crossOverSwitch(int i1, int i2);


int GA()
{
    init();

    for (int i = 0; i < pop_size; ++i)
        for (int j = 0; j < lung; ++j)
            population[i][j] = rand() % (lung - j) + 1;

    int best = INT_MAX;
    int k = evaluatePopulation();
    if (k < best)
        best = k;

    int t = 1;
    int val = 0;
    while (++t <= 10000) 
    {
        if (ok == true)
            if (t % pas == 0)
            {
                if (cp > 0.01)
                     cp -= 0.01;
                if (mp > 0.001)
                     mp -= 0.001; // 0.001   0.0005
            }
            float totalFitness = 0;
            evaluateFitness(); 
            selectionProbability[0] = 0;

            for (int i = 0; i < pop_size; ++i) 
                totalFitness = totalFitness + populationFitness[i];
            for (int i = 0; i < pop_size; ++i) 
                populationFitness[i] = populationFitness[i] / totalFitness;
            for (int i = 0; i < pop_size; ++i) 
                selectionProbability[i + 1] = selectionProbability[i] + populationFitness[i];

            // selectie
            for (int i = 0; i < pop_size; ++i)
            {
                float r = (double)rand() / (RAND_MAX); // (0, 1]
                for (int j = 0; j < pop_size; j++)
                    if (selectionProbability[j] < r && r <= selectionProbability[j + 1]) 
                        for (int k = 0; k < lung; k++)
                            newPopulation[i][k] = population[j][k];
            }
            
            for (int i = 0; i < pop_size; i++)
                for (int j = 0; j < lung; j++)
                    population[i][j] = newPopulation[i][j];
            crossOver();
            mutate();

            int k = evaluatePopulation();
            if (k < best)
                best = k;
            if(t % 2500 == 0)
                cout << "Iteratie: " << t << "   Valoare: " << best << " \n";
        }
    cout << "\n";
    return best;
}

int main()
{
    srand(time(0));
    string s;
    for (int i = 0; i < 7; ++i)
        getline(fin, s);

    for (int i = 0; i < lung; ++i)
        for (int j = 0; j < lung; ++j)
            fin >> map[i][j];

    vector <double> timp_;
    for (int i = 0; i < 30; ++i)
    {
        time_t t1 = time(NULL);
        fout << GA() << " ";
        time_t t2 = time(NULL);
        timp_.push_back(t2 - t1);
    }
    fout << "\n\n";
    for (int i = 0; i < 30; ++i)
        fout << timp_[i] << " ";
    return 0;
}

long long int evaluate(int* vec)
{
    long long int cost = 0;
    for (int i = 0; i < lung - 1; i++)
        cost += map[vec[i]][vec[i + 1]];
    return cost + map[vec[lung - 1]][vec[0]];
}

void decode(int* vec, int* rez)
{
    int v[lung];
    for (int i = 0; i < lung; ++i)
        v[i] = 0;
    for (int i = 0; i < lung; ++i)
    {
        int pos = 0;
        for (int j = 0; j < lung; ++j)
            if (v[j] == 0)
                if (++pos == vec[i])
                {
                    rez[i] = j;
                    v[j] = 1;
                    break;
                }
    }
}

long long int evaluatePopulation()
{
    int res_eval[lung];
    long long int minim = INT_MAX;
    for (int i = 0; i < pop_size; ++i)
    {
        decode(population[i], res_eval);
        long long int k = evaluate(res_eval);
        if (k < minim)
            minim = k;
    }
    return minim;
}

void evaluateFitness()
{
    int res[lung];
    for (int i = 0; i < pop_size; ++i) {
        decode(population[i], res);
        long long int k = evaluate(res);
        populationFitness[i] = pow(1.0 / (float)k, 9);
    }
}

void crossOverSwitch(int i1, int i2)
{
    int ind = rand() % lung;
    for (int i = ind; i < lung; ++i)
        swap(population[i1][i], population[i2][i]);
}

void crossOver()
{
    for (int i = 0; i < pop_size; i = i + 2)
        if ((double)rand() / (RAND_MAX) < cp)
            crossOverSwitch(i, i + 1);
    /*
    int x = 0;
    int i1, i2;
    for (int i = 0; i < pop_size; ++i)
        if ((double)rand() / (RAND_MAX) < cp){
            if (x == 0){
                x = !x;
                i1 = i;
            }
            else
            if (x == 1){
                x = !x;
                i2 = i;
                crossOverSwitch(i1, i2);
            }
        }
    */
}

void mutate()
{
    for (int i = 0; i < pop_size; i++)
        for (int j = 0; j < lung; j++)
            if ((double)rand() / (RAND_MAX) < mp)
                population[i][j] = rand() % (lung - j) + 1;
}
