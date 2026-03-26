#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <stdio.h>
 
int ini_i = -1;
int ini_j = -1;
int dest_i = -1;
int dest_j = -1;
 
typedef struct
{
    float x;
    float y;
    bool estado;
    int color;
} Punto;
 
typedef struct
{
    int p1_i;
    int p1_j;
    int p2_i;
    int p2_j;
    int color;
} Linea;
 
typedef struct
{
    int x, y;
    int G, H, F;
    int prev;
} Espacio;
 
typedef struct
{
    Espacio* data;
    int size;
    int capacity;
} VecEspacio;
 
void vec_init(VecEspacio* v)
{
    v->size     = 0;
    v->capacity = 16;
    v->data     = (Espacio*)malloc(v->capacity * sizeof(Espacio));
}
 
void vec_free(VecEspacio* v)
{
    free(v->data);
    v->size = v->capacity = 0;
    v->data = NULL;
}
 
void vec_push(VecEspacio* v, Espacio e)
{
    if (v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = (Espacio*)realloc(v->data, v->capacity * sizeof(Espacio));
    }
    v->data[v->size++] = e;
}
 
void vec_erase(VecEspacio* v, int idx)
{
    for (int i = idx; i < v->size - 1; i++)
        v->data[i] = v->data[i + 1];
    v->size--;
}
 
Espacio espacio_new(int x, int y, int prev)
{
    Espacio e;
    e.x = x; e.y = y;
    e.prev = prev;
    e.G = e.H = e.F = 0;
    return e;
}
 
int distA(Espacio* a, Espacio* b)
{
    int dx = abs(b->x - a->x);
    int dy = abs(b->y - a->y);
 
    if (dx == 1 && dy == 1) return 14.14;  
    return 10;                           
}
 
int comparar_F(const void* a, const void* b)
{
    return ((Espacio*)a)->F - ((Espacio*)b)->F;
}
 
int BRe(int x, int y, VecEspacio* v, int* z)
{
    for (int i = 0; i < v->size; i++)
    {
        if (v->data[i].x == x && v->data[i].y == y)
        {
            *z = i;
            return 1;
        }
    }
    return 0;
}
 
void algoritmoa(Punto grid[20][20],
                int ini_x, int ini_y,
                int fin_x, int fin_y,
                int camino[][2], int* n)
{
    *n = 0;
 
    VecEspacio Analizado, Recorrido;
    vec_init(&Analizado);
    vec_init(&Recorrido);
 
    Espacio Inicial = espacio_new(ini_x, ini_y, -1);
    Espacio Final   = espacio_new(fin_x, fin_y, -1);
    vec_push(&Analizado, Inicial);
 
    while (Analizado.size > 0)
    {
        qsort(Analizado.data, Analizado.size, sizeof(Espacio), comparar_F);
 
        Espacio Actual = Analizado.data[0];
        vec_erase(&Analizado, 0);
        vec_push(&Recorrido, Actual);
        int ActualIdx = Recorrido.size - 1;
 
        if (Actual.x == Final.x && Actual.y == Final.y)
        {
            int tmp[400][2];
            int t = 0;
            int idx = ActualIdx;
 
            while (idx != -1)
            {
                tmp[t][0] = Recorrido.data[idx].x;
                tmp[t][1] = Recorrido.data[idx].y;
                t++;
                idx = Recorrido.data[idx].prev;
            }
 
            for (int k = 0; k < t; k++)
            {
                camino[k][0] = tmp[t - 1 - k][0];
                camino[k][1] = tmp[t - 1 - k][1];
            }
            *n = t;
 
            vec_free(&Analizado);
            vec_free(&Recorrido);
            return;
        }
 
        for (int j = -1; j <= 1; j++)
        {
            int ty = Actual.y + j;
            for (int i = -1; i <= 1; i++)
            {
                int tx = Actual.x + i;
                if (tx == Actual.x && ty == Actual.y) continue;
                if (tx < 0 || tx > 19 || ty < 0 || ty > 19) continue;
                if (!grid[tx][ty].estado) continue;
 
                int m = 0;
                if (BRe(tx, ty, &Recorrido, &m)) continue;
 
                Espacio vecino = espacio_new(tx, ty, ActualIdx);
                vecino.G = Actual.G + distA(&vecino, &Actual);
                vecino.H = distA(&vecino, &Final);
                vecino.F = vecino.G + vecino.H;
 
                if (BRe(tx, ty, &Analizado, &m))
                {
                    if (vecino.G > Analizado.data[m].G) continue;
                    vec_erase(&Analizado, m);
                }
                vec_push(&Analizado, vecino);
            }
        }
    }
 
    vec_free(&Analizado);
    vec_free(&Recorrido);
}
 
