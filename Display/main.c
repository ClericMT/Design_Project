#include <stdio.h>
#include <windows.h>

#define RESET "\033[0m"
#define cls "\033[1;1H\033[2J" // clear console screen
// Text Font face
#define regular "\033[0m" // regular text
#define bold "\033[1m" // bold text
#define under "\033[4m" // underlined text
#define flash "\033[5m" // flash text (red if not supported)
#define invert "\033[7m" // inverted background and text colour

#define bold "\033[1m"
#define BLK "\033[30m" // Black text
#define RED "\033[31m" // Red text
#define GRN "\033[32m" // Green text
#define YEL "\033[33m" // Yellow text
#define WHT "\033[37m" // White/Gray text
#define BWHT "\033[97m" // Bright White (Default colour)

// Background colour
#define blk "\033[40m" // Black background (Default colour)
#define red "\033[41m" // Red background
#define grn "\033[42m" // Green background
#define yel "\033[43m" // Yellow background

// Set ANSI colour for status based on current state
const char* signalColour(char signal) {
    switch (signal) {
    case 'R': return RED;  // Red
    case 'Y': return YEL;  // Yellow
    case 'G': return GRN;  // Green
    default:  return regular;   // Default
    }
}

// print signal status formatted and coloured
#define PRINT_SIGNAL(label, signal) \
    printf("  %-25s %s%c\033[0m\n", label, signalColour(signal), signal)

// traffic signal state
typedef enum {
    R = 'R', Y = 'Y', G = 'G'
} TrafficSignal;

// Status of lights, sensors for one intersection
typedef struct {
    int ID;                     // Intersection ID (i.e I1, I2)
    int active;                 // online / offline status
    // Traffic Signals
    TrafficSignal NSSLight;
    TrafficSignal NSRLight;
    TrafficSignal EWSLight;
    TrafficSignal EWRLight;
    TrafficSignal NSPedLight;
    TrafficSignal EWPedLight;
    // Sensors
    int NS_Vehicle;            
    int EW_Vehicle;
    int NS_Ped;
    int EW_Ped;
} Status_I;


void printStatus_I(const Status_I* status) {
    //printf(cls);  // Clear console
    system("cls"); // Windows Clear Console
    printf(bold);
    printf("========================================\n");
    printf("          INTERSECTION I%d\n", status->ID);
    printf("========================================\n");
    printf("\n  %-25s %s%s%s\n", "STATUS: ", 
        status->active ? GRN : RED,
        status->active ? "Online" : "Offline",
        regular);
    printf("\n  %sTRAFFIC SIGNALS%s\n", bold, regular);
    printf("----------------------------------------\n");
    printf("  %-25s %s\n", "Signal", "State");
    printf("----------------------------------------\n");

    PRINT_SIGNAL("NS Straight/Left",    status->NSSLight);
    PRINT_SIGNAL("NS Right",            status->NSRLight);
    PRINT_SIGNAL("EW Straight/Left",    status->EWSLight);
    PRINT_SIGNAL("EW Right",            status->EWRLight);
    PRINT_SIGNAL("NS Pedestrian",       status->NSPedLight);
    PRINT_SIGNAL("EW Pedestrian",       status->EWPedLight);

    printf("\n  SENSORS\n");
    printf("----------------------------------------\n");
    printf("  %-25s %s\n", "Sensor", "Detected");
    printf("----------------------------------------\n");
    printf("  %-25s %s\n", "NS Vehicle", status->NS_Vehicle ? "Vehicle Detected" : "No Vehicle");
    printf("  %-25s %s\n", "EW Vehicle", status->EW_Vehicle ? "Vehicle Detected" : "No Vehicle");
    printf("  %-25s %s\n", "NS Pedestrian Request", status->NS_Ped ? "Pedestrian Request" : "No Pedestrian");
    printf("  %-25s %\n", "EW Pedestrian Request", status->EW_Ped ? "Pedestrian Request" : "No Pedestrian");
    printf("========================================\n");
}


int main(void) {
    Status_I I1_status = { 1, 1, R, R, R, R, R, R, 0, 0, 0, 0 };

    printStatus_I(&I1_status);

    Sleep(1000);

    I1_status.NSSLight = G;

    printStatus_I(&I1_status);

    Sleep(1000);

    return 0;
}