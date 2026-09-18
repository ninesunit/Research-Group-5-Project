#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <random>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

struct TripRecord {
    string originName;
    string destinationName;
    string mode;
    double distanceKm;
    double errorMarginKm;
    string trafficLevel;
    string compassDir;
    int hours;
    int minutes;
    double cost;
};
