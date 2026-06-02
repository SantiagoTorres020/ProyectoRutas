#pragma once

#include <string>

using namespace std;

class Arista
{
private:
    string osm_id;
    int to;
    double distance_m;
    string fclass;
    int oneway;
    double maxspeed;

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

    Arista(string osm_id, int to, double distance_m, string fclass, int oneway, double maxspeed)
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