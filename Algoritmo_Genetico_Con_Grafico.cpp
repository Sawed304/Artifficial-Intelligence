#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <bitset>
#include <GL/glut.h>
#include <string>
#include <thread>
#include <mutex>

//Cantidad de Bits del rango en X
#define BitsX 5 

//Cantidad de Bits del rango en Y
#define BitsY 6

using namespace std;

mutex m;
bool SumList{ false };

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////FUNCIONES ALGORITMO GENETICO/////////////////////////////////////

//f(x,y)
double f(int x, int y) {
    //double result = pow(2, x) - 2 * x * y + pow(2, y); // ecuacion x² - 2xy + y² 
    //double result = 2 * x + y;
    double result = (2,(x - y));
    return result;
}

//Funcion escoger Maximo o el Minimo
double EscogerMaxOMin(vector<double> val, bool i) {
    double result{ val[0] };
    for (auto j : val) {
        if (i) {
            if (j > result) result = j;
        }
        else { if (j < result) result = j; }
    }
    return result;
}

//<Funcion para escoger la siguiente población>
/////////////////////////////////////////////
///////SELECCION PROPORCIONAL///////////////

pair < vector< bitset<BitsX> >, vector< bitset<BitsY> > > Sig(vector< bitset<BitsX> > px, vector< bitset<BitsY> > py, int CantIn, bool MaxOrMin, vector<int> Va) {
    pair < vector< bitset<BitsX> >, vector< bitset<BitsY> > > result;

    //Si es MINIMIZAR
    if (!MaxOrMin) {
        for (auto& i : Va) i *= -1; //Cada elemento se multiplica por -1
        MaxOrMin = !MaxOrMin; //MaxOrMin ahora es MAXIMIZAR 
    }
    for (int i = 0; i < CantIn; i++) {
        int IndexMax0Min{ 0 };
        int Max0Min{ Va[0] };
        for (int j = 0; j < CantIn; j++) {
            if (Va[j] > Max0Min) {
                Max0Min = Va[j];
                IndexMax0Min = j;
            }
        }
        result.first.push_back(px[IndexMax0Min]);
        result.second.push_back(py[IndexMax0Min]);
        Va[IndexMax0Min]--;
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
////////////////////FUNCION INDIVIDUO////////////////////////////////////
// <Funcion llamada con threads>
void callIndividuo(int CantIn, int idPi, vector< bitset<BitsX> > Px, vector< bitset<BitsY> > Py, vector<int>& Vx, vector<int>& Vy, vector<double>& fucn, double& sumatoria, vector<double>& fucn_Sum, vector<double>& Ve, vector<int>& Va) {

    //Transformacion de bits a int
    //Columna Vx and Vy
    Vx[idPi] = (Px[idPi]).to_ulong(); // "to_ulong()" transforma directamente los bits a enteros
    Vy[idPi] = (Py[idPi]).to_ulong(); // "to_ulong()" transforma directamente los bits a enteros
    
    //Columna funcion
    fucn[idPi] = f(Vx[idPi], Vy[idPi]);
    
    //Mutex 
    m.lock();
    sumatoria += fucn[idPi];
    m.unlock();
    
    //Caso donde la sumatoria este lista:
    if (idPi == CantIn - 1) SumList = true; // Donde estamos en el ultimo thread

    while (!SumList) {
        //Esperada hasta que "Sumatoria" este listo para usar
    }

    //Columna Funcion / Sumatoria
    fucn_Sum[idPi] = fucn[idPi] / sumatoria;
    
    //Columna valor esperado
    Ve[idPi] = CantIn * fucn_Sum[idPi];

    //Columna valor actual
    Va[idPi] = round(Ve[idPi]); // "round" redondea
  
}



/*Funcion Principal

CantIn : Número de individuos
GenId : Identificador de la generación
px : Vector de población en X en bits
py : Vector de población en Y en bits
MaxOrMin : Bool que decide si es Maximizar(True) o Minimizar(False)

RETURN:
El primer pair<double,double> contiene la MEDIA y el MEJOR DATO, respectivamente, de esa generación.
El segundo pair<... , ...> contrendra los individuos escogidos para la siguiente generación.
*/
pair< pair<double, double>, pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> > Algoritmo_Genetico(int CantIn, int GenId, vector< bitset<BitsX> > px, vector< bitset<BitsY> > py, bool MaxOrMin) {

    vector< bitset<BitsX> > Px = px; // Px: vector de valores en bits de X
    vector< bitset<BitsY> > Py = py; // Px: vector de valores en bits de Y

    //FUNCION CRUZAMIENTO
    if (GenId != 0) {
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

    vector<int> Vx(CantIn); //Vx: vector de valores en INT de X
    vector<int> Vy(CantIn); //Vy: vector de valores en INT de Y
    vector<double> fucn(CantIn); // fucn: vector de resultados de f(Xi,Yi)
    vector<double> fucn_Sum(CantIn); // fucn_Sum: vector de resultados de f(Xi,Yi) / Sumatoria
    vector<double> Ve(CantIn); // Ve: vector de valores de Valor Esperado
    vector<int> Va(CantIn); // Va: vector de valores de Valor Actual

    double sumatoria{ 0 };
    double media{ 0 };
    double Max_Min{ 0 };

    //////////////////////////////////////////////////////////////////////////
    /////////////////////////THREADS/////////////////////////////////////////
    vector<thread> myThreads(CantIn);

    for (int i{ 0 }; i < CantIn;i++) {
        myThreads[i] =  thread( callIndividuo,CantIn, i, Px, Py, ref(Vx), ref(Vy), ref(fucn), ref(sumatoria), ref(fucn_Sum), ref(Ve), ref(Va));
    }

    // Esperar que todos terminen antes de continuar
    for (int i{ 0 }; i < CantIn; i++) {
        myThreads[i].join();
    }
    
    SumList = false; //Reseteo para la proxima generación

    Max_Min = EscogerMaxOMin(fucn, MaxOrMin);
    media = sumatoria / CantIn;

    pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> NewPoblacion; //Vector de la siguiente población en "X" y "Y"

    NewPoblacion = Sig(Px, Py, CantIn, MaxOrMin, Va); // <Funcion para escoger la siguiente población>

    pair<double, double> MediaAndDatoRele(media, Max_Min);

    pair< pair<double, double>, pair<vector< bitset<BitsX> >, vector< bitset<BitsY> >> > MyPair(MediaAndDatoRele, NewPoblacion);

    return MyPair;

}



//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////FUNCIONES COMPLEMENTARIAS////////////////////////////////////////


//<Asignar una población inicial aleatoria>
void PoblacionInicial(int CantIn, vector<bitset<BitsX>>& Px, vector<bitset<BitsY>>& Py) {

    srand(time(0));

    for (int i = 0; i < CantIn; i++) {
        Px.push_back(bitset<BitsX>(rand() % (int)pow(2, BitsX)));
        Py.push_back(bitset<BitsY>(rand() % (int)pow(2, BitsY)));
    }
}


////////////////////////////////////////////////////////////////////
////////////////////////VISUALIZACIÓN GLUT//////////////////////////

// Datos globales para graficar
vector<double> g_medias;
vector<double> g_mejores;
int g_cantGen = 0;

int g_winW = 900, g_winH = 600;

// Margenes del área de graficado
const int MARGIN_L = 90;
const int MARGIN_R = 40;
const int MARGIN_T = 50;
const int MARGIN_B = 60;

void drawTextGL(float x, float y, const string& text) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

void drawTextSmall(float x, float y, const string& text) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
}

void display_cb() {
    glClear(GL_COLOR_BUFFER_BIT);

    int W = g_winW, H = g_winH;
    int gW = W - MARGIN_L - MARGIN_R;
    int gH = H - MARGIN_T - MARGIN_B;

    // Calcular rango Y automaticamente
    double minY = g_medias[0], maxY = g_medias[0];
    for (double v : g_medias) { minY = min(minY, v); maxY = max(maxY, v); }
    for (double v : g_mejores) { minY = min(minY, v); maxY = max(maxY, v); }
    double rangeY = maxY - minY;
    if (rangeY < 1e-10) rangeY = 1.0;
    // Padding del 10% arriba y abajo
    double padY = rangeY * 0.1;
    minY -= padY;
    maxY += padY;
    rangeY = maxY - minY;

    // Lambda para convertir datos a coordenadas de pantalla
    auto toScreenX = [&](int gen) -> float {
        return MARGIN_L + (gen * gW) / (float)(g_cantGen - 1);
        };
    auto toScreenY = [&](double val) -> float {
        return MARGIN_B + ((val - minY) / rangeY) * gH;
        };

    // Fondo del area de graficado
    glColor3f(0.97f, 0.97f, 0.97f);
    glBegin(GL_QUADS);
    glVertex2i(MARGIN_L, MARGIN_B);
    glVertex2i(MARGIN_L + gW, MARGIN_B);
    glVertex2i(MARGIN_L + gW, MARGIN_B + gH);
    glVertex2i(MARGIN_L, MARGIN_B + gH);
    glEnd();

    // Grilla horizontal (5 divisiones en Y)
    int NUM_DIV_Y = 5;
    glColor3f(0.80f, 0.80f, 0.80f);
    glLineWidth(1.0f);
    for (int k = 0; k <= NUM_DIV_Y; k++) {
        double val = minY + (rangeY * k) / NUM_DIV_Y;
        int y = toScreenY(val);
        glBegin(GL_LINES);
        glVertex2i(MARGIN_L, y);
        glVertex2i(MARGIN_L + gW, y);
        glEnd();
        // Etiqueta Y
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f", val);
        glColor3f(0.3f, 0.3f, 0.3f);
        drawTextSmall(MARGIN_L - 55, y - 4, string(buf));
        glColor3f(0.80f, 0.80f, 0.80f);
    }

    // Grilla vertical (una por generacion, max 10 lineas)
    int stepX = max(1, g_cantGen / 10);
    for (int g = 0; g < g_cantGen; g += stepX) {
        int x = toScreenX(g);
        glBegin(GL_LINES);
        glVertex2i(x, MARGIN_B);
        glVertex2i(x, MARGIN_B + gH);
        glEnd();
        // Etiqueta X
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", g + 1);
        glColor3f(0.3f, 0.3f, 0.3f);
        drawTextSmall(x - 5, MARGIN_B - 18, string(buf));
        glColor3f(0.80f, 0.80f, 0.80f);
    }

    // Borde del area
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(MARGIN_L, MARGIN_B);
    glVertex2i(MARGIN_L + gW, MARGIN_B);
    glVertex2i(MARGIN_L + gW, MARGIN_B + gH);
    glVertex2i(MARGIN_L, MARGIN_B + gH);
    glEnd();

    // Curva MEDIA (azul)
    glColor3f(0.23f, 0.51f, 0.96f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (int g = 0; g < g_cantGen; g++)
        glVertex2f(toScreenX(g), toScreenY(g_medias[g]));
    glEnd();

    // Curva MEJOR (rojo)
    glColor3f(0.94f, 0.27f, 0.27f);
    glLineWidth(2.5f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xF0F0); // linea punteada
    glBegin(GL_LINE_STRIP);
    for (int g = 0; g < g_cantGen; g++)
        glVertex2f(toScreenX(g), toScreenY(g_mejores[g]));
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // Titulo
    glColor3f(0.1f, 0.1f, 0.1f);
    drawTextGL(W / 2 - 100, H - 25, "Convergencia - Algoritmo Genetico");

    // Label eje X
    drawTextSmall(W / 2 - 30, MARGIN_B - 40, "Generacion");

    // Label eje Y (rotado no es directo en GLUT, usamos texto vertical)
    for (int k = 0; k < 7; k++)
        drawTextSmall(12, MARGIN_B + gH / 2 - 30 + k * 10, string(1, "Fitness"[k]));

    // LEYENDA
    int lx = MARGIN_L + gW - 160;
    int ly = MARGIN_B + gH - 20;

    glColor3f(0.95f, 0.95f, 0.95f);
    glBegin(GL_QUADS);
    glVertex2i(lx - 10, ly - 30);
    glVertex2i(lx + 155, ly - 30);
    glVertex2i(lx + 155, ly + 15);
    glVertex2i(lx - 10, ly + 15);
    glEnd();
    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(lx - 10, ly - 30);
    glVertex2i(lx + 155, ly - 30);
    glVertex2i(lx + 155, ly + 15);
    glVertex2i(lx - 10, ly + 15);
    glEnd();

    // Línea azul leyenda
    glColor3f(0.23f, 0.51f, 0.96f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2i(lx, ly);
    glVertex2i(lx + 25, ly);
    glEnd();
    glColor3f(0.1f, 0.1f, 0.1f);
    drawTextSmall(lx + 30, ly - 4, "Media");

    // Línea roja punteada leyenda
    glColor3f(0.94f, 0.27f, 0.27f);
    glLineWidth(2.5f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xF0F0);
    glBegin(GL_LINES);
    glVertex2i(lx + 80, ly);
    glVertex2i(lx + 105, ly);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawTextSmall(lx + 110, ly - 4, "Mejor");

    glutSwapBuffers();
}

void reshape_cb(int w, int h) {
    if (w == 0 || h == 0) return;
    g_winW = w; g_winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard_cb(unsigned char key, int, int) {
    if (key == 27) exit(0); // ESC para salir
}

void Graficar(int argc, char** argv, vector<double> medias, vector<double> mejores, int cantGen) {
    g_medias = medias;
    g_mejores = mejores;
    g_cantGen = cantGen;

    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(g_winW, g_winH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Algoritmo Genetico - Convergencia");
    glutDisplayFunc(display_cb);
    glutReshapeFunc(reshape_cb);
    glutKeyboardFunc(keyboard_cb);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glutMainLoop();
}

////////////////////////////////////////////////////////////////////
//////////////////////// MAIN /////////////////////////////////////
int main(int argc, char** argv)
{
    int CantIn{ 0 };
    bool MaxOrMin{ false };
    vector<bitset<BitsX>> Px;
    vector<bitset<BitsY>> Py;
    cout << "\n \n Ahora ingrese la cantidad de individuos por cada generacion (Tiene que ser par) : ";
    cin >> CantIn;
    cout << "\n \n Maximizar(1) o Minimizar(0) ? : ";
    cin >> MaxOrMin;

    PoblacionInicial(CantIn, Px, Py);

    int CantGen = 10;

    vector<double> vectorMedias;
    vector<double> vectorMejores;
    
    
    cout <<endl << "Poblacion X: " << endl;
    for (auto i : Px) {
        cout << i<<endl;
    }

    cout << "Poblacion Y: " << endl;
    for (auto i : Py) {
        cout << i << endl;
    }
    

    
    for (int i{ 0 }; i < CantGen; i++) {
        auto resultado = Algoritmo_Genetico(CantIn, i, Px, Py, MaxOrMin);

        vectorMedias.push_back(resultado.first.first);
        vectorMejores.push_back(resultado.first.second);
        Px = resultado.second.first; 
        Py = resultado.second.second;

        cout << "GENERACION " << i << endl;
        cout << "Media: " << vectorMedias[i] << endl;
        cout << "Mejor dato: " << vectorMejores[i] << endl;
        cout << endl << "=============================" << endl;


    }

    Graficar(argc, argv, vectorMedias, vectorMejores, CantGen);
    
}