void setColor(int c)
{
    if(c == 1) glColor3f(0.0f, 0.0f, 0.0f);
    else if(c == 2) glColor3f(1.0f, 0.2f, 0.0f);
    else if(c == 3) glColor3f(0.0f, 0.0f, 1.0f);
    else if(c == 4) glColor3f(0.5f, 0.2f, 0.6f);
    else glColor3f(1.0f, 1.0f, 1.0f);
}
 
void pintarCamino(Punto grid[20][20], Linea lineas[1482], int camino[][2], int n, int color)
{
    for(int k = 0; k < n-1; k++)
    {
        int i1 = camino[k][0];
        int j1 = camino[k][1];
        int i2 = camino[k+1][0];
        int j2 = camino[k+1][1];
 
        for(int l = 0; l < 1482; l++)
        {
            if(
                (lineas[l].p1_i == i1 && lineas[l].p1_j == j1 &&
                 lineas[l].p2_i == i2 && lineas[l].p2_j == j2)
                ||
                (lineas[l].p1_i == i2 && lineas[l].p1_j == j2 &&
                 lineas[l].p2_i == i1 && lineas[l].p2_j == j1)
              )
            {
                lineas[l].color = color;
            }
        }
    }
}
 
void pintarPunto(Punto grid[20][20], int camino[][2], int n, int color)
{
    for(int k = 0; k < n; k++)
    {
        int i = camino[k][0];
        int j = camino[k][1];
        grid[i][j].color = color;
    }
}
 
void eliminarPunto(Punto grid[20][20])
{
    int eliminar = 80;
    int eliminados = 0;
 
    while(eliminados < eliminar)
    {
        int i = rand() % 20;
        int j = rand() % 20;
 
        if(grid[i][j].estado == 1)
        {
            grid[i][j].estado = 0;
            eliminados++;
        }
    }
}
 
void crearPuntos(Punto grid[20][20])
{
    float startX = -0.5f;
    float startY = -0.88f;
    float stepX  =  0.05f;
    float stepY  =  0.093f;
 
    for(int i = 0; i < 20; i++)
        for(int j = 0; j < 20; j++)
        {
            grid[i][j].x      = startX + j * stepX;
            grid[i][j].y      = startY + i * stepY;
            grid[i][j].estado = 1;
            grid[i][j].color  = 0;
        }
}
 
void crearLineas(Linea lineas[1482])
{
    int cont = 0;
 
    for(int i = 0; i < 20; i++)
        for(int j = 0; j < 19; j++)
        { lineas[cont].p1_i=i; lineas[cont].p1_j=j; lineas[cont].p2_i=i;   lineas[cont].p2_j=j+1; lineas[cont].color=0; cont++; }
 
    for(int i = 0; i < 19; i++)
        for(int j = 0; j < 20; j++)
        { lineas[cont].p1_i=i; lineas[cont].p1_j=j; lineas[cont].p2_i=i+1; lineas[cont].p2_j=j;   lineas[cont].color=0; cont++; }
 
    for(int i = 0; i < 19; i++)
        for(int j = 0; j < 19; j++)
        { lineas[cont].p1_i=i; lineas[cont].p1_j=j; lineas[cont].p2_i=i+1; lineas[cont].p2_j=j+1; lineas[cont].color=0; cont++; }
 
    for(int i = 0; i < 19; i++)
        for(int j = 1; j < 20; j++)
        { lineas[cont].p1_i=i; lineas[cont].p1_j=j; lineas[cont].p2_i=i+1; lineas[cont].p2_j=j-1; lineas[cont].color=0; cont++; }
}
 
void drawPuntos(Punto grid[20][20])
{
    glPointSize(10);
    glBegin(GL_POINTS);
 
    for(int i = 0; i < 20; i++)
        for(int j = 0; j < 20; j++)
            if(grid[i][j].estado == 1)
            {
                if(i == ini_i && j == ini_j)
                    glColor3f(0.0f, 1.0f, 0.0f);
                else if(i == dest_i && j == dest_j)
                    glColor3f(1.0f, 0.0f, 1.0f);
                else
                    setColor(grid[i][j].color);
 
                glVertex2f(grid[i][j].x, grid[i][j].y);
            }
 
    glEnd();
}
 
