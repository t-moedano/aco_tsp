#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "aco.h"



int main()
{
    tipoGrafoACO *G = (tipoGrafoACO*) malloc(sizeof (tipoGrafoACO));

    int m, p;
    int vertice = 22;
    double n = 22;

    inicializaGrafo(vertice, G); //passar G como parametro
    m = leMFormigas();
    tipoFormiga f[m];


    IniciarPesoFeromonio(G, n);

    ComputarInfo(G, vertice);

    for (p = 0; p < 10; p++) {

        ConstruirSolucao(G, f, m, vertice);
        AtualizarFeromonios(G, m, f, vertice);
        ComputarEstatisticas(G, f, m, p);
    }
    return 0;
}
