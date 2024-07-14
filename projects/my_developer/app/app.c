#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

int main(int argc, char **argv) {
    char *app_name = argv[0];
    char *dev_name = "/dev/ttyS1";
    int fd = -1;
    unsigned char read_buffer[6]; // Buffer to store exactly 5 characters + null terminator
    ssize_t bytes_read;
    struct termios options;
    time_t start_time, current_time;
    const char *target = "hello";
    const char *response = "hello_back";
    int runtime_seconds = 100; // Change runtime to 100 seconds

    /*
     * Open the ttyS1 device for reading and writing
     */
    if ((fd = open(dev_name, O_RDWR | O_NOCTTY)) < 0) {
        fprintf(stderr, "%s: unable to open %s: %s\n",
                app_name, dev_name, strerror(errno));
        return -1;
    }

    /*
     * Flush input buffer to prevent characters from being displayed
     */
    tcflush(fd, TCIFLUSH);

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
     * Disable echo, canonical input processing, and signal characters
     */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /*
     * Set minimum characters for non-canonical read to 5
     */
    options.c_cc[VMIN] = 5;

    /*
     * Set timeout for non-canonical read (10 seconds)
     */
    options.c_cc[VTIME] = 100; // 100 * 0.1 seconds = 10 seconds timeout

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
         * Check if runtime_seconds seconds have passed
         */
        if (difftime(current_time, start_time) >= runtime_seconds) {
            break;
        }

        /*
         * Read exactly 5 characters from the ttyS1 device
         */
        bytes_read = read(fd, read_buffer, 5);
        if (bytes_read > 0) {
            read_buffer[bytes_read] = '\0'; // Null-terminate the string

            printf("Received: %s\n", read_buffer); // Log the received characters

            if (strcmp((char *)read_buffer, target) == 0) {
                // Send "hello_back" back after receiving the entire string "hello"
                if (write(fd, response, strlen(response)) < 0) {
                    perror("write: Unable to send data to UART");
                } else {
                    printf("Sent: %s\n", response); // Log the sent string
                }
            }
        } else if (bytes_read < 0) {
            fprintf(stderr, "%s: unable to read from %s: %s\n",
                    app_name, dev_name, strerror(errno));
            close(fd);
            return -1;
        }

        /*
         * Clear read_buffer
         */
        memset(read_buffer, 0, sizeof(read_buffer));

    }

    /*
     * Close the file descriptor at the end
     */
    close(fd);

    /*
     * End of program
     */
    return 0;
}
