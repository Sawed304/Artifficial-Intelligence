#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>

#define BitsX 5
#define BitsY 6

using namespace std;

////////FUNCIONES///////

//f(x,y)
double f(int x, int y) {
    double result = pow(2, x) - 2*x*y + pow(2,y); //ecuacion
    return result;
}

//principal
void Algoritmo_Genetico(int CantIn, int GenId, vector<int> px, vector<int> py) {

    vector< bitset<BitsX> > Px;
    vector< bitset<BitsY> > Py;

    //Población "x" y "y" guardados en bits
    for (int i : px) {
        Px.push_back(bitset<BitsX>(i));
    }
    for (int i : py) {
        Py.push_back(bitset<BitsY>(i));
    }

    vector<int> vx = px;
    vector<int> vy = py;

    //Función cruzamiento
    if(GenId != 0){
    
    }

    vector<double> fucn;
    vector<double> fucn_Sum;
    vector<double> Ve;
    vector<int> Va;

    double sumatoria{0};
    double media{0};

    //Columna Funcion
    for (int i = 0; i < CantIn; i++) {
        double r = f(vx[i], vy[i]);
        fucn.push_back(r);
        sumatoria += r;
    }

    media = sumatoria / CantIn;

    //Columna Funcion / Sumatoria
    for (auto i : fucn) {
        fucn_Sum.push_back(i/sumatoria);
    }

    //Columna valor esperado
    for (auto i : fucn_Sum) {
        Ve.push_back(i * CantIn);
    }

    //Columna valor actual
    for (auto i : Ve) {
        Va.push_back( round(i) ); // "round" redondea
    }



}



int main()
{

    std::cout << "Hello World!\n";
}
