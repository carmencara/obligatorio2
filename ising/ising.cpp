//---------------------------------------------------------------------
//                                                                    |
//           ALGORITMO DE METROPOLIS PARA EL MODELO DE ISING          |
//                                                                    |
//  Objetivo: realizar el programa que simula el Modelo de Ising      |
//  con el Método de Monte Carlo. Mostrar su evolución por pantalla   |
//  para diversas temperaturas.                                       |
//  - Temperatura cercana a 0: observar magnetización (color uniforme)|
//    -> empezar en estado desordenado                                |
//  - Temperatura tendiendo a infinito: observar desorden             |
//                                                                    |
//---------------------------------------------------------------------

#include <cstdlib>    // Funciones srand() y rand()
#include <ctime>      // Función time
#include <iostream>
#include <fstream>    // Usar ficheros
#include <cmath>

#define N 5           // Tamaño de la matriz de spines -1

using namespace std;

// FUNCIONES QUE SE VAN A UTILIZAR
int Random_int(int min, int max);
double Random_double();

/*---------------------------------------------------------------------
|                           FUNCIÓN PRINCIPAL                         |
---------------------------------------------------------------------*/
int main()
{   
    // ------------------- DECLARACIÓN DE VARIABLES -------------------
    int paso;         // Paso de Montecarlo
    int i,j,k;        // Contadores
    int s[N][N];  // Matriz de spines
    int x,y;          // Coordenadas aleatorias en la red
    double T;         // Temperatura
    double aux;       // Real auxiliar para asignar cond. iniciales aleatorias
    double deltaE;    // DeltaE = 2s[n][m](s[n+1][m] + s[n-1][m] + s[n][m+1] + s[n][m-1])
    double expo;      // expo=exp(-deltaE/T)
    double p;         // p = min(1, expo)
    double xi;        // Número aleatorio entre 0 y 1
    bool aleatorio;   // Elegir si los elementos de la red son aleatorios/constantes
    double iter;      // Número de iteraciones = iter
    ofstream datos;   // Fichero para guardar los spines
    

    // ------------------ CONFIGURACIONES INICIALES -------------------
    // Elegir si los elementos de la red son aleatorios (true) o definidos (false)
    aleatorio=true;
    // Tamaño de la red y paso de Montecarlo
    paso=N*N;
    iter=1000*paso; // Cambiar a 1E6
    // Temperatura inicial
    T=0.01;
    // Establecer la semilla para generar números aleatorios
    srand(time(NULL));
    // Abrir fichero
    datos.open("datos_ising.txt");

    // -------------------- CONDICIONES INICIALES ---------------------
    if(aleatorio==true) // Spines iniciales aleatorios
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                // Inicializo aleatoriamente algunos spines a +1 y otros a -1
                aux=Random_int(0,20);
                if(aux<10) s[i][j]=-1;
                else s[i][j]=1;
            }
        }
    }
    else // Spines iniciales definidos
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                s[i][j]=1;
            }
        }
    }

    // ---------------------- GUARDAR EN FICHERO -----------------------
    for(i=0;i<N;i++)
    {
        for(j=0;j<N-1;j++)
        {
            datos << s[i][j] << ", ";
        }
        datos << s[i][N-1] << endl;
    }
    datos << endl;

    // -------------------- ALGORITMO DE MONTECARLO --------------------
    for(k=0;k<iter;k++)
    {
        // ------------------- ELIJO (n,m) ALEATORIO -------------------
        x=1+rand()%N;
        y=1+rand()%N;

        // ----------------- ESCRIBO LOS NUEVOS SPINES -----------------
        if((k%paso)==0) // Sólo lo escribo en cada paso de Montecarlo
        {
            for(i=0;i<N;i++)
            {
                for(j=0;j<N-1;j++)
                {
                    datos << s[i][j]<<", ";
                }
                datos << s[i][N-1] << endl;
            }
            datos << endl;
        }

        // ------------ CONDICIONES DE CONTORNO PERIÓDICAS -------------
        for(i=0;i<N;i++)
        {
            s[0][i]=s[N-1][i];
            s[N][i]=s[0][i];
            s[i][0]=s[i][N-1];
            s[i][N]=s[i][1];
        }

        // --------------------- CÁLCULO DE deltaE ---------------------
        deltaE=2*s[x][y]*(s[x+1][y]+s[x-1][y]+s[x][y+1]+s[x][y-1]);

        // ------------ EVALUAR p = min(1, exp(−deltaE/T)) -------------
        expo=exp(-deltaE/T);
        if(expo<1) p=expo;
        else p=1;

        // ---------- GENERAR NÚMERO ALEATORIO xi ENTRE [0,1] ----------
        // ------------- SI xi<p, CAMBIAR EL SIGNO DEL SPIN ------------
        xi=Random_double();
        if(xi<p) s[x][y]=-s[x][y];
    }

    // Cerrar el fichero y finalizar
    datos.close();
    return 0;
}

/*---------------------------------------------------------------------
|                  FUNCIONES PARA NÚMEROS ALEATORIOS                  |
---------------------------------------------------------------------*/

// Función que genera un número ENTERO ALEATORIO en un rango [min,max]
int Random_int(int min, int max)
{
    // Establecer la semilla para generar números aleatorios
    srand(time(NULL));

    // Generar un número aleatorio entre min y max (ambos incluidos)
    int numero_aleatorio = rand() % (max - min + 1) + min;

    return numero_aleatorio;
}

// Función que genera un número REAL ALEATORIO en un rango [0,1]
double Random_double()
{
    // Establecer la semilla para generar números aleatorios
    srand(time(NULL));

    // Generar un número aleatorio entre 0 y 1
    double numero_aleatorio = rand()/RAND_MAX;

    return numero_aleatorio;

}