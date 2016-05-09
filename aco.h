#ifndef ACO_H_INCLUDED
#define ACO_H_INCLUDED
#endif // ACO_H_INCLUDED
#include <math.h>
#include <time.h>
///TAD Formiga
struct formiga {
    double dist_tour; //Distancia percorrida pela formiga
    int tour[30]; ///Memória da formiga n+1 (estava testando para um grafo com 6 cidades)
    int visitado[29]; //Cidades visitada 1 - para visitado / 0 - para nao visitado  n
};
typedef struct formiga tipoFormiga;

///Grafo para o problema em ACO. Contém a matriz de peso (clássica) em adição da matriz de feromônios e da regra de escolha
struct Grafo {
   int V;
   int A;
   int **adj;
   double **peso;
   double **feromonio;
   double **info;
};
typedef struct Grafo tipoGrafoACO;

///Função para alocar memória para as matrizes
double **alocaMatrizF(int linhas, int colunas) {
   int i, j;

   double **m = (double**)malloc( linhas * sizeof (double *));
   for (i = 0; i < linhas; i++)
      m[i] = (double*)malloc( colunas * sizeof (double));
   for (i = 0; i < linhas; i++)
      for (j = 0; j < colunas; j++)
         m[i][j] = 0;
   return m;
}

int **alocaMatrizI(int linhas, int colunas) {
   int i, j;

   int **m = (int**)malloc( linhas * sizeof (int *));
   for (i = 0; i < linhas; i++)
      m[i] = (int*)malloc( colunas * sizeof (int));
   for (i = 0; i < linhas; i++)
      for (j = 0; j < colunas; j++)
         m[i][j] = 0;
   return m;
}

///Inicia um grafo própio para o trabalho em ACO
void inicializaGrafo(int vertice, tipoGrafoACO *G) {
   //tipoGrafoACO G = (tipoGrafoACO*) malloc(sizeof (tipoGrafoACO));
   G->V = vertice;
   G->A = 0;
   G->peso = alocaMatrizF(vertice, vertice);
   G->feromonio = alocaMatrizF(vertice, vertice);
   G->info = alocaMatrizF(vertice, vertice);
   G->adj = alocaMatrizI(vertice, vertice);
   //return G;
}


///Procedimento que computa as distancias entre cada cidade e também inicializa os feromônios
void IniciarPesoFeromonio (tipoGrafoACO *G, int n) {

    FILE *Grafo;
    Grafo = fopen("5ulysses22.txt", "r");
    int i, j;
    double peso;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (j != i && G->adj[i][j] == 0) {
                fscanf (Grafo, "%lf ", &peso);
                G->peso[i][j] = peso;
                G->peso[j][i] = peso;
                G->feromonio[i][j] = 0.0005; //Um pouco maior do que as formigas depositariam em um tour qualquer
                G->feromonio[j][i] = 0.0005;
                G->adj[j][i] = 1;
                G->adj[i][j] = 1;
            }
            else if (j == i) {
                 G->peso[i][j] = -1;
                G->peso[j][i] = -1;
                G->feromonio[i][j] = -1;
                G->feromonio[j][i] = -1;
                G->adj[j][i] = 0;
                G->adj[i][j] = 0;
            }


        }
    }
    fclose(Grafo);
}


///Computa informações heuristicas
void ComputarInfo (tipoGrafoACO *G, int n) {

    double a = 0.5, b = 5.0, N;
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (G->adj[i][j] == 1) {
                N = 1.0/(G->peso[i][j]);
              //  printf("%e", pow(G->feromonio[i][j],a)*(pow(N, b)));
                //getchar();
                G->info[i][j] = pow(G->feromonio[i][j],a)*(pow(N, b));

                G->info[j][i] = G->info[i][j];
               // printf("info: %lf  adj[%d][%d]\n", G->info[i][j], i, j);
            }
        }
    }
}



int leMFormigas () {
    int m;
    scanf ("%d", &m);
    return m;
}

