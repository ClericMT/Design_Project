#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <sys/iofunc.h>

#include "CentralDisplay.h"

// create instance of GUI_data
IntersectionStatus I1_status = {
		1, 1, FIXED_MODE,    // ID, active, mode
		R, R, R, R, R, R,   // Lights
		0, 0, 0, 0,         // Sensors
		0, 1              // Boom gate down, communication connected
};

// Synchronisation
pthread_mutex_t display_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t display_cond = PTHREAD_COND_INITIALIZER;
int data_ready = 0;

// Display thread function
void* display_thread(void* arg) {

    while (1) {

        pthread_mutex_lock(&display_mutex); // lock the d

        // Wait until new data is ready
        while (!data_ready) {
            pthread_cond_wait(&display_cond, &display_mutex);
        }

        // Copy of intersection status so mutex doesn't remain locked while printing
        IntersectionStatus I1_status_copy = I1_status;

        data_ready = 0;

        pthread_mutex_unlock(&display_mutex);

        // Update display outside of mutex so other threads can print
        printIntersectionStatus(&I1_status_copy);
        fflush(stdout);
    }

    return NULL;
}

// Updates GUI
void updateDisplay(const IntersectionStatus* status) {
	pthread_mutex_lock(&display_mutex); // Protect shared variables

	I1_status = *status; // Update status

	data_ready = 1;
	pthread_cond_signal(&display_cond);

	pthread_mutex_unlock(&display_mutex);
}


int main(void) {

    if (setup_terminal() != 0) {
        perror("Cannot configure terminal");
        return 1;
    }

    pthread_t display_tid;

    // Start display thread
    pthread_create(&display_tid, NULL, display_thread, NULL);

    sleep(2);

    ///*** Example Update Display ***///
    IntersectionStatus newStatus = I1_status;

    newStatus.EW_Vehicle = 1;

    updateDisplay(&newStatus); // Update display

    sleep(2);

    newStatus.EW_Vehicle = 0;


    updateDisplay(&newStatus); // Update display

    // Keep process running
    pthread_join(display_tid, NULL);


    restore_terminal();
    terminal_configured = 0;
    printf("\nExiting system...\n");
    return 0;
}
