#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#define RESET "\033[0m"
#define cls "\033[1;1H\033[2J" // clear console screen
// Text Font face
#define regular "\033[0m" // regular text
#define bold "\033[1m" // bold text
#define under "\033[4m" // underlined text
#define flash "\033[5m" // flash text (red if not supported)
#define invert "\033[7m" // inverted background and text colour

#define BLK "\033[30m" // Black text
#define RED "\033[31m" // Red text
#define BLDRED "\033[1;31m" // Bold Red text
#define GRN "\033[32m" // Green text
#define BLDGRN "\033[1;32m" // Bold Red text
#define YEL "\033[33m" // Yellow text
#define BLDYEL "\033[1;33m" // Bold Red text
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
    case 'R': return BLDRED;  // Red
    case 'Y': return BLDYEL;  // Yellow
    case 'G': return BLDGRN;  // Green
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

// Traffic mode
typedef enum {
    FIXED_MODE,
    SENSOR_MODE,
    OVERRIDE_MODE
} Mode;

// Status of lights, sensors for one intersection
typedef struct {
    int ID;                     // Intersection ID (i.e I1, I2)
    int active;                 // online / offline status
    Mode mode;
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


/*void printStatus_I(const Status_I* status) {
    //printf(cls);  // Clear console
    system("cls"); // Windows Clear Console
    printf(bold);
    // printf(flash); // not sure if this works in linux
    printf("========================================\n");
    printf("          INTERSECTION I%d\n", status->ID);
    printf("========================================\n");
    printf("\n  %-25s %s%s%s\n", "STATUS: ", 
        status->active ? GRN : RED,
        status->active ? "Online" : "Offline",
        regular);
    printf("  %-25s %s\n", "MODE",
        status->mode == 0 ? "FIXED" :
        status->mode == 1 ? "SENSOR DRIVEN" :
        "MANUAL OVERRIDE");
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
    printf("  %-25s %s\n", "EW Pedestrian Request", status->EW_Ped ? "Pedestrian Request" : "No Pedestrian");
    printf("========================================\n");
}*/



void printStatus_I(const Status_I* status) {
    system("cls");

    printf(bold);
    printf("=======================================================\n");
    printf("                    INTERSECTION I%d\n", status->ID);
    printf("=======================================================\n");
    printf(RESET);

    printf("\n  %-25s %s%s%s\n", "STATUS:",
        status->active ? GRN : RED,
        status->active ? "Online" : "Offline",
        RESET);

    printf("  %-25s %s\n", "MODE:",
        status->mode == FIXED_MODE ? "FIXED" :
        status->mode == SENSOR_MODE ? "SENSOR DRIVEN" :
        "MANUAL OVERRIDE");

    printf("\n  %sTRAFFIC SIGNALS%s\n", bold, RESET);
    printf("-------------------------------------------------------\n");
    printf("  %-26s %s\n", "NORTH / SOUTH", "EAST / WEST");
    printf("-------------------------------------------------------\n");

    // Straight / Left signals
    printf("  %-15s %s%c%s", "Straight/Left:",
        signalColour(status->NSSLight), status->NSSLight, RESET);
    printf("         %-15s %s%c%s\n", "Straight/Left:",
        signalColour(status->EWSLight), status->EWSLight, RESET);

    // Right turn signals
    printf("  %-15s %s%c%s", "Right Turn:",
        signalColour(status->NSRLight), status->NSRLight, RESET);
    printf("         %-15s %s%c%s\n", "Right Turn:",
        signalColour(status->EWRLight), status->EWRLight, RESET);

    // Pedestrian signals
    printf("  %-15s %s%c%s", "Pedestrian:",
        signalColour(status->NSPedLight), status->NSPedLight, RESET);
    printf("         %-15s %s%c%s\n", "Pedestrian:",
        signalColour(status->EWPedLight), status->EWPedLight, RESET);

    printf("-------------------------------------------------------\n");

    printf("\n  %sSENSORS%s\n", bold, RESET);
    printf("-------------------------------------------------------\n");
    printf("  %-25s %s\n", "Sensor", "Detected");
    printf("-------------------------------------------------------\n");

    printf("  %-25s %s\n", "NS Vehicle",
        status->NS_Vehicle ? "Vehicle Detected" : "No Vehicle");
    printf("  %-25s %s\n", "EW Vehicle",
        status->EW_Vehicle ? "Vehicle Detected" : "No Vehicle");
    printf("  %-25s %s\n", "NS Pedestrian Request",
        status->NS_Ped ? "Pedestrian Request" : "No Pedestrian");
    printf("  %-25s %s\n", "EW Pedestrian Request",
        status->EW_Ped ? "Pedestrian Request" : "No Pedestrian");

    printf("=======================================================\n");
}



int state = 0;

void State0(Status_I *status) { // EWRG-NSxR
    status->NSRLight    = R;
    status->NSSLight    = R;
    status->EWSLight    = R;
    status->EWRLight    = G;
    status->NSPedLight  = R;
    status->EWPedLight  = R;

    printStatus_I(status);
    Sleep(2000);
    state = 1;
}

void State1(Status_I* status) { // EWRY-NSxR
    status->NSRLight    = R;
    status->NSSLight    = R;
    status->EWSLight    = R;
    status->EWRLight    = Y;
    status->NSPedLight  = R;
    status->EWPedLight  = R;

    printStatus_I(status);
    Sleep(1000);
    state = 2;
}

void State2(Status_I* status) { // EWxR-NSxR
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(1000);
    state = 3;
}

void State3(Status_I* status) { // EWSG-NSxR
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = G;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = G;

    printStatus_I(status);
    Sleep(2000);
    state = 4;
}

void State4(Status_I* status) { // EWSY-NSxR
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = Y;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = Y;

    printStatus_I(status);
    Sleep(1000);
    state = 5;
}

void State5(Status_I* status) { // EWxR-NSxR
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(1000);
    state = 6;
}

void State6(Status_I* status) { // EWxR-NSRG
    status->NSRLight = G;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(2000);
    state = 7;
}

void State7(Status_I* status) { // EWxR-NSRY
    status->NSRLight = Y;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(1000);
    state = 8;
}

void State8(Status_I* status) { // EWxR-NSRR
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(1000);
    state = 9;
}

void State9(Status_I* status) { // EWxR-NSSG
    status->NSRLight = R;
    status->NSSLight = G;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = G;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(2000);
    state = 10;
}

void State10(Status_I* status) { // EWxR-NSSY
    status->NSRLight    = R; status->EWSLight   = R;
    status->NSSLight    = Y; status->EWRLight   = R;
    status->NSPedLight  = Y; status->EWPedLight = R;
    

    printStatus_I(status);
    Sleep(1000);
    state = 11;
}

void State11(Status_I* status) { // EWxR-NSSR
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printStatus_I(status);
    Sleep(1000);
    state = 0;
}

void singleStepState(Status_I* status) { 
    int IO = 1;
    while (IO) {
        switch (state) {
        case 0:
            State0(status);
            break;
        case 1:
            State1(status);
            break;
        case 2:
            State2(status);
            break;
        case 3:
            State3(status);
            break;
        case 4:
            State4(status);
            break;
        case 5:
            State5(status);
            break;
        case 6:
            State6(status);
            break;
        case 7:
            State7(status);
            break;
        case 8:
            State8(status);
            break;
        case 9:
            State9(status);
            break;
        case 10:
            State10(status);
            break;
        case 11:
            State11(status);
            break;
        }
    }
}

int main(void) {
    Status_I I1_status = { 
        1, 1, 0, 
        R, R, R, R, R, R, 
        0, 0, 0, 0 
    };

    printStatus_I(&I1_status);

    singleStepState(&I1_status);

    return 0;
}