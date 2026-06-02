#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <utility>
#include <algorithm>

#include "Arista.h"
#include "AristaKruskal.h"

using namespace std;

class Grafo
{
private:
    vector<bool> nodoExiste;
    vector<vector<Arista>> listaAdyacencia;
    vector<vector<int>> grafoNoDirigido;
    vector<int> componenteGigante;
    vector<AristaKruskal> listaAristasKruskal;

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

            for (int i = 0; i < grafoNoDirigido[nodoActual].size(); i++)
            {
                int vecino = grafoNoDirigido[nodoActual][i];

                if (visitado[vecino] == false)
                {
                    visitado[vecino] = true;
                    cola.push(vecino);
                }
            }
        }

        return nodosComponente;
    }

    int buscarNodoMasLejano(vector<double> distancia)
    {
        const double INFINITO = 999999999.0;

        int nodoMasLejano = -1;
        double mayorDistancia = -1;

        for (int i = 0; i < componenteGigante.size(); i++)
        {
            int nodo = componenteGigante[i];

            if (distancia[nodo] < INFINITO && distancia[nodo] > mayorDistancia)
            {
                mayorDistancia = distancia[nodo];
                nodoMasLejano = nodo;
            }
        }

        return nodoMasLejano;
    }

    static bool compararDistancia(const AristaKruskal& arista1, const AristaKruskal& arista2)
    {
        return arista1.obtenerDistancia() < arista2.obtenerDistancia();
    }

    int buscarRepresentante(int nodo, vector<int>& representante)
    {
        while (representante[nodo] != nodo)
        {
            nodo = representante[nodo];
        }

        return nodo;
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

            if (from_id < 0 || to_id < 0 || from_id >= nodoExiste.size() || to_id >= nodoExiste.size() || nodoExiste[from_id] == false || nodoExiste[to_id] == false || distance_m <= 0)
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

            listaAristasKruskal.push_back(AristaKruskal(from_id, to_id, distance_m));

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
        cout << "Conexiones guardadas en la lista de adyacencia: " << cantidadAristasGuardadas << endl;
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

        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> cola;

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

                            cola.push({ nuevaDistancia, nodoVecino });
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
            if (nodoExiste[nodo] == true && distancia[nodo] <= limiteDistancia)
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
                vector<int> componenteActual = recorrerComponenteBFS(nodo, visitado);

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
        cout << "Numero total de islas: " << cantidadIslas << endl;
        cout << "Tamano de la componente principal: " << componenteGigante.size() << " nodos" << endl;
    }

    void calcularDiametroVialAproximado()
    {
        if (componenteGigante.empty())
        {
            cout << "Error: primero se deben analizar las islas viales." << endl;
            return;
        }

        int nodoInicial = componenteGigante[0];

        vector<double> distanciaInicial = dijkstra(nodoInicial);

        int nodoOrigenDiametro = buscarNodoMasLejano(distanciaInicial);

        if (nodoOrigenDiametro == -1)
        {
            cout << "Error: no se pudo calcular el diametro vial." << endl;
            return;
        }

        vector<double> distanciaFinal = dijkstra(nodoOrigenDiametro);

        int nodoDestinoDiametro = buscarNodoMasLejano(distanciaFinal);

        if (nodoDestinoDiametro == -1)
        {
            cout << "Error: no se pudo calcular el diametro vial." << endl;
            return;
        }

        double diametroVial = distanciaFinal[nodoDestinoDiametro];

        cout << endl;
        cout << "Diametro vial aproximado" << endl;
        cout << "------------------------" << endl;
        cout << "Nodo origen: " << nodoOrigenDiametro << endl;
        cout << "Nodo destino: " << nodoDestinoDiametro << endl;
        cout << "Distancia minima: " << diametroVial << " metros" << endl;
    }

    void calcularRedEmergenciaMinima()
    {
        if (componenteGigante.empty())
        {
            cout << "Error: primero se deben analizar las islas viales." << endl;
            return;
        }

        vector<bool> perteneceComponente(nodoExiste.size(), false);

        for (int i = 0; i < componenteGigante.size(); i++)
        {
            int nodo = componenteGigante[i];

            perteneceComponente[nodo] = true;
        }

        vector<AristaKruskal> aristasComponente;

        for (int i = 0; i < listaAristasKruskal.size(); i++)
        {
            AristaKruskal arista = listaAristasKruskal[i];

            int nodoOrigen = arista.obtenerOrigen();
            int nodoDestino = arista.obtenerDestino();

            if (perteneceComponente[nodoOrigen] == true && perteneceComponente[nodoDestino] == true)
            {
                aristasComponente.push_back(arista);
            }
        }

        sort(aristasComponente.begin(), aristasComponente.end(), compararDistancia);

        vector<int> representante(nodoExiste.size());

        for (int i = 0; i < representante.size(); i++)
        {
            representante[i] = i;
        }

        double distanciaTotal = 0;
        int aristasSeleccionadas = 0;

        for (int i = 0; i < aristasComponente.size(); i++)
        {
            int nodoOrigen = aristasComponente[i].obtenerOrigen();
            int nodoDestino = aristasComponente[i].obtenerDestino();

            int representanteOrigen = buscarRepresentante(nodoOrigen, representante);
            int representanteDestino = buscarRepresentante(nodoDestino, representante);

            if (representanteOrigen != representanteDestino)
            {
                representante[representanteDestino] = representanteOrigen;

                distanciaTotal += aristasComponente[i].obtenerDistancia();

                aristasSeleccionadas++;

                if (aristasSeleccionadas == componenteGigante.size() - 1)
                {
                    break;
                }
            }
        }

        cout << endl;
        cout << "Red de emergencia minima" << endl;
        cout << "-------------------------" << endl;
        cout << "Nodos de la componente principal: " << componenteGigante.size() << endl;
        cout << "Aristas seleccionadas: " << aristasSeleccionadas << endl;
        cout << "Distancia total cubierta: " << distanciaTotal / 1000 << " km" << endl;
    }
};