void drawLineas(Punto grid[20][20], Linea lineas[1482])
{
    glLineWidth(4);
    glBegin(GL_LINES);
 
    for(int k = 0; k < 1482; k++)
    {
        Punto p1 = grid[lineas[k].p1_i][lineas[k].p1_j];
        Punto p2 = grid[lineas[k].p2_i][lineas[k].p2_j];
 
        if(p1.estado == 1 && p2.estado == 1)
        {
            setColor(lineas[k].color);
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p2.y);
        }
    }
 
    glEnd();
}
 
void dfsCamino(Punto grid[20][20], Linea lineas[1482],
               int ini_i, int ini_j,
               int dest_i, int dest_j,
               int camino[][2], int *n)
{
    int pila_i[400], pila_j[400];
    int visitado[20][20] = {0};
    int padre_i[20][20], padre_j[20][20];
    int tope = -1;
    int recorrido[400][2];
    int r = 0;
    *n = 0;
 
    tope++;
    pila_i[tope] = ini_i;
    pila_j[tope] = ini_j;
    visitado[ini_i][ini_j] = 1;
    padre_i[ini_i][ini_j]  = -1;
    padre_j[ini_i][ini_j]  = -1;
 
    while(tope >= 0)
    {
        int i = pila_i[tope];
        int j = pila_j[tope];
        tope--;
 
        recorrido[r][0] = i;
        recorrido[r][1] = j;
        r++;
 
        if(i == dest_i && j == dest_j)
        {
            int temp[400][2];
            int t = 0;
            int ci = dest_i, cj = dest_j;
 
            while(ci != -1 && cj != -1)
            {
                temp[t][0] = ci; temp[t][1] = cj; t++;
                int pi = padre_i[ci][cj];
                int pj = padre_j[ci][cj];
                ci = pi; cj = pj;
            }
 
            for(int k = 0; k < t; k++)
            { camino[k][0] = temp[t-1-k][0]; camino[k][1] = temp[t-1-k][1]; }
            *n = t;
            return;
        }
 
        for(int k = 0; k < 1482; k++)
        {
            int ni = -1, nj = -1;
 
            if(lineas[k].p1_i == i && lineas[k].p1_j == j)
            { ni = lineas[k].p2_i; nj = lineas[k].p2_j; }
            else if(lineas[k].p2_i == i && lineas[k].p2_j == j)
            { ni = lineas[k].p1_i; nj = lineas[k].p1_j; }
 
            if(ni != -1 && nj != -1 && !visitado[ni][nj] && grid[ni][nj].estado == 1)
            {
                visitado[ni][nj] = 1;
                padre_i[ni][nj]  = i;
                padre_j[ni][nj]  = j;
                tope++;
                pila_i[tope] = ni;
                pila_j[tope] = nj;
            }
        }
    }
 
    for(int k = 0; k < r; k++)
    { camino[k][0] = recorrido[k][0]; camino[k][1] = recorrido[k][1]; }
    *n = r;
}
 
int g_camino[400][2];
int g_numNodos = 0;
 
float heuristica(Punto grid[20][20],
                 int ni, int nj,
                 int oi, int oj)
{
    int di = abs(ni - oi);              
    int dj = abs(nj - oj);              
 
    int diagonales = (di < dj) ? di : dj;  
    int rectos     = abs(di - dj);          
 
    return (float)(diagonales * 14.14f + rectos * 10.0f);
}
 
