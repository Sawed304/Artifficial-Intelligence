#include <iostream>
#include <cmath>
#include <vector>
#include <bitset>

//Cantidad de Bits del rango en X
#define BitsX 5 

//Cantidad de Bits del rango en Y
#define BitsY 6

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////FUNCIONES ALGORITMO GENETICO/////////////////////////////////////

//f(x,y)
double f(int x, int y) {
    double result = pow(2, x) - 2*x*y + pow(2,y); //ecuacion
    return result;
}

//Funcion escoger Maximo o el Minimo
double EscogerMaxOMin(vector<double> val, bool i) {
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
pair < vector< bitset<BitsX> >, vector< bitset<BitsY> > > Sig(vector< bitset<BitsX> > px, vector< bitset<BitsY> > py, int CantIn, bool MaxOrMin, vector<int> Va){
    pair < vector< bitset<BitsX> >, vector< bitset<BitsY> > > result;
    for (int i = 0; i < CantIn; i++) {
        int IndexMax0Min{ 0 };
        int Max0Min{ Va[0]};
        for (int j = 0; j < CantIn; j++) {
            if (MaxOrMin) {
                if (Va[j] > Max0Min) { 
                    Max0Min = Va[j];
                    IndexMax0Min = j;
                }
            }
            else if (Va[j] < Max0Min) {
                Max0Min = Va[j];
                IndexMax0Min = j;
            }
        }
        result.first.push_back(px[IndexMax0Min]);
        result.second.push_back(py[IndexMax0Min]);
        if (MaxOrMin) Va[IndexMax0Min]--;
        else Va[IndexMax0Min]++;
    }
    return result;
}


/*Funcion Principal

CantIn : Número de individuos
GenId : Identificador de la generación
px : Vector de población en X en bits
py : Vector de población en Y en bits
MaxOrMin : Bool que decide si es Maximizar(True) o Minimizar(False)

RETURN:
El primer pair<double,double> contendra la MEDIA y el MEJOR DATO, respectivamente, de esa generación.
El segundo pair<... , ...> contrendra los individuos escogidos para la siguiente generación.
*/
pair< pair<double, double>, pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> > Algoritmo_Genetico(int CantIn, int GenId, vector< bitset<BitsX> > px, vector< bitset<BitsY> > py, bool MaxOrMin) {

    vector<int> Vx; //Vx: vector de valores en INT de X
    vector<int> Vy; //Vy: vector de valores en INT de Y

    vector< bitset<BitsX> > Px = px; // Px: vector de valores en bits de X
    vector< bitset<BitsY> > Py = py; // Px: vector de valores en bits de Y

    //Función cruzamiento
    if(GenId != 0){
        //Se cortara siempre 2 de izquierda a derecha (00|000)
        for (int n = 0; n < CantIn / 2; n++) {
            int cont{ 0 };
            bitset<BitsX> provX1;
            bitset<BitsX> provX2;
            bitset<BitsY> provY1;
            bitset<BitsY> provY2;

            for (int i = BitsX - 1; i >= 0; i--) {
                if (cont > 2) {
                    provX1.set(i, Px[(n * 2) + 1][i]);
                    provX2.set(i, Px[(n * 2)][i]);
                }
                else {
                    provX1.set(i, Px[(n * 2)][i]);
                    provX2.set(i, Px[(n * 2) + 1][i]);
                }
                cont++;
            }

            cont = 0;

            for (int i = BitsY - 1; i >= 0; i--) {
                if (cont > 2) {
                    provY1.set(i, Py[(n * 2) + 1][i]);
                    provY2.set(i, Py[(n * 2)][i]);
                }
                else {
                    provY1.set(i, Py[(n * 2)][i]);
                    provY2.set(i, Py[(n * 2) + 1][i]);
                }
                cont++;
            }

            Px[(n * 2)] = provX1;
            Px[(n * 2) + 1] = provX2;
            Py[(n * 2)] = provY1;
            Py[(n * 2) + 1] = provY2;
        }
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

    if (sumatoria == 0) {
        // manejar caso especial
        cout << "Sumatoria es 0 en generacion " << GenId << endl;
        exit;
    }

    Max_Min = EscogerMaxOMin(fucn, MaxOrMin);
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

    pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> NewPoblacion; //Siguiente población en "X" y "Y"

    NewPoblacion = Sig(Px, Py, CantIn, MaxOrMin, Va);

    pair<double, double> MediaAndDatoRele(media, Max_Min);

    pair< pair<double, double>, pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> > MyPair(MediaAndDatoRele, NewPoblacion);

    return MyPair;

}



//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////FUNCIONES COMPLEMENTARIAS////////////////////////////////////////




int main()
{

    std::cout << "Hello World!\n";
}
