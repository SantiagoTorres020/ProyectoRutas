#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Edge {
    int to;                 // to: nodo destino al que llega la calle
    double distance_m;      // distance_m: distancia de la calle en metros
    string fclass;          // fclass: tipo de via, por ejemplo residential, primary, secondary
    int oneway;             // oneway: indica si la calle es de un solo sentido
    double maxspeed;        // maxspeed: velocidad maxima registrada
};

vector<string> separarLinea(string linea, char separador) 
{
    vector<string> partes;
    string parte;
    stringstream ss(linea);

    while (getline(ss, parte, separador)) {
        partes.push_back(parte);
    }

    return partes;
}

int main() 
{
    //Leer Nodos
    ifstream archivoNodos("nodes.csv");

    if (!archivoNodos.is_open()) 
    {
        cout << "Error: no se pudo abrir nodes.csv" << endl;
        return 1;
    }

    string linea;
    getline(archivoNodos, linea);

    int cantidadNodos = 0;
    int mayorIdNodo = -1;

    while (getline(archivoNodos, linea)) 
    {
        if (linea.empty())
        {
            continue;
        }

        vector<string> datos = separarLinea(linea, ',');

        if (datos.size() < 3) {
            continue;
        }

        int node_id = stoi(datos[0]); // node_id: identificador unico del nodo

        if (node_id > mayorIdNodo) 
        {
            mayorIdNodo = node_id;
        }

        cantidadNodos++;
    }

    archivoNodos.close();

    cout << "Nodos leidos desde nodes.csv: " << cantidadNodos << endl;
    cout << "Mayor ID de nodo encontrado: " << mayorIdNodo << endl;

    vector<vector<Edge>> grafo(mayorIdNodo + 1); //lista de adyacencia.

    //Leer aristas
    ifstream archivoAristas("edges.csv");

    if (!archivoAristas.is_open()) 
    {
        cout << "Error: no se pudo abrir edges.csv" << endl;
        return 1;
    }

    getline(archivoAristas, linea);

    int filasLeidas = 0;
    int aristasValidas = 0;
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

        string osm_id = datos[0];           // osm_id: identificador de la calle en OpenStreetMap
        int from_id = stoi(datos[1]);       // from_id: nodo origen de la calle
        int to_id = stoi(datos[2]);         // to_id: nodo destino de la calle
        double distance_m = stod(datos[3]); // distance_m: longitud de la calle en metros
        string fclass = datos[4];           // fclass: tipo de via
        int oneway = stoi(datos[5]);        // oneway: 1 si es un solo sentido, 0 si es doble sentido

        double maxspeed = 0;                // maxspeed: velocidad maxima, si esta vacia se guarda como 0

        if (datos[6] != "") 
        {
            maxspeed = stod(datos[6]);
        }

        if (from_id < 0 || to_id < 0 || from_id > mayorIdNodo || to_id > mayorIdNodo || distance_m <= 0) 
        {
            aristasDescartadas++;
            continue;
        }

        grafo[from_id].push_back({ to_id, distance_m, fclass, oneway, maxspeed });
        aristasGuardadasEnGrafo++;

        if (oneway == 0) 
        {
            grafo[to_id].push_back({ from_id, distance_m, fclass, oneway, maxspeed });
            aristasGuardadasEnGrafo++;
        }

        aristasValidas++;
    }

    archivoAristas.close();

    cout << endl;
    cout << "Resumen de carga del grafo" << endl;
    cout << "--------------------------" << endl;
    cout << "Filas leidas desde edges.csv: " << filasLeidas << endl;
    cout << "Aristas validas originales: " << aristasValidas << endl;
    cout << "Aristas descartadas: " << aristasDescartadas << endl;
    cout << "Aristas guardadas en la lista de adyacencia: " << aristasGuardadasEnGrafo << endl;
    cout << "Cantidad de posiciones en el grafo: " << grafo.size() << endl;

    return 0;
}