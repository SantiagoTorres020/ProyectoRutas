#pragma once

#include <string>

using namespace std;

class Arista
{
private:
    string osm_id;        // osm_id: identificador de la calle en OpenStreetMap
    int to;               // to: nodo destino al que llega la calle
    double distance_m;    // distance_m: distancia del segmento en metros
    string fclass;        // fclass: tipo de via
    int oneway;           // oneway: 1 si es un solo sentido, 0 si es doble sentido
    double maxspeed;      // maxspeed: velocidad maxima registrada

public:
    Arista()
    {
        osm_id = "";
        to = -1;
        distance_m = 0;
        fclass = "";
        oneway = 0;
        maxspeed = 0;
    }

    Arista(string osm_id,int to,double distance_m,string fclass,int oneway, double maxspeed)
    {
        this->osm_id = osm_id;
        this->to = to;
        this->distance_m = distance_m;
        this->fclass = fclass;
        this->oneway = oneway;
        this->maxspeed = maxspeed;
    }

    int obtenerDestino() const
    {
        return to;
    }

    double obtenerDistancia() const
    {
        return distance_m;
    }
};