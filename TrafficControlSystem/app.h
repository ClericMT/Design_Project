#ifndef APP_H
#define APP_H

#include <sys/dispatch.h>

#define BUF_SIZE 300

typedef union {	  			// This replaced the standard:  union sigval
	union{
		_Uint32t sival_int;
		void *sival_ptr;	// This has a different size in 32-bit and 64-bit systems
	};
	_Uint32t dummy[4]; 		// Hence, we need this dummy variable to create space
}_mysigval;

typedef struct _Mypulse {   // This replaced the standard:  typedef struct _pulse msg_header_t;
   _Uint16t type;
   _Uint16t subtype;
   _Int8t code;
   _Uint8t zero[3];         // Same padding that is used in standard _pulse struct
   _mysigval value;
   _Uint8t zero2[2];		// Extra padding to ensure alignment access.
   _Int32t scoid;
} msg_header_t;


// This is where pedestrian/vehicle detection data is stored
typedef struct {
   int NS_Vehicle;          // north/south vehicle detected
   int EW_Vehicle;          // east/west vehicle detected
   int NS_Ped;              // north/south ped. crossing request
   int EW_Ped;              // east/west ped. crossing request
} SensorData;

typedef struct {
    msg_header_t hdr;
    int ClientID;
    SensorData data;
} SensorMessage;

// Data received from CentralController
typedef struct {
    int mode;
    int boomGateDown;
    int boomGateConnected;
} CentralCommand;

typedef struct {
    msg_header_t hdr;
    int ClientID;
    CentralCommand data;
} CentralCommandMessage;

// traffic signal state
typedef enum {
    R = 'R', Y = 'Y', G = 'G'
} TrafficSignal;

// Traffic Signals for local intersection
typedef struct {
	TrafficSignal NSSLight;
	TrafficSignal NSRLight;
	TrafficSignal EWSLight;
	TrafficSignal EWRLight;
	TrafficSignal NSPedLight;
	TrafficSignal EWPedLight;
} TrafficSignals;

// Displays all info about intersection
typedef struct {
    int ID;                     // Intersection ID (i.e I1, I2)
    int active;					// Intersection online / offline status
    CentralCommand centralCommand;
    TrafficSignals trafficSignals;
    SensorData sensorData;
} IntersectionStatus;

//
typedef struct {
	msg_header_t hdr;
    int ID;                     // Intersection ID (i.e I1, I2)
    int active;					// Intersection online / offline status
    IntersectionStatus intersectionStatus;
} IntersectionStatusMessage;

// reply message
typedef struct {
   msg_header_t hdr;   // Custom header
   char buf[BUF_SIZE]; // Message to send back to send back to other thread
} my_reply;

// Using a union to receive different message types (sensor, central, pulse)
typedef union {
    msg_header_t hdr;
    struct _pulse pulse;
    SensorMessage sensor;
    CentralCommandMessage central;
} IncomingMessage;

#endif
