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

void setCurrentPosition(double& originLat, double& originLon, string& originName) {
    cout << "\n========================================\n";
    cout << "          SET CURRENT POSITION          \n";
    cout << "========================================\n";
    cout << "Enter Latitude (-90.0 to 90.0): ";
    originLat = getValidDoubleRange(-90.0, 90.0);
    
    cout << "Enter Longitude (-180.0 to 180.0): ";
    originLon = getValidDoubleRange(-180.0, 180.0);
    
    cout << "Enter location name identifier: ";
    cin >> ws;
    getline(cin, originName);
    
    cout << "\nOrigin updated successfully to " << originName << ".\n";
    cout << "========================================\n";
}

void planTrip(vector<TripRecord>& history, const vector<Destination>& destinations, double originLat, double originLon, string originName, bool saEnabled, double errorMargin) {
    cout << "\n========================================\n";
    cout << "            TRIP PLANNER                \n";
    cout << "========================================\n";
    cout << "Origin: " << originName << " [Lat: " << setprecision(4) << originLat << ", Lon: " << originLon << "]\n";
    cout << setprecision(2);
    
    for (size_t i = 0; i < destinations.size(); ++i) {
        double dist = calculateHaversine(originLat, originLon, destinations[i].lat, destinations[i].lon);
        cout << (i + 1) << ". " << destinations[i].name << " (" << dist << " km)\n";
    }
    
    cout << "Enter choice (1 to " << destinations.size() << "): ";
    int destChoice = getValidInt(1, static_cast<int>(destinations.size()));
    Destination selectedDest = destinations[destChoice - 1];
    
    double trueDistance = calculateHaversine(originLat, originLon, selectedDest.lat, selectedDest.lon);
    double bearing = calculateBearing(originLat, originLon, selectedDest.lat, selectedDest.lon);
    string compassDir = getCompassDirection(bearing);
    
    double perceivedLat = originLat;
    double perceivedLon = originLon;

if (saEnabled) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(-0.0009, 0.0009); 
        perceivedLat += dist(gen);
        perceivedLon += dist(gen);
    }
    
    double perceivedDistance = calculateHaversine(perceivedLat, perceivedLon, selectedDest.lat, selectedDest.lon);

double displayErrorMarginKm = errorMargin >= 0.0 ? errorMargin / 1000.0 : -1.0;
    if (saEnabled && displayErrorMarginKm >= 0.0) {
        displayErrorMarginKm += 0.10; 
    }

    
    cout << "\nSelect Transport Mode:\n";
    cout << "1. Car (Base speed: 60 km/h, Cost: RM 0.60/km)\n";
    cout << "2. Motorcycle (Base speed: 50 km/h, Cost: RM 0.25/km)\n";
    cout << "3. Bus (Base speed: 40 km/h, Fare: RM 0.15/km)\n";
    cout << "4. Walking (Base speed: 5 km/h, Cost: RM 0.00/km)\n";
    cout << "Enter mode choice (1 to 4): ";
    int modeChoice = getValidInt(1, 4);

    string modeName;
    double baseSpeed = 0.0;
    double costPerKm = 0.0;

    switch (modeChoice) {
        case 1: modeName = "Car"; baseSpeed = 60.0; costPerKm = 0.60; break;
        case 2: modeName = "Motorcycle"; baseSpeed = 50.0; costPerKm = 0.25; break;
        case 3: modeName = "Bus"; baseSpeed = 40.0; costPerKm = 0.15; break;
        case 4: modeName = "Walking"; baseSpeed = 5.0; costPerKm = 0.00; break;
    }

    cout << "\nSelect Traffic Level:\n";
    cout << "1. Light (100% normal speed)\n";
    cout << "2. Moderate (75% normal speed)\n";
    cout << "3. Heavy (40% normal speed)\n";
    cout << "Enter traffic level (1 to 3): ";
    int trafficChoice = getValidInt(1, 3);

    string trafficName;
    double speedMultiplier = 1.0;

    if (trafficChoice == 1) { trafficName = "Light"; speedMultiplier = 1.00; } 
    else if (trafficChoice == 2) { trafficName = "Moderate"; speedMultiplier = 0.75; } 
    else { trafficName = "Heavy"; speedMultiplier = 0.40; }

    double effectiveSpeed = baseSpeed * speedMultiplier;
    double totalTimeHours = perceivedDistance / effectiveSpeed;
    
    int hours = static_cast<int>(totalTimeHours);
    int minutes = static_cast<int>(round((totalTimeHours - hours) * 60.0));
    
    if (minutes == 60) {
        hours += 1;
        minutes = 0;
    }

    double totalCost = perceivedDistance * costPerKm;

    cout << "\n========================================\n";
    cout << "            TRIP SUMMARY                \n";
    cout << "========================================\n";
    cout << "Destination     : " << selectedDest.name << "\n";
    cout << "Bearing         : " << fixed << setprecision(1) << bearing << " deg (" << compassDir << ")\n";

