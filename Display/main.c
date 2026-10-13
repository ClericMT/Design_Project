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

// Background colour
#define blk "\033[40m" // Black background (Default colour)
#define red "\033[41m" // Red background
#define grn "\033[42m" // Green background
#define yel "\033[43m" // Yellow background

typedef enum {
    R = 'R', Y = 'Y', G = 'G'
} TrafficSignal;

// Status of lights, sensors for one intersection
typedef struct {
    int ID;                     // Intersection ID (i.e I1, I2)
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
    printf(cls);  // Clear console

    printf(bold);
    printf(invert);
    printf("========================================\n");
    printf("          INTERSECTION I%d\n", status->ID);
    printf("========================================\n");

    printf("\n  TRAFFIC SIGNALS\n");
    printf(regular);
    printf("----------------------------------------\n");
    printf("  %-25s %s\n", "Signal", "State");
    printf("----------------------------------------\n");
    printf("  %-25s %c\n", "NS Straight/Left", status->NSSLight);
    printf("  %-25s %c\n", "NS Right", status->NSRLight);
    printf("  %-25s %c\n", "EW Straight/Left", status->EWSLight);
    printf("  %-25s %c\n", "EW Right", status->EWRLight);
    printf("  %-25s %c\n", "NS Pedestrian", status->NSPedLight);
    printf("  %-25s %c\n", "EW Pedestrian", status->EWPedLight);

    printf(bold);
    printf("\n  SENSORS\n");
    printf("----------------------------------------\n");
    printf("  %-25s %s\n", "Sensor", "Detected");
    printf(regular);
    printf("----------------------------------------\n");
    printf("  %-25s %d\n", "NS Vehicle", status->NS_Vehicle);
    printf("  %-25s %d\n", "EW Vehicle", status->EW_Vehicle);
    printf("  %-25s %d\n", "NS Pedestrian Request", status->NS_Ped);
    printf("  %-25s %d\n", "EW Pedestrian Request", status->EW_Ped);
    printf("========================================\n");
}


int main(void) {
    Status_I I1_status = { 1, R, R, R, R, R, R, 0, 0, 0, 0 };

    printStatus_I(&I1_status);

    Sleep(1000);

    I1_status.NSSLight = G;

    printStatus_I(&I1_status);

    Sleep(1000);

    return 0;
}