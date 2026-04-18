
/***********************************************
 *This function represents the control module of the system. It takes the sensor data stored in the shared memory
 *and performs priority based decision logic to control vehicle parameters such as speed,lane position and alert signals.
 ***********************************************/




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
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

int main() {

    int fd = shm_open("/sensor_shm", O_RDWR, 0666);

    data = mmap(0, sizeof(SensorData),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    int speed = 50;
    int buzzer = 0;
    int lane = 1;

    time_t drowsy_start=0;
    int drowsy_active=0;

    while (1) {

        int limit = data->ota_speed_limit;
        if (limit == 0) limit = 100;

        printf("debug control tilt=%d limit=%d\n",
               data->tilt_angle, limit);

        //  PRIORITY SCHEDULING

        // 1️st priority given to driver fainting
        if (data->fainted) {
            printf("driver fainted, stopping vehicle\n");
            speed = 0;
        }

        // 2️nd priority given to ROLLOVER
        else if (data->tilt_angle > 30) {
            printf("rolling over, stopping the vehicle\n");
            speed = 0;
        }

        // 3️rd priority given to obstacle
        else if (data->obstacle) {
            printf("obstacle, applying brake\n");
            speed -= 20;
            if (speed < 0) speed = 0;
        }

        // 4️th priority given to driver drowsiness
        else if (data->drowsy) {

            if (!drowsy_active) {
                drowsy_start = time(NULL);
                drowsy_active = 1;
            }

            buzzer = 1;
            printf("drowsy\n");

            int t = time(NULL)-drowsy_start;

            if (t > 3) {
                printf("lane shift\n");
                lane = 0;
            }

            if (t > 6) {
                printf("stop\n");
                speed = 0;
            }
        }

        // 5️th prirority to NORMAL + MPU + OTA
        else {

            buzzer = 0;
            lane = 1;
            drowsy_active = 0;

            // downhill
            if (data->tilt_angle < -10) {
                printf(" downhill : accelerating\n");
                speed += 10;
            }

            if (speed < limit)
                speed += 5;

            printf("normal\n");
        }


        if (data->temp_high) {
            printf(" temp reducing\n");
            speed -= 10;
            if (speed < 0) speed = 0;
        }

        printf("control : speed=%d lane=%d buzzer=%d\n",
               speed, lane, buzzer);

        sleep(1);
    }
}