if (saEnabled) {
        int saErrorMeters = static_cast<int>(round(fabs(perceivedDistance - trueDistance) * 1000.0));
        cout << "True Distance   : " << setprecision(2) << trueDistance << " km\n";
        if (displayErrorMarginKm < 0.0) {
            cout << "Reported        : " << setprecision(2) << perceivedDistance << " km (Error: N/A)   [SA error: " << saErrorMeters << " m]\n";
        } else {
            cout << "Reported        : " << setprecision(2) << perceivedDistance << " km (+/- " << setprecision(3) << displayErrorMarginKm << " km)   [SA error: " << saErrorMeters << " m]\n";
        }
        cout << "[SA ALERT] Intentional signal degradation active.\n";
    } else {
        if (displayErrorMarginKm < 0.0) {
            cout << "Distance        : " << setprecision(2) << perceivedDistance << " km (Error: N/A)\n";
        } else {
            cout << "Distance        : " << setprecision(2) << perceivedDistance << " km (+/- " << setprecision(3) << displayErrorMarginKm << " km)\n";
        }
    }

    cout << setprecision(2);
    cout << "Transport Mode  : " << modeName << "\n";
    cout << "Traffic Level   : " << trafficName << "\n";
    cout << "Effective Speed : " << effectiveSpeed << " km/h\n";
    cout << "Estimated ETA   : " << hours << " hrs " << minutes << " mins\n";
    cout << "Estimated Cost  : RM " << totalCost << "\n";
    cout << "========================================\n";
    cout << "System Recommendation:\n";

if (trafficChoice == 3 && (modeChoice == 1 || modeChoice == 2)) {
        cout << "Traffic is heavy. Consider taking the bus or using rail transit to avoid congestion.\n";
    } else if (perceivedDistance <= 2.0 && modeChoice != 4) {
        cout << "Short distance route detected. Walking is zero-cost and environmentally friendly.\n";
    } else if (modeChoice == 4 && perceivedDistance > 10.0) {
        cout << "Long walking distance detected. Consider motorized transport for safety and efficiency.\n";
    } else {
        cout << "Selected route and transport mode are optimal for current conditions.\n";
    }
    cout << "========================================\n";

    TripRecord record;
    record.originName = originName;
    record.destinationName = selectedDest.name;
    record.mode = modeName;
    record.distanceKm = perceivedDistance;
    record.errorMarginKm = displayErrorMarginKm;
    record.trafficLevel = trafficName;
    record.compassDir = compassDir;
    record.hours = hours;
    record.minutes = minutes;
    record.cost = totalCost;
    history.push_back(record);
}

void viewDestinations(const vector<Destination>& destinations, double originLat, double originLon) {
    cout << "\n========================================\n";
    cout << "        SAVED DESTINATIONS LIST         \n";
    cout << "========================================\n";
    for (size_t i = 0; i < destinations.size(); ++i) {
        double dist = calculateHaversine(originLat, originLon, destinations[i].lat, destinations[i].lon);
        double bearing = calculateBearing(originLat, originLon, destinations[i].lat, destinations[i].lon);
        cout << (i + 1) << ". " << destinations[i].name 
             << " [Lat: " << setprecision(4) << destinations[i].lat << ", Lon: " << destinations[i].lon << "]\n"
             << setprecision(2) << "   Distance: " << dist << " km | Bearing: " << getCompassDirection(bearing) << "\n";
    }
    cout << "========================================\n";
}

void checkSatelliteSignal(double& errorMargin, int& activeSats, double originLat, double originLon) {
    cout << "\n========================================\n";
    cout << "       GPS SATELLITE SIGNAL STATUS      \n";
    cout << "========================================\n";
    cout << "Enter number of satellites currently in view (0 to 32): ";
    activeSats = getValidInt(0, 32);

    cout << "\nSatellites Locked    : " << activeSats << "\n";
    
    double hdop = 0.0;

    if (activeSats < 4) {
