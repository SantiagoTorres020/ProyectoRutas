#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <utility>

#include "Arista.h"

using namespace std;

class Grafo
{
private:
    vector<bool> nodoExiste;
    vector<vector<Arista>> listaAdyacencia;
    vector<vector<int>> grafoNoDirigido;
    vector<int> componenteGigante;

    int cantidadNodos;
    int cantidadAristasOriginales;
    int cantidadAristasGuardadas;
    int cantidadAristasDescartadas;

    vector<string> separarLinea(string linea, char separador)
    {
        vector<string> partes;
        string parte;
        stringstream ss(linea);

        while (getline(ss, parte, separador))
        {
            partes.push_back(parte);
        }

        return partes;
    }

    vector<int> recorrerComponenteBFS(int nodoInicial, vector<bool>& visitado)
    {
        vector<int> nodosComponente;
        queue<int> cola;

        visitado[nodoInicial] = true;
        cola.push(nodoInicial);

        while (!cola.empty())
        {
            int nodoActual = cola.front();
            cola.pop();

            nodosComponente.push_back(nodoActual);

            for (int vecino : grafoNoDirigido[nodoActual])
            {
                if (visitado[vecino] == false)
                {
                    visitado[vecino] = true;
                    cola.push(vecino);
                }
            }
        }

        return nodosComponente;
    }

public:
    Grafo()
    {
        cantidadNodos = 0;
        cantidadAristasOriginales = 0;
        cantidadAristasGuardadas = 0;
        cantidadAristasDescartadas = 0;
    }

    bool cargarNodos(string nombreArchivo)
    {
        ifstream archivoNodos(nombreArchivo);

        if (!archivoNodos.is_open())
        {
            cout << "Error: no se pudo abrir " << nombreArchivo << endl;
            return false;
        }

        string linea;
        getline(archivoNodos, linea);

        vector<int> idsNodos;
        int mayorIdNodo = -1;

        while (getline(archivoNodos, linea))
        {
            if (linea.empty())
            {
                continue;
            }

            vector<string> datos = separarLinea(linea, ',');

            if (datos.size() < 3)
            {
                continue;
            }

            int node_id = stoi(datos[0]);

            idsNodos.push_back(node_id);

            if (node_id > mayorIdNodo)
            {
                mayorIdNodo = node_id;
            }

            cantidadNodos++;
        }

        archivoNodos.close();

        if (mayorIdNodo == -1)
        {
            cout << "Error: no se encontraron nodos validos." << endl;
            return false;
        }

        nodoExiste.resize(mayorIdNodo + 1, false);
        listaAdyacencia.resize(mayorIdNodo + 1);
        grafoNoDirigido.resize(mayorIdNodo + 1);

        for (int i = 0; i < idsNodos.size(); i++)
        {
            int node_id = idsNodos[i];

            nodoExiste[node_id] = true;
        }

        return true;
    }

    bool cargarAristas(string nombreArchivo)
    {
        ifstream archivoAristas(nombreArchivo);

        if (!archivoAristas.is_open())
        {
            cout << "Error: no se pudo abrir " << nombreArchivo << endl;
            return false;
        }

        string linea;
        getline(archivoAristas, linea);

        while (getline(archivoAristas, linea))
        {
            if (linea.empty())
            {
                continue;
            }

            vector<string> datos = separarLinea(linea, ',');

            if (datos.size() < 7)
            {
                cantidadAristasDescartadas++;
                continue;
            }

            string osm_id = datos[0];
            int from_id = stoi(datos[1]);
            int to_id = stoi(datos[2]);
            double distance_m = stod(datos[3]);
            string fclass = datos[4];
            int oneway = stoi(datos[5]);
            double maxspeed = stod(datos[6]);

            if (from_id < 0 || to_id < 0 || from_id >= nodoExiste.size() || to_id >= nodoExiste.size() || nodoExiste[from_id] == false ||
                nodoExiste[to_id] == false ||
                distance_m <= 0)
            {
                cantidadAristasDescartadas++;
                continue;
            }

            listaAdyacencia[from_id].push_back(Arista(osm_id, to_id, distance_m, fclass, oneway, maxspeed));
            cantidadAristasGuardadas++;

            if (oneway == 0)
            {
                listaAdyacencia[to_id].push_back(Arista(osm_id, from_id, distance_m, fclass, oneway, maxspeed));
                cantidadAristasGuardadas++;
            }
            grafoNoDirigido[from_id].push_back(to_id);
            grafoNoDirigido[to_id].push_back(from_id);

            cantidadAristasOriginales++;
        }

        archivoAristas.close();

        return true;
    }

