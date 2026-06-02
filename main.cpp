#include <iostream>

#include "Grafo.h"

using namespace std;

int main()
{
    Grafo grafo;

    if (!grafo.cargarNodos("nodes_limpio.csv"))
    {
        return 1;
    }

    if (!grafo.cargarAristas("edges_limpio.csv"))
    {
        return 1;
    }

    grafo.mostrarResumen();

    grafo.analizarComponentesDebiles();

    int nodoOrigen;

    cout << endl;
    cout << "Ingrese el ID del nodo origen: ";
    cin >> nodoOrigen;

    double limiteDistancia = 5000.0;

    int cantidadAlcanzables =
        grafo.contarNodosAlcanzables(
            nodoOrigen,
            limiteDistancia
        );

    cout << endl;
    cout << "Desde el nodo " << nodoOrigen;
    cout << " se pueden alcanzar ";
    cout << cantidadAlcanzables;
    cout << " nodos recorriendo como maximo 5 km." << endl;

    return 0;
}