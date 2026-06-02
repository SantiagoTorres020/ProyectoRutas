#pragma once

class AristaKruskal
{
private:
    int from;
    int to;
    double distance_m;

public:
    AristaKruskal()
    {
        from = -1;
        to = -1;
        distance_m = 0;
    }

    AristaKruskal(int from, int to, double distance_m)
    {
        this->from = from;
        this->to = to;
        this->distance_m = distance_m;
    }

    int obtenerOrigen() const
    {
        return from;
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