    void mostrarResumen()
    {
        cout << endl;
        cout << "Resumen del grafo" << endl;
        cout << "-----------------" << endl;
        cout << "Cantidad de nodos: " << cantidadNodos << endl;
        cout << "Aristas originales cargadas: " << cantidadAristasOriginales << endl;
        cout << "Aristas descartadas: " << cantidadAristasDescartadas << endl;
        cout << "Conexiones guardadas en la lista de adyacencia: ";
        cout << cantidadAristasGuardadas << endl;
    }

    vector<double> dijkstra(int nodoOrigen)
    {
        const double INFINITO = 999999999.0;

        if (nodoOrigen < 0 || nodoOrigen >= nodoExiste.size() || nodoExiste[nodoOrigen] == false)
        {
            cout << "Error: el nodo origen no existe." << endl;
            return {};
        }

        vector<double> distancia(nodoExiste.size(), INFINITO);
        vector<bool> visitado(nodoExiste.size(), false);

        distancia[nodoOrigen] = 0;

        priority_queue<pair<double, int>,vector<pair<double, int>>,greater<pair<double, int>>> cola;

        cola.push({ 0, nodoOrigen });

        while (!cola.empty())
        {
            pair<double, int> actual = cola.top();
            cola.pop();

            int nodoActual = actual.second;

            if (visitado[nodoActual] == false)
            {
                visitado[nodoActual] = true;

                for (int i = 0; i < listaAdyacencia[nodoActual].size(); i++)
                {
                    Arista arista = listaAdyacencia[nodoActual][i];

                    int nodoVecino = arista.obtenerDestino();

                    if (visitado[nodoVecino] == false)
                    {
                        double nuevaDistancia = distancia[nodoActual] + arista.obtenerDistancia();

                        if (nuevaDistancia < distancia[nodoVecino])
                        {
                            distancia[nodoVecino] = nuevaDistancia;

                            cola.push({nuevaDistancia,nodoVecino});
                        }
                    }
                }
            }
        }

        return distancia;
    }

    int contarNodosAlcanzables(int nodoOrigen, double limiteDistancia)
    {
        vector<double> distancia = dijkstra(nodoOrigen);

        if (distancia.empty())
        {
            return 0;
        }

        int cantidadAlcanzables = 0;

        for (int nodo = 0; nodo < distancia.size(); nodo++)
        {
            if (
                nodoExiste[nodo] == true &&
                distancia[nodo] <= limiteDistancia
                )
            {
                cantidadAlcanzables++;
            }
        }

        return cantidadAlcanzables;
    }

    void analizarComponentesDebiles()
    {
        vector<bool> visitado(grafoNoDirigido.size(), false);

        int cantidadIslas = 0;
        componenteGigante.clear();

        for (int nodo = 0; nodo < grafoNoDirigido.size(); nodo++)
        {
            if (nodoExiste[nodo] == true && visitado[nodo] == false)
            {
                vector<int> componenteActual =
                    recorrerComponenteBFS(nodo, visitado);

                cantidadIslas++;

                if (componenteActual.size() > componenteGigante.size())
                {
                    componenteGigante = componenteActual;
                }
            }
        }

        cout << endl;
        cout << "Analisis de islas viales" << endl;
        cout << "------------------------" << endl;
        cout << "Numero total de islas: ";
        cout << cantidadIslas << endl;
        cout << "Tamano de la componente principal: ";
        cout << componenteGigante.size() << " nodos" << endl;
    }
};