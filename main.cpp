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

struct Destination {
    string name;
    double lat;
    double lon;
};

void clearInputBuffer();
int getValidInt(int minVal, int maxVal);
double getValidDoubleRange(double minVal, double maxVal);
double calculateHaversine(double lat1, double lon1, double lat2, double lon2);
double calculateBearing(double lat1, double lon1, double lat2, double lon2);
string getCompassDirection(double bearing);
string generateNMEA(double lat, double lon, int sats, double hdop);
void displayMainMenu(bool saEnabled);
void planTrip(vector<TripRecord>& history, const vector<Destination>& destinations, double originLat, double originLon, string originName, bool saEnabled, double errorMargin);
void viewDestinations(const vector<Destination>& destinations, double originLat, double originLon);
void checkSatelliteSignal(double& errorMargin, int& activeSats, double originLat, double originLon);
void setCurrentPosition(double& originLat, double& originLon, string& originName);
void viewTripHistory(const vector<TripRecord>& history);

int main() {
    vector<TripRecord> tripHistory;
    int choice = 0;

    double currentLat = 2.9278;
    double currentLon = 101.6418;
    string currentPositionName = "MMU Cyberjaya";

    bool saEnabled = false;
    double currentErrorMargin = 5.0; 
    int activeSatellites = 6; 

 vector<Destination> localDestinations = {
        {"DPULZE Shopping Centre", 2.9223, 101.6510},
        {"KLCC", 3.1578, 101.7115},
        {"KLIA", 2.7456, 101.7099},
        {"IOI City Mall", 2.9696, 101.7130},
        {"Cheras Leisure Mall", 3.0888, 101.7404},
        {"Malacca Jonker Street", 2.1953, 102.2476}
    };

