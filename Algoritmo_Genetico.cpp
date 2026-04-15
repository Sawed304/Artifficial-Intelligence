#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>

//Cantidad de Bits del rango en X
#define BitsX 5 

//Cantidad de Bits del rango en Y
#define BitsY 6

using namespace std;

////////FUNCIONES///////

//f(x,y)
double f(int x, int y) {
    double result = pow(2, x) - 2*x*y + pow(2,y); //ecuacion
    return result;
}

//Funcion escoger Maximo o el Minimo
double Max0Min(vector<double> val, bool i) {
    double result{val[0]};
    for (auto j : val) {
        if (i) {
            if (j > result) result = j;
        }
        else { if (j < result) result = j; }
    }
    return result;
}

//Funcion escoger la siguiente población
pair < vector< bitset<BitsX> >, vector< bitset<BitsY> > > Sig() {

}


/*Funcion Principal

CantIn : Número de individuos
GenId : Identificador de la generación
px : Vector de población en X en bits
py : Vector de población en Y en bits
MaxOrMin : Bool que decide si es Maximizar(1) o Minimizar(0)

*/
void Algoritmo_Genetico(int CantIn, int GenId, vector< bitset<BitsX> > px, vector< bitset<BitsY> > py, bool MaxOrMin) {

    vector<int> Vx; //Vx: vector de valores en INT de X
    vector<int> Vy; //Vy: vector de valores en INT de Y

    vector< bitset<BitsX> > Px = px; // Px: vector de valores en bits de X
    vector< bitset<BitsY> > Py = py; // Px: vector de valores en bits de Y

    //Función cruzamiento
    if(GenId != 0){
    
    }

    //Transformacion de bits a int
    for (auto i : Px) {
        Vx.push_back(i.to_ulong()); // "to_ulong()" transforma directamente los bits a enteros
    }
    for (auto i : Py) {
        Vy.push_back(i.to_ulong());
    }



    vector<double> fucn; // fucn: vector de resultados de f(Xi,Yi)
    vector<double> fucn_Sum; // fucn_Sum: vector de resultados de f(Xi,Yi) / Sumatoria
    vector<double> Ve; // Ve: vector de valores de Valor Esperado
    vector<int> Va; // Va: vector de valores de Valor Actual

    double sumatoria{0};
    double media{0};
    double Max_Min{0};

    //Columna Funcion
    for (int i = 0; i < CantIn; i++) {
        double r = f(Vx[i], Vy[i]);
        fucn.push_back(r);
        sumatoria += r;
    }

    Max_Min = Max0Min(fucn, MaxOrMin);
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

    pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> NPoblacion; //Siguiente población en "X" y "Y"



}



int main()
{

    std::cout << "Hello World!\n";
}
