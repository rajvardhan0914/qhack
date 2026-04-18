
/***********************************************
 *This function consists of the sensor data which is simulated or entered manually by the user so that the state detection
 *is further calculated and sent to the shared memory to be accessed by the control process. it is simulating obstacles
 *temperature of the battery,the driver state manually input by the user.
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

typedef struct {
    int drowsy;
    int fainted;
    int obstacle;
    int temp_high;

    int tilt_angle;
    int ota_speed_limit;

    time_t event_time;
} SensorData;

SensorData *data;

/*
 *Here the obstacle is auto simulated where the obstacle is generated and stays for 3 secs and then disappears for
 *8 seconds and reappears.
 */
void* obstacle_thread(void* arg) {
    while (1) {
        sleep(8);
        data->obstacle = 1;
        data->event_time = time(NULL);
        printf("auto obstacle\n");

        sleep(3);
        data->obstacle = 0;
        printf("cleared\n");
        printf("Obstacle thread running\n");
    }
}

/*
 *Here the temp is auto simulated where the temperature is generated randomly and following the given thresholds the
 *the state is determined.
 */
void* temp_thread(void* arg) {
    int temp = 30;
    while (1) {
        temp += rand() % 5;

        if (temp > 60) {
            data->temp_high = 1;
            printf("overheat\n");
        } else {
            data->temp_high = 0;
        }

        sleep(2);
    }
}

/*
 *Here the sensor input is manually entered by the user for driver state , temperature status, obstacl, value of mpu for
 *hill slope assist and OTA update.
 */
void* input_thread(void* arg) {
    char line[100];

    while (1) {
        printf("\nCommand:\n");
        fgets(line, sizeof(line), stdin);

        if (strncmp(line,"sleep",5)==0) {
            data->drowsy = 1;
            data->event_time = time(NULL);
        }

        else if (strncmp(line,"wake",4)==0)
            data->drowsy = 0;

        else if (strncmp(line,"faint",5)==0) {
            data->fainted = 1;
            data->event_time = time(NULL);
        }

        else if (strncmp(line,"recover",7)==0)
            data->fainted = 0;

        else if (strncmp(line,"obs",3)==0) {
            data->obstacle = 1;
            data->event_time = time(NULL);
        }

        else if (strncmp(line,"clear",5)==0)
            data->obstacle = 0;

        else if (strncmp(line,"temp",4)==0)
            data->temp_high = 1;

        else if (strncmp(line,"normal",6)==0)
            data->temp_high = 0;

        //  MPU
        else if (strncmp(line,"mpu",3)==0) {
            int angle;
            sscanf(line,"mpu %d",&angle);
            data->tilt_angle = angle;
            data->event_time = time(NULL);
            printf("Tilt=%d\n",angle);
        }

        //  OTA
        else if (strncmp(line,"update",6)==0) {
            data->ota_speed_limit = 60;
            printf("OTA limit=60\n");
        }

        printf("debug sensor : tilt=%d limit=%d\n",
               data->tilt_angle, data->ota_speed_limit);
    }
}

/*
*this function initializes the shared memory used for inter-process communication between the sensor and
 * control modules. It sets up initial sensor values,creates multiple threads to simulate sensor behavior,
 * and continuously monitors the shared data.
 */


 int main() {

    shm_unlink("/sensor_shm");

    int fd = shm_open("/sensor_shm", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SensorData));

    data = mmap(0, sizeof(SensorData),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // initialising the values
    data->drowsy = 0;
    data->fainted = 0;
    data->obstacle = 0;
    data->temp_high = 0;
    data->tilt_angle = 0;
    data->ota_speed_limit = 100;
    data->event_time = time(NULL);

    pthread_t t1,t2,t3;

    pthread_create(&t1,NULL,obstacle_thread,NULL);
    pthread_create(&t2,NULL,temp_thread,NULL);
    pthread_create(&t3,NULL,input_thread,NULL);

    while (1) {
        printf("[sensor] tilt=%d limit=%d\n",
               data->tilt_angle, data->ota_speed_limit);
        sleep(1);
    }
}
