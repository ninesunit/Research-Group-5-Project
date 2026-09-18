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

cout << fixed << setprecision(2);

    do {
        displayMainMenu(saEnabled);
        choice = getValidInt(1, 7);
//SWITCH CASE
        switch (choice) {
            case 1:
                if (activeSatellites < 4) {
                    cout << "\nError: Insufficient satellite geometry for 3D fix. Cannot calculate routes.\n";
                } else {
                    planTrip(tripHistory, localDestinations, currentLat, currentLon, currentPositionName, saEnabled, currentErrorMargin);
                }
                break;
            case 2:
                viewDestinations(localDestinations, currentLat, currentLon);
                break;
            case 3:
                checkSatelliteSignal(currentErrorMargin, activeSatellites, currentLat, currentLon);
                break;
            case 4:
                setCurrentPosition(currentLat, currentLon, currentPositionName);
                break;
            case 5:
                saEnabled = !saEnabled;
                cout << "\nSelective Availability: " << (saEnabled ? "ENABLED" : "DISABLED") << "\n";
                break;
            case 6:
                viewTripHistory(tripHistory);
                break;
            case 7:
                cout << "\nExiting GPS Navigation Assistant. System offline.\n";
                break;
            default:
                cout << "\nInvalid choice.\n";
                break;
        }
    } while (choice != 7);

    return 0;
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getValidInt(int minVal, int maxVal) {
    int input;
    while (true) {
        if (cin >> input) {
            if (input >= minVal && input <= maxVal) {
                return input;
            }
        }
        cout << "Invalid input. Enter an integer between " << minVal << " and " << maxVal << ": ";
        clearInputBuffer();
    }
}

double getValidDoubleRange(double minVal, double maxVal) {
    double input;
    while (true) {
        if (cin >> input) {
            if (input >= minVal && input <= maxVal) {
                return input;
            }
        }
        cout << "Invalid input. Enter a value between " << minVal << " and " << maxVal << ": ";
        clearInputBuffer();
    }
}


double calculateHaversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = pow(sin(dLat / 2.0), 2) + pow(sin(dLon / 2.0), 2) * cos(lat1) * cos(lat2);
    double c = 2.0 * asin(sqrt(a));
    return 6371.0 * c;
}


double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double theta = atan2(y, x);
    
    return fmod((theta * 180.0 / M_PI) + 360.0, 360.0);
}

string getCompassDirection(double bearing) {
    const char* directions[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", "N"};
    int index = static_cast<int>(round(bearing / 22.5));
    return directions[index];
}

string generateNMEA(double lat, double lon, int sats, double hdop) {
    char buffer[128];
    char latDir = (lat >= 0) ? 'N' : 'S';
    char lonDir = (lon >= 0) ? 'E' : 'W';
    
    double latAbs = fabs(lat);
    double lonAbs = fabs(lon);
    
    int latDeg = static_cast<int>(latAbs);
    double latMin = (latAbs - latDeg) * 60.0;
    
    int lonDeg = static_cast<int>(lonAbs);
    double lonMin = (lonAbs - lonDeg) * 60.0;
    
    snprintf(buffer, sizeof(buffer), "GPGGA,120000.00,%02d%07.4f,%c,%03d%07.4f,%c,1,%02d,%03.1f,50.0,M,0.0,M,,",
             latDeg, latMin, latDir, lonDeg, lonMin, lonDir, sats, hdop);

   
    unsigned char checksum = 0;
    for (int i = 0; buffer[i] != '\0'; ++i) {
        checksum ^= buffer[i];
    }
    
    char finalBuffer[150];
    snprintf(finalBuffer, sizeof(finalBuffer), "$%s*%02X", buffer, checksum);
    return string(finalBuffer);
}

void displayMainMenu(bool saEnabled) {
    cout << "\n========================================\n";
    cout << "       GPS TRIP PLANNER & ASSISTANT     \n";
    cout << "========================================\n";
    cout << "1. Plan a trip\n";
    cout << "2. View destinations\n";
    cout << "3. Satellite signal check\n";
    cout << "4. Set current position\n";
    cout << "5. Toggle Selective Availability [" << (saEnabled ? "ON" : "OFF") << "]\n";
    cout << "6. Trip history\n";
    cout << "7. Exit\n";
    cout << "========================================\n";
    cout << "Enter your choice (1 to 7): ";
}
