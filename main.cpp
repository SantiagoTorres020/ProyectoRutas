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

    int opcion;

    do
    {
        cout << endl;
        cout << "Menu principal" << endl;
        cout << "--------------" << endl;
        cout << "1. Alcance vehicular" << endl;
        cout << "2. Islas viales" << endl;
        cout << "3. Diametro vial aproximado" << endl;
        cout << "4. Red de emergencia minima" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion)
        {
        case 1:
        {
            int nodoOrigen;
            double limiteDistancia = 5000.0;

            cout << endl;
            cout << "Ingrese el ID del nodo origen: ";
            cin >> nodoOrigen;

            int cantidadAlcanzables = grafo.contarNodosAlcanzables(nodoOrigen, limiteDistancia);

            cout << endl;
            cout << "Desde el nodo " << nodoOrigen << " se pueden alcanzar " << cantidadAlcanzables << " nodos recorriendo como maximo 5 km." << endl;
            break;
        }

        case 2:
        {
            grafo.analizarComponentesDebiles();

            break;
        }

        case 3:
        {
            grafo.calcularDiametroVialAproximado();

            break;
        }

        case 4:
        {
            grafo.calcularRedEmergenciaMinima();

            break;
        }

        case 0:
        {
            cout << endl;
            cout << "Programa finalizado." << endl;

            break;
        }

        default:
        {
            cout << endl;
            cout << "Opcion invalida." << endl;

            break;
        }
        }
    } while (opcion != 0);

    return 0;
}