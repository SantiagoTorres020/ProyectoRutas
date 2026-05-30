#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>

using namespace std;

struct Edge
{
    int to;
    double distance_m;
    string fclass;
    int oneway;
    double maxspeed;
};

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

vector<int> recorrerComponenteBFS(
    int nodoInicial,
    const vector<vector<int>>& grafoNoDirigido,
    vector<bool>& visitado
)
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
            if (!visitado[vecino])
            {
                visitado[vecino] = true;
                cola.push(vecino);
            }
        }
    }

    return nodosComponente;
}

void analizarComponentesDebiles(
    const vector<vector<int>>& grafoNoDirigido,
    const vector<bool>& nodoExiste,
    int cantidadNodos
)
{
    vector<bool> visitado(grafoNoDirigido.size(), false);

    int cantidadComponentes = 0;
    vector<int> componenteGigante;

    for (int nodo = 0; nodo < grafoNoDirigido.size(); nodo++)
    {
        if (nodoExiste[nodo] && !visitado[nodo])
        {
            vector<int> componenteActual = recorrerComponenteBFS(
                nodo,
                grafoNoDirigido,
                visitado
            );

            cantidadComponentes++;

            if (componenteActual.size() > componenteGigante.size())
            {
                componenteGigante = componenteActual;
            }
        }
    }

    double porcentaje = 0;

    if (cantidadNodos > 0)
    {
        porcentaje =
            100.0 * componenteGigante.size() / cantidadNodos;
    }

    cout << endl;
    cout << "Analisis de componentes debilmente conexas" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Cantidad total de islas viales: "
        << cantidadComponentes << endl;
    cout << "Cantidad de nodos en la componente gigante: "
        << componenteGigante.size() << endl;
    cout << "Porcentaje de nodos en la componente gigante: "
        << porcentaje << "%" << endl;
}

int main()
{
    ifstream archivoNodos("nodes_limpio.csv");

    if (!archivoNodos.is_open())
    {
        cout << "Error: no se pudo abrir nodes_limpio.csv" << endl;
        return 1;
    }

    string linea;
    getline(archivoNodos, linea);

    int cantidadNodos = 0;
    int mayorIdNodo = -1;
    vector<int> idsNodos;

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

        if (node_id > mayorIdNodo)
        {
            mayorIdNodo = node_id;
        }

        idsNodos.push_back(node_id);
        cantidadNodos++;
    }

    archivoNodos.close();

    cout << "Nodos leidos desde nodes_limpio.csv: "
        << cantidadNodos << endl;
    cout << "Mayor ID de nodo encontrado: "
        << mayorIdNodo << endl;

    vector<vector<Edge>> grafo(mayorIdNodo + 1);
    vector<vector<int>> grafoNoDirigido(mayorIdNodo + 1);
    vector<bool> nodoExiste(mayorIdNodo + 1, false);

    for (int id : idsNodos)
    {
        nodoExiste[id] = true;
    }

    ifstream archivoAristas("edges_limpio.csv");

    if (!archivoAristas.is_open())
    {
        cout << "Error: no se pudo abrir edges_limpio.csv" << endl;
        return 1;
    }

    getline(archivoAristas, linea);

    int filasLeidas = 0;
    int aristasDescartadas = 0;
    int aristasGuardadasEnGrafo = 0;

    while (getline(archivoAristas, linea))
    {
        if (linea.empty())
        {
            continue;
        }

        filasLeidas++;

        vector<string> datos = separarLinea(linea, ',');

        if (datos.size() < 7)
        {
            aristasDescartadas++;
            continue;
        }

        int from_id = stoi(datos[1]);
        int to_id = stoi(datos[2]);
        double distance_m = stod(datos[3]);
        string fclass = datos[4];
        int oneway = stoi(datos[5]);
        double maxspeed = stod(datos[6]);

        if (
            from_id < 0 ||
            to_id < 0 ||
            from_id > mayorIdNodo ||
            to_id > mayorIdNodo ||
            distance_m <= 0
            )
        {
            aristasDescartadas++;
            continue;
        }

        grafo[from_id].push_back({
            to_id,
            distance_m,
            fclass,
            oneway,
            maxspeed
            });

        aristasGuardadasEnGrafo++;

        if (oneway == 0)
        {
            grafo[to_id].push_back({
                from_id,
                distance_m,
                fclass,
                oneway,
                maxspeed
                });

            aristasGuardadasEnGrafo++;
        }

        grafoNoDirigido[from_id].push_back(to_id);
        grafoNoDirigido[to_id].push_back(from_id);
    }

    archivoAristas.close();

    cout << endl;
    cout << "Resumen de carga del grafo" << endl;
    cout << "--------------------------" << endl;
    cout << "Filas leidas desde edges_limpio.csv: "
        << filasLeidas << endl;
    cout << "Aristas descartadas: "
        << aristasDescartadas << endl;
    cout << "Aristas guardadas en la lista de adyacencia: "
        << aristasGuardadasEnGrafo << endl;
    cout << "Cantidad de posiciones en el grafo: "
        << grafo.size() << endl;

    analizarComponentesDebiles(
        grafoNoDirigido,
        nodoExiste,
        cantidadNodos
    );

    return 0;
}