void RegraDecisao (tipoGrafoACO *G, tipoFormiga formiga[], int k, int i, int n) {

    int c = formiga[k].tour[i-1], j;
    double probabilidade = 0.0, p, r;
    double vetorSelecao[n];


    for (j = 0; j < n; j++) {

        if (formiga[k].visitado[j] == 1 || G->adj[c][j] == 0)
            vetorSelecao[j] = 0.0;
        else {
            vetorSelecao[j] = G->info[c][j];
            probabilidade += vetorSelecao[j];
        }
    }



    do {
   // time_t seconds;
    //seconds = time(NULL);
    srand(time(NULL));
    r = (probabilidade/(double)(rand()%16));
    } while (r >= probabilidade);


   // printf("%e\n", probabilidade);
    j = 0;
    p = vetorSelecao[j];

    while (p < r) {
        j++;
        p+=vetorSelecao[j];
    }

    formiga[k].tour[i] = j;
    formiga[k].visitado[j] = 1;


}


double ComputarDist (tipoGrafoACO *G, tipoFormiga formiga[], int k, int n) {

    int l, i, j;
    double dist = 0;

    for (l = 0; l < n; l++) {

        i = formiga[k].tour[l];
        j = formiga[k].tour[l+1];
        dist += G->peso[i][j];
    }

    return dist;
}

///Procedimento principal de construção de soluções onde m = numero de formigas e n = numero de cidades
void ConstruirSolucao (tipoGrafoACO *G, tipoFormiga formiga[], int m, int n) {
    //printf ("construir solucao \n\n");
    int k, i, inicio, passo;


    for (k = 0; k < m; k++) {
        for (i = 0; i < n; i++)
            formiga[k].visitado[i] = 0;
    }

    passo = 0;


    for (k = 0; k < m; k++) {
        inicio = rand()%n;
        formiga[k].tour[passo] = inicio;
        formiga[k].visitado[inicio] = 1;
    }

    while (passo < n-1) {
        passo++;

        for (k = 0; k < m; k++)
           RegraDecisao(G, formiga, k, passo, n);
    }

    for (k = 0; k < m; k++) {

            formiga[k].tour[n] = formiga[k].tour[0];
            formiga[k].dist_tour = ComputarDist(G, formiga, k, n);
        }
    }



void Evaporar (tipoGrafoACO *G, int n) {

    int i, j;
    double p = 0.2;

    for (i = 0; i < n; i++) {

        for (j = 0; j < n; j++) {
            if (G->adj[i][j] == 1) {
                G->feromonio[i][j] = (1.0 - p) * G->feromonio[i][j];
                G->feromonio[j][i] = G->feromonio[i][j];
            }
        }
    }

}

void Depositar (tipoGrafoACO *G, int n, int k, tipoFormiga formiga[]) { //K -> indentificador da formiga

    double deltaF = 1.0 / formiga[k].dist_tour;

    int i, j, l;

    for (i = 0; i < n; i++) {
        j = formiga[k].tour[i];
        l = formiga[k].tour[i+1];
        G->feromonio[j][l] += deltaF;
        G->feromonio[l][j] += deltaF;
        //printf("Feromonios: %lf [%d][%d]\n",G->feromonio[l][j], l, j);
    }
}
void AtualizarFeromonios (tipoGrafoACO *G, int m, tipoFormiga formiga[], int n) {

    int  best_formiga = 0, i;

    Evaporar(G, n);

      for (i = 1; i < m; i++) {
        if(formiga[best_formiga].dist_tour > formiga[i].dist_tour)
            best_formiga = i;
    }
    Depositar(G, n, best_formiga, formiga);

    ComputarInfo(G, n);

}


void ComputarEstatisticas (tipoGrafoACO *G, tipoFormiga formiga[], int m, int p) {


    int i, j, best_formiga = 0;

    printf("Relatorio da iteracao %d: \n", p);

    for (i = 1; i < m; i++) {
        if(formiga[best_formiga].dist_tour > formiga[i].dist_tour)
            best_formiga = i;
    }
       printf("Formiga %d: \n", best_formiga);
       printf("Distancia percorrida pela formiga: %lf\n", formiga[best_formiga].dist_tour);
       printf("Caminho feito: ");
       for(j = 0; j < 22; j++) {

       printf (" %d", formiga[best_formiga].tour[j]);

    }
    printf("\n\n");
  //  getchar();
}
