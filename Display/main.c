#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>

// ANSI font definitions

#define RESET "\033[0m"
#define cls "\033[1;1H\033[2J" // clear console screen
// Text Font face
#define regular "\033[0m" // regular text
#define bold "\033[1m" // bold text
#define under "\033[4m" // underlined text
#define flash "\033[5m" // flash text (red if not supported)
#define invert "\033[7m" // inverted background and text colour
// Font colour
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

// Set ANSI font style for light output based on light state
const char* signalColour(char signal) {
    switch (signal) {
    case 'R': return BLDRED;  // Red
    case 'Y': return BLDYEL;  // Yellow
    case 'G': return BLDGRN;  // Green
    default:  return regular;   // Default
    }
}

// print function for signal status formatted and ANSI styled
#define PRINT_SIGNAL(label, signal) \
    printf("  %-25s %s%c\033[0m\n", label, signalColour(signal), signal)

// traffic signal state
typedef enum {
    R = 'R', Y = 'Y', G = 'G'
} TrafficSignal;

// Traffic operation mode
typedef enum {
    FIXED_MODE,
    SENSOR_MODE,
    OVERRIDE_MODE
} Mode;

// Holds all the info for one intersection
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
} IntersectionStatus;

// Prints all the info for one intersection
void printIntersectionStatus(const IntersectionStatus* status) {
    //printf(CLS); // ubuntu version of clear screen
    system("cls"); // windows version

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

void sensorThread(IntersectionStatus* status);

int state = 0; // current state (TODO: maybe should be in main passed by ref)

void State0(IntersectionStatus *status) { // East-West Right-Turn = Green
    // update status
    status->NSRLight    = R;
    status->NSSLight    = R;
    status->EWSLight    = R;
    status->EWRLight    = G;
    status->NSPedLight  = R;
    status->EWPedLight  = R;

    printIntersectionStatus(status);      // update status display

    Sleep(2000);                // delay 2 seconds
 
    state = 1;
}

void State1(IntersectionStatus* status) { // East-West Right-Turn = Yellow
    status->NSRLight    = R;
    status->NSSLight    = R;
    status->EWSLight    = R;
    status->EWRLight    = Y;
    status->NSPedLight  = R;
    status->EWPedLight  = R;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 2;
}

void State2(IntersectionStatus* status) { // All lights = Red
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 3;
}

void State3(IntersectionStatus* status) { // East-West Straight-Left/Pedestrian = Green
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = G;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = G;

    // reset east/west sensors as vehichles/ped. are being serviced
    status->EW_Vehicle = 0;
    status->EW_Ped = 0;

    printIntersectionStatus(status);

    // Only wait if in sensor operation mode or NS vehicle / pedestrian detected
    if (status->mode != FIXED_MODE && !(status->NS_Vehicle) && !(status->NS_Ped) && status->active) {
        sensorThread(status); // wait
    }
    Sleep(2000);
    state = 4;
}

void State4(IntersectionStatus* status) { // East-West Straight-Left/Pedestrian = Yellow
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = Y;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = Y;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 5;
}

void State5(IntersectionStatus* status) { // All lights = Red
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 6;
}

void State6(IntersectionStatus* status) { // North-South Right-Turn = Green
    status->NSRLight = G;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printIntersectionStatus(status);
    Sleep(2000);
    state = 7;
}

void State7(IntersectionStatus* status) { // North-South Right-Turn = Yellow
    status->NSRLight = Y;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 8;
}

void State8(IntersectionStatus* status) { // All lights = Red
    status->NSRLight = R;
    status->NSSLight = R;
    status->EWSLight = R;
    status->EWRLight = R;
    status->NSPedLight = R;
    status->EWPedLight = R;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 9;
}

void State9(IntersectionStatus* status) { // North-South Straight-Left/Pedestrian = Green
    status->NSRLight    = R;
    status->NSSLight    = G;
    status->EWSLight    = R;
    status->EWRLight    = R;
    status->NSPedLight  = G;
    status->EWPedLight  = R;

    // reset north/south sensors as vehicles/ped. have been serviced
    status->NS_Vehicle = 0;
    status->NS_Ped = 0;

    printIntersectionStatus(status);

    // Only wait if in sensor operation mode or EW vehicle / pedestrian detected
    // If no communication (status inactive) then operate in fixed mode
    if (status->mode != FIXED_MODE && !(status->EW_Vehicle) && !(status->EW_Ped) && status->active) {
        sensorThread(status); // wait
    }
    Sleep(2000);

    state = 10;
}

void State10(IntersectionStatus* status) { // North-South Straight-Left/Pedestrian = Yellow
    status->NSRLight    = R;
    status->NSSLight    = Y;
    status->EWSLight    = R; 
    status->EWRLight    = R;
    status->NSPedLight  = Y; 
    status->EWPedLight  = R;
    
    printIntersectionStatus(status);
    Sleep(1000);
    state = 11;
}

void State11(IntersectionStatus* status) { // All lights = Red
    status->NSRLight    = R;
    status->NSSLight    = R;
    status->EWSLight    = R;
    status->EWRLight    = R;
    status->NSPedLight  = R;
    status->EWPedLight  = R;

    printIntersectionStatus(status);
    Sleep(1000);
    state = 0;
}

// TODO: timer expires
void sensorThread(IntersectionStatus* status) {
    int waiting = 1;
    char key;
    while (waiting) {
        if (_kbhit()) {
            key = _getch();

            switch (key) {
            case '1':
                status->mode = FIXED_MODE;
                break;
            case '2':
                status->mode = SENSOR_MODE;
                break;
            case '3':
                status->mode = OVERRIDE_MODE;
                break;
            case 'n':                                               // north/south vehicle detected
                if (status->NSSLight != G) status->NS_Vehicle = 1;  // Assume NS vehicles serviced instantly if NSSLight = G
                printIntersectionStatus(status);
                if (status->EWSLight == G) waiting = 0;             // Car approaches east/west -> goto next state
                break;
            case 'e':                                               // east/west vehicle detected
                if (status->EWSLight != G) status->EW_Vehicle = 1;  // Assume EW vehicles serviced instantly if EWSLight = G
                printIntersectionStatus(status);
                if (status->NSSLight == G) waiting = 0;             // Car approaches north/south -> goto next state
                break;
            case 'N':                                               // north/south ped. detected
                if (status->NSSLight != G) status->NS_Ped = 1;      // Assume NS peds. serviced instantly if NSSLight = G
                printIntersectionStatus(status);
                if (status->EWSLight == G) waiting = 0;             // Ped. approaches east/west -> goto next state
                break;
            case 'E':                                               // east/west ped. detected
                if (status->EWSLight != G) status->EW_Ped = 1;      // Assume EW peds. serviced instantly if EWSLight = G
                printIntersectionStatus(status);
                if (status->NSSLight == G) waiting = 0;             // Ped. approaches north/south -> goto next state
                break;
            case 'a':                                               // Toggle intersection active      
                status->active = !(status->active);
                waiting = 0;
                break;
            case 'q':                                               // shutdown
                status->active = !(status->active);
                waiting = 0;
                break;
            }
        }
    }
}

// Step through state machine
void singleStepState(IntersectionStatus* status) { 
    int IO = 1;
    char key;
    while (IO) {
        if (_kbhit()) {
            key = _getch();

            switch (key) {
            case '1':
                status->mode = FIXED_MODE;
                break;
            case '2':
                status->mode = SENSOR_MODE;
                break;
            case '3':
                status->mode = OVERRIDE_MODE;
                break;
            case 'n':
                status->NS_Vehicle = 1;
                break;
            case 'e':
                status->EW_Vehicle = 1;
                break;
            case 'N':
                status->NS_Ped = 1;
                break;
            case 'E':
                status->EW_Ped = 1;
                break;
            case 'a':
                status->active = !(status->active);
                break;
            case 'q':
                IO = 0;
                break;
            }
        }
        Sleep(50);
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
    // Init intersection 1
    IntersectionStatus I1_status = { 
        1, 1, 0,            // ID, active, mode
        R, R, R, R, R, R,   // Light states
        0, 0, 0, 0          // Sensor states
    };

    // Print status once
    printIntersectionStatus(&I1_status);
    char key;
    int running = 1;
    singleStepState(&I1_status);
    

    printf("\nExiting system...\n");
    Sleep(2000);

    return 0;
}