#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <chrono>
#include <string>

using namespace std;

#define PI 3.14159265359

void initialisation(double& a, double& b, int func)
{
    if (func == 1 || func == 3)
    {
        a = -5.12;
        b = 5.12;
    }
    else if (func == 2)
    {
        a = -500;
        b = 500;
    }
    else
    {
        a = 0;
        b = PI;
    }
}

double evaluation(vector <double>& values, int func) // folosesc pt a decide ce functie pt HC si SA de terminat
{
    double rez = 0;
    switch (func) {
    case 1:
        for (double x : values)
            rez += x * x;
        return rez;
    case 2:
        for (double x : values)
            rez += ((-x) * sin(sqrt(abs(x))));
        return rez;
    case 3:
        rez = 10. * values.size(); // = 10*n
        for (double x : values)
            rez += (x * x - 10 * cos(2 * PI * x));
        return rez;
    case 4:
        for (double i = 0; i < values.size(); ++i)
            rez += sin(values[i]) * pow(sin((i * values[i] * values[i]) / PI), 20); // 20 = 2 * m, m = 10
        return -rez;
    }
}

inline int length(double a, double b, int precision)
{
    return ceil(log2((b - a) * pow(10, precision)));
}

inline void randVector(vector <char>& bits)
{
    for (char& x : bits)
        x = rand() % 2;
}

vector <double> decode(vector <char>& bits, double a, double b, int l)
{
    double p = pow(2, l);
    vector <double> rez(bits.size() / l);
    for (int i = 0; i < rez.size(); ++i)
    {
        int x = 0;
        for (int j = i * l; j < (i + 1) * l; ++j)
            x = x * 2 + bits[j];
        double interval = b - a;
        rez[i] = a + x * interval / (p - 1);
    }
    return rez;
}

vector <char> neighbours(vector <char>& bits, double a, double b, int l, int func, int improvement)
{
    vector <char> vn = bits;
    vector <double> vc = decode(vn, a, b, l);
    double vcRez = evaluation(vc, func);
    for (char& x : bits)
    {
        x = !x;
        vc = decode(bits, a, b, l);
        double vnRez = evaluation(vc, func);
        if (vnRez < vcRez)
        {
            vn = bits;
            vcRez = vnRez;
            if (improvement == 1) // first
            {
                x = !x;
                return vn;
            }
        }
        x = !x;
    }
    return vn;
}


double HC(double a, double b, int n, int prec, int func, int improvement)
{
    int l = length(a, b, prec);
    int L = l * n;
    vector <char> vc(L);

    int t = 0, MAX = 100;
    double best = DBL_MAX;
    do{
        bool local = false;
        randVector(vc);
        vector <double> vec = decode(vc, a, b, l);
        double vcRez = evaluation(vec, func);
        do{
            vector <char> vn = neighbours(vc, a, b, l, func, improvement);
            vec = decode(vn, a, b, l);
            double vnRez = evaluation(vec, func);
            if (vnRez < vcRez)
            {
                vc = vn;
                vcRez = vnRez;
            }
            else
                local = true;

        } while (local == false);
        ++t;
        if (vcRez < best)
            best = vcRez;
    } while (t < MAX);  // 100
    return best;
}

double SA(double a, double b, int n, int prec, int func)
{
    int l = length(a, b, prec);
    int L = l * n;
    vector <char> vc(L);

    double T = 1000;

    randVector(vc);
    vector <double> vec = decode(vc, a, b, l);
    double vcRez = evaluation(vec, func);
    do
    {
        int c = 0;
        do
        {
            vector <char> vn = vc;
            int ind = rand() % vn.size();
            vn[ind] = !vn[ind];

            vector <double> vn_ = decode(vn, a, b, l);
            double vnRez = evaluation(vn_, func);
            if (vnRez < vcRez)
            {
                vcRez = vnRez;
                vc = vn;
            }
            else    //   val in [0, 1)
                if ((((double)rand() / (double)(RAND_MAX))) < exp(-(abs(vnRez - vcRez)) / T))
                {
                    vcRez = vnRez;
                    vc = vn;
                }
                //else // sau punem else si l in loc de 150
            ++c;
        } while (c < 150);  // 150
        T *= 0.99; // 0.99
    } while (T > 0.000001);
    return vcRez;
}

int main()
{
    srand(time(NULL));
    double a, b;
    int prec = 5;
    int func, n, alg, improvement;

    cout << "Alegeti algoritmul: \n   1. Hill Climbing\n   2. Simulateat Annealing\n-->"; cin >> alg;
    if (alg == 1) {
        cout << "Alegeti improvement-ul:\n   1. First Improvement\n   2. Best Improvement\n-->"; cin >> improvement;
    }
    cout << "Alegeti functia:\n   1. De Jong\n   2. Schwefel\n   3. Rastrigin\n   4. Michalewicz\n-->"; cin >> func;
    initialisation(a, b, func);
    cout << "Dati dimensiunea n = "; cin >> n;
    

    vector <double> rez(30);
    vector <double> timp(30);
    if (alg == 1)
    {
        for (int i = 0; i < 10; ++i)
        {
            cout << i << " ";
            auto t1 = chrono::high_resolution_clock::now();
            double val = HC(a, b, n, prec, func, improvement);
            auto t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
            rez[i] = val;
            timp[i] = duration / 1e6;
        }
    }
    else
    {
        for (int i = 0; i < 10; ++i)
        {
            cout << i << " ";
            auto t1 = chrono::high_resolution_clock::now();
            double val = SA(a, b, n, prec, func);
            auto t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
            rez[i] = val;
            timp[i] = duration / 1e6;
            cout << rez[i] << " " << timp[i] << endl;
        }
    }
    // "f _HC/SA(b/f pt HC) _D/S/R/M _n .txt"
    string filename = "f_";
    string aux = (alg == 1 ? "HC" : "SA");
    filename += aux;
    if (alg == 1) 
        filename += (improvement == 1) ? "_f" : "_b";
    switch (func) { 
        case 1: filename += "_D"; break; 
        case 2: filename += "_S"; break;
        case 3: filename += "_R"; break;
        case 4: filename += "_M"; break;
    }
    filename += "_" + to_string(n) + ".txt";
    ofstream fout(filename);
    fout << filename << "\n\n";
    for (double x : rez)
        fout << x << " ";
    fout << "\n\n";
    for (double x : timp)
        fout << x << " ";
    return 0;
}