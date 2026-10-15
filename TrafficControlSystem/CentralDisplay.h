#ifndef CENTRALDISPLAY_H
#define CENTRALDISPLAY_H

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

// Basic POSIX terminal helpers
static struct termios original_termios;
static int terminal_configured = 0;

static void restore_terminal(void) {
    if (terminal_configured)
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

static int setup_terminal(void) {
    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &original_termios) != 0) return -1;
    raw = original_termios;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) return -1;
    terminal_configured = 1;
    atexit(restore_terminal);
    return 0;
}

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

// Struct holds all the info for one intersection
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
    int NS_Vehicle;          // north/south vehicle detected
    int EW_Vehicle;          // east/west vehicle detected
    int NS_Ped;              // north/south ped. crossing request
    int EW_Ped;              // east/west ped. crossing request
    int boomGateDown;         // 1 = down, 0 = up
    int boomGateConnected;    // 1 = connected, 0 = disconnected
} IntersectionStatus;

// Thread function
void* display_thread(void* arg);

void updateDisplay(const IntersectionStatus* newStatus);

// Prints all the info for one intersection
void printIntersectionStatus(const IntersectionStatus* status) {
    //printf(CLS); // ubuntu version of clear screen
    //printf("\033[2J\033[H"); // ANSI clear screen for QNX
    printf("\033[H\033[2J");
    fflush(stdout);

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

    printf("  %-25s %s%s%s\n", "NS Vehicle",
        status->NS_Vehicle ? BLDGRN : RESET,
        status->NS_Vehicle ? "Vehicle Detected" : "No Vehicle",
        RESET);

    printf("  %-25s %s%s%s\n", "EW Vehicle",
        status->EW_Vehicle ? BLDGRN : RESET,
        status->EW_Vehicle ? "Vehicle Detected" : "No Vehicle",
        RESET);

    printf("  %-25s %s%s%s\n", "NS Pedestrian Request",
        status->NS_Ped ? BLDGRN : RESET,
        status->NS_Ped ? "Pedestrian Request" : "No Pedestrian",
        RESET);

    printf("  %-25s %s%s%s\n", "EW Pedestrian Request",
        status->EW_Ped ? BLDGRN : RESET,
        status->EW_Ped ? "Pedestrian Request" : "No Pedestrian",
        RESET);
    // Boom gate status
    printf("\n  %sBOOM GATE%s\n", bold, RESET);
    printf("-------------------------------------------------------\n");

    printf("  %-25s %s%s%s\n", "Gate Position",
        status->boomGateDown ? RED : BLDGRN,
        status->boomGateDown ? "DOWN" : "UP",
        RESET);

    printf("  %-25s %s%s%s\n", "Communication",
        status->boomGateConnected ? BLDGRN : RED,
        status->boomGateConnected ? "Connected" : "Disconnected",
        RESET);

    printf("=======================================================\n");
}

#endif