int hillClimbing(Punto grid[20][20], Linea lineas[], int numLineas,
                 int si, int sj,
                 int oi, int oj)
{
    int pila_i[400], pila_j[400];
    int tope = -1;
 
    int visitado[20][20] = {0};
    int padre_i [20][20], padre_j[20][20];
 
    tope++;
    pila_i[tope] = si;
    pila_j[tope] = sj;
    visitado[si][sj] = 1;
    padre_i [si][sj] = -1;
    padre_j [si][sj] = -1;
 
    while (tope >= 0)
    {
        int i = pila_i[tope];
        int j = pila_j[tope];
        tope--;
 
        if (i == oi && j == oj)
        {
            int tmp_i[400], tmp_j[400];
            int len = 0;
            int ci = oi, cj = oj;
 
            while (ci != -1 && cj != -1)
            {
                tmp_i[len] = ci; tmp_j[len] = cj; len++;
                int pi = padre_i[ci][cj];
                int pj = padre_j[ci][cj];
                ci = pi; cj = pj;
            }
 
            g_numNodos = len;
            for (int k = len - 1; k >= 0; k--)
            {
                int idx = len - 1 - k;
                g_camino[idx][0] = tmp_i[k];
                g_camino[idx][1] = tmp_j[k];
            }
            return 1;
        }
 
        int   vec_i[400], vec_j[400];
        float vec_h[400];
        int   numVec = 0;
 
        for (int k = 0; k < numLineas; k++)
        {
            int ni = -1, nj = -1;
 
            if (lineas[k].p1_i == i && lineas[k].p1_j == j)
            { ni = lineas[k].p2_i; nj = lineas[k].p2_j; }
            else if (lineas[k].p2_i == i && lineas[k].p2_j == j)
            { ni = lineas[k].p1_i; nj = lineas[k].p1_j; }
 
            if (ni != -1 && !visitado[ni][nj] && grid[ni][nj].estado)
            {
                vec_i[numVec] = ni;
                vec_j[numVec] = nj;
 
                int di = abs(ni - i);
                int dj = abs(nj - j);
                float costo = (di == 1 && dj == 1) ? 14.14f : 10.0f;
 
                vec_h[numVec] = heuristica(grid, ni, nj, oi, oj) + costo;
                numVec++;
            }
        }
 
        for (int a = 0; a < numVec - 1; a++)
            for (int b = 0; b < numVec - 1 - a; b++)
                if (vec_h[b] < vec_h[b+1])
                {
                    float tf = vec_h[b]; vec_h[b] = vec_h[b+1]; vec_h[b+1] = tf;
                    int   ti = vec_i[b]; vec_i[b] = vec_i[b+1]; vec_i[b+1] = ti;
                    int   tj = vec_j[b]; vec_j[b] = vec_j[b+1]; vec_j[b+1] = tj;
                }
 
        for (int a = 0; a < numVec; a++)
        {
            visitado[vec_i[a]][vec_j[a]] = 1;
            padre_i [vec_i[a]][vec_j[a]] = i;
            padre_j [vec_i[a]][vec_j[a]] = j;
            tope++;
            pila_i[tope] = vec_i[a];
            pila_j[tope] = vec_j[a];
        }
    }
 
    g_numNodos = 0;
    return 0;
}
 
void bfsCamino(Punto grid[20][20], Linea lineas[1482],
               int si, int sj,
               int ei, int ej,
               int camino[][2], int *n)
{
    *n = 0;
 
    if(si == -1 || ei == -1) return;
    if(!grid[si][sj].estado || !grid[ei][ej].estado) return;
 
    int cola_i[400], cola_j[400];
    int frente = 0, fin_cola = 0;
 
    int visitado[20][20] = {0};
    int padre_i [20][20];
    int padre_j [20][20];
 
    for(int i = 0; i < 20; i++)
        for(int j = 0; j < 20; j++)
        { padre_i[i][j] = -1; padre_j[i][j] = -1; }
 
    cola_i[fin_cola] = si;
    cola_j[fin_cola] = sj;
    fin_cola++;
    visitado[si][sj] = 1;
 
    int di[] = {-1,-1,-1, 0, 0, 1, 1, 1};
    int dj[] = {-1, 0, 1,-1, 1,-1, 0, 1};
 
    bool encontrado = false;
 
    while(frente < fin_cola && !encontrado)
    {
        int ci = cola_i[frente];
        int cj = cola_j[frente];
        frente++;
 
        if(ci == ei && cj == ej) { encontrado = true; break; }
 
        for(int d = 0; d < 8; d++)
        {
            int ni = ci + di[d];
            int nj = cj + dj[d];
 
            if(ni >= 0 && ni < 20 && nj >= 0 && nj < 20
               && grid[ni][nj].estado && !visitado[ni][nj])
            {
                visitado[ni][nj] = 1;
                padre_i [ni][nj] = ci;
                padre_j [ni][nj] = cj;
                cola_i[fin_cola] = ni;
                cola_j[fin_cola] = nj;
                fin_cola++;
            }
        }
    }
 
    if(encontrado)
    {
        int temp[400][2];
        int t = 0;
        int ci = ei, cj = ej;
 
        while(ci != -1 && cj != -1)
        {
            temp[t][0] = ci; temp[t][1] = cj; t++;
            int pi = padre_i[ci][cj];
            int pj = padre_j[ci][cj];
            ci = pi; cj = pj;
        }
 
        for(int k = 0; k < t; k++)
        { camino[k][0] = temp[t-1-k][0]; camino[k][1] = temp[t-1-k][1]; }
        *n = t;
    }
}
 
