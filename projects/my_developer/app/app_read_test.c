#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

int main(int argc, char **argv)
{
    char *app_name = argv[0];
    char *dev_name = "/dev/ttyS1";
    int fd = -1;
    unsigned char read_buffer[256]; // Buffer to store read data
    ssize_t bytes_read;
    struct termios options;
    time_t start_time, current_time;
    char response = 'B'; // Response character

    /*
     * Open the ttyS1 device for reading and writing
     */
    if ((fd = open(dev_name, O_RDWR | O_NOCTTY)) < 0) {
        fprintf(stderr, "%s: unable to open %s: %s\n",
                app_name, dev_name, strerror(errno));
        return -1;
    }

    /*
     * Get the current options for the port
     */
    tcgetattr(fd, &options);

    /*
     * Set the baud rates to 115200
     */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    /*
     * Enable the receiver and set local mode
     */
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;     // 8 data bits
    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit

    /*
     * Set the new options for the port
     */
    tcsetattr(fd, TCSANOW, &options);

    /*
     * Get the start time
     */
    start_time = time(NULL);

    while (1) {
        /*
         * Get the current time
         */
        current_time = time(NULL);

        /*
         * Check if 10 seconds have passed
         */
        if (difftime(current_time, start_time) >= 10) {
            break;
        }

        /*
         * Read the data from the ttyS1 device
         */
        bytes_read = read(fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read > 0) {
            /*
             * Null-terminate the read buffer and print it
             */
            read_buffer[bytes_read] = '\0';
            fprintf(stdout, "Read from %s: %s\n", dev_name, read_buffer);
            fflush(stdout); // Ensure the output is printed immediately

            /*
             * Check if the received data is 'A' and respond with 'B'
             */
            if (read_buffer[0] == 'A') {
                ssize_t bytes_written = write(fd, &response, 1);
                if (bytes_written < 0) {
                    fprintf(stderr, "%s: unable to write to %s: %s\n",
                            app_name, dev_name, strerror(errno));
                    close(fd);
                    return -1;
                }
            }
        } else if (bytes_read < 0) {
            fprintf(stderr, "%s: unable to read from %s: %s\n",
                    app_name, dev_name, strerror(errno));
            close(fd);
            return -1;
        }

        usleep(100000); // Sleep for 100ms to avoid busy-waiting
    }

    /*
     * Close the file descriptor
     */
    close(fd);
    return 0;
}
