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
#include "gsl_rng.h" //Libreria para generación de números aleatorios
#include <sys/time.h>

#define N 64           // Tamaño de la matriz de spines -1

using namespace std;

// FUNCIONES QUE SE VAN A UTILIZAR
long int SemillaTiempo();
int Random_int(int max, gsl_rng *tau);
double Random_double(gsl_rng *tau);
double DeltaEnergia(int s[N][N], int x, int y);

/*---------------------------------------------------------------------
|                           FUNCIÓN PRINCIPAL                         |
---------------------------------------------------------------------*/
int main()
{   
    // ------------------- DECLARACIÓN DE VARIABLES -------------------
    int N2;         // Paso de Montecarlo
    int i,j,k;        // Contadores
    int s[N][N];      // Matriz de spines
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
    gsl_rng *tau;
    int semilla;


    // ------------------ CONFIGURACIONES INICIALES -------------------
    // Elegir si los elementos de la red son aleatorios (true) o definidos (false)
    aleatorio=true;
    // Paso de Montecarlo
    N2=N*N;
    // Temperatura inicial
    T=1.5;
    // Establecer la semilla para generar números aleatorios
    semilla=SemillaTiempo();
    tau = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(tau,semilla);

    // Abrir fichero
    datos.open("datos_ising_gsl.txt");

    // -------------------- CONDICIONES INICIALES ---------------------
    if(aleatorio==true) // Spines iniciales aleatorios
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                // Inicializo aleatoriamente algunos spines a +1 y otros a -1
                aux=Random_int(100,tau);
                if(aux<50) s[i][j]=-1;
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
    iter=1000*N2;
    for(k=1;k<=iter;k++)
    {
        // ------------------- ELIJO (n,m) ALEATORIO -------------------
        x=rand()%N;
        y=rand()%N;

        // --------------------- CÁLCULO DE deltaE ---------------------
        deltaE=DeltaEnergia(s, x, y);

        // ------------ EVALUAR p = min(1, exp(−deltaE/T)) -------------
        expo=exp(-1.0*deltaE/T);
        if(expo<1) p=expo;
        else p=1;

        // ---------- GENERAR NÚMERO ALEATORIO xi ENTRE [0,1] ----------
        // ------------- SI xi<p, CAMBIAR EL SIGNO DEL SPIN ------------
        xi=Random_double(tau);
        if(xi<p) s[x][y]=-s[x][y];

        // ----------------- ESCRIBO LOS NUEVOS SPINES -----------------
        if(k%N2==0) // Sólo lo escribo en cada paso de Montecarlo
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
    }

    // Cerrar el fichero y finalizar
    datos.close();
    return 0;
}

/*---------------------------------------------------------------------
|                  FUNCIONES PARA NÚMEROS ALEATORIOS                  |
---------------------------------------------------------------------*/

// Función que crea una semilla para generar números aleatorios, basada en el tiempo actual
long int SemillaTiempo()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    return (tv.tv_sec + tv.tv_usec);
}

// Función que genera un número ENTERO ALEATORIO en un rango [min,max]
int Random_int(int max, gsl_rng *tau)
{   
    // Generar un número aleatorio entre 0 y max (ambos incluidos)
    int numero_aleatorio = gsl_rng_uniform_int(tau,max+1);

    return numero_aleatorio;
}

// Función que genera un número REAL ALEATORIO en un rango [0,1]
double Random_double(gsl_rng *tau)
{
    // Generar un número aleatorio entre 0 y 1
    double numero_aleatorio = gsl_rng_uniform(tau);

    return numero_aleatorio;

}

/*---------------------------------------------------------------------
|              FUNCIONES PARA EL ALGORITMO DE MONTECARLO              |
---------------------------------------------------------------------*/

// Función que devuelve deltaE a partir de una posición (x,y) aleatoria
// en la red y aplicando las CONDICIONES DE CONTORNO PERIÓDICAS:
// s[0][i]=s[N][i], s[N+1][i]=s[1][i], s[i][0]=s[i][N], s[i][N+1]=s[i][1]

double DeltaEnergia(int s[N][N], int x, int y)
{
    // x, y son las posiciones aleatorias en la red
    int x_plus, x_subt, y_plus, y_subt;
    double deltaE;

    x_plus=x+1;
    x_subt=x-1;
    y_plus=y+1;
    y_subt=y-1;

    // Condiciones de contorno periódicas: 
    if(x_subt==-1) x_subt=N-1;
    if(x_plus==N) x_plus=0;
    if(y_subt==-1) y_subt=N-1;
    if(y_plus==N) y_plus=0;

    // deltaE = 2s[n][m](s[n+1][m] + s[n-1][m] + s[n][m+1] + s[n][m-1])
    deltaE= 2.0*s[x][y]*(s[x_plus][y]+s[x_subt][y]+s[x][y_plus]+s[x][y_subt]);
    return deltaE;
}