void acciones(GLFWwindow* window, Punto grid[20][20], Linea lineas[1482])
{
    static bool rP = 0, nP = 0, tP = 0, cP = 0, vP = 0, bP = 0, xP = 0;
    static int  clickP = 0, clickM = 0;
 
    /* C → DFS */
    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cP)
    {
        int camino[400][2]; int n;
        dfsCamino(grid, lineas, ini_i, ini_j, dest_i, dest_j, camino, &n);
        if(n > 0) { pintarCamino(grid, lineas, camino, n, 1); pintarPunto(grid, camino, n, 1); }
        else printf("No hay camino\n");
        cP = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) cP = 0;
 
    /* V → Hill Climbing */
    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !vP)
    {
        int ok = hillClimbing(grid, lineas, 1482, ini_i, ini_j, dest_i, dest_j);
        if(ok && g_numNodos > 0)
        { pintarCamino(grid, lineas, g_camino, g_numNodos, 2); pintarPunto(grid, g_camino, g_numNodos, 2); }
        vP = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) vP = 0;
 
    /* B → A* */
    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bP)
    {
        if(ini_i != -1 && dest_i != -1)
        {
            int camino[400][2]; int n;
            algoritmoa(grid, ini_i, ini_j, dest_i, dest_j, camino, &n);
            if(n > 0) { pintarCamino(grid, lineas, camino, n, 4); pintarPunto(grid, camino, n, 4); }
            else printf("A*: No hay camino\n");
        }
        bP = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) bP = 0;
 
    /* X → BFS */
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !xP)
    {
        int camino[400][2]; int n;
        bfsCamino(grid, lineas, ini_i, ini_j, dest_i, dest_j, camino, &n);
        if(n > 0) { pintarCamino(grid, lineas, camino, n, 3); pintarPunto(grid, camino, n, 3); }
        else printf("No hay camino (BFS)\n");
        xP = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) xP = 0;
 
    /* ESC */
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
 
    /* R */
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rP)
    { eliminarPunto(grid); rP = 1; }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) rP = 0;
 
    /* Click izquierdo */
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !clickP)
    {
        double mouseX, mouseY;
        clickP = 1;
        glfwGetCursorPos(window, &mouseX, &mouseY);
 
        float x   = (mouseX / 1900.0f) * 2.0f - 1.0f;
        float y   = -((mouseY / 1000.0f) * 2.0f - 1.0f);
        float tol = 0.02f;
 
        for(int i = 0; i < 20; i++)
            for(int j = 0; j < 20; j++)
                if(grid[i][j].estado)
                {
                    float dx = grid[i][j].x - x;
                    float dy = grid[i][j].y - y;
                    if(dx*dx + dy*dy < tol*tol)
                    {
                        printf("Click en punto grid[%d][%d]\n", i, j);
                        if(clickM == 0) { ini_i=i; ini_j=j; clickM=1; }
                        else            { dest_i=i; dest_j=j; clickM=0; }
                    }
                }
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) clickP = 0;
 
    /* N */
    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nP)
    { crearPuntos(grid); nP = 1; }
    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) nP = 0;
 
    /* T → borrar colores */
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tP)
    {
        for(int i = 0; i < 1482; i++) lineas[i].color = 0;
        for(int i = 0; i < 20; i++)
            for(int j = 0; j < 20; j++)
                grid[i][j].color = 0;
        tP = 1;
    }
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) tP = 0;
}
 
int main(void)
{
    srand(time(NULL));
 
    GLFWwindow* window;
 
    if(!glfwInit()) return -1;
 
    window = glfwCreateWindow(1900, 1000, "Camino mas corto", NULL, NULL);
    glfwMaximizeWindow(window);
 
    if(!window) { glfwTerminate(); return -1; }
 
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
 
    Punto grid[20][20];
    Linea lineas[1482];
 
    crearPuntos(grid);
    crearLineas(lineas);
 
    printf("=== Controles ===\n");
    printf("  Click izq (1ro) : nodo INICIO  (verde)\n");
    printf("  Click izq (2do) : nodo DESTINO (verde)\n");
    printf("  C               : DFS          (negro)\n");
    printf("  V               : Hill Climbing (naranja) diagonal=14.14 recto=10\n");
    printf("  B               : A*            (morado)\n");
    printf("  X               : BFS           (azul)\n");
    printf("  T               : borrar caminos\n");
    printf("  R               : eliminar 80 nodos aleatorios\n");
    printf("  N               : regenerar grid\n");
    printf("  ESC             : salir\n\n");
 
    while(!glfwWindowShouldClose(window))
    {
        acciones(window, grid, lineas);
 
        glClearColor(0.6f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        drawLineas(grid, lineas);
        drawPuntos(grid);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwTerminate();
    return 0;
}