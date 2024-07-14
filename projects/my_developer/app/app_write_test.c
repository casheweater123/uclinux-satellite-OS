#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

int main(int argc, char **argv)
{
    char *app_name = argv[0];
    char *dev_name = "/dev/ttyS1";// writing to UART_1
    int ret = -1;
    int fd = -1;
    unsigned char hex_data[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello" in ASCII
    ssize_t bytes_written;
    size_t data_len = sizeof(hex_data);
    size_t i; 
    struct termios options;

    /*
     * Open the ttyS1 device for writing only
     */
    if ((fd = open(dev_name, O_WRONLY)) < 0) {
        fprintf(stderr, "%s: unable to open %s: %s\n",
                app_name, dev_name, strerror(errno));
        return -1;
    }

    /*
     * Get the current port settings
     */
    tcgetattr(fd, &options);

    /*
     * Set the baud rates to 115200. The default baud rate is 9600.
     */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    /*
     * Enable the receiver and set local mode
     */
    options.c_cflag |= (CLOCAL | CREAD);

    /*
     * Set the new options for the port
     */
    tcsetattr(fd, TCSANOW, &options);

    while (1) {
        /*
         * Write the hex data to the ttyS1 device
         */
        bytes_written = write(fd, hex_data, data_len);
        if (bytes_written < 0) {
            fprintf(stderr, "%s: unable to write to %s: %s\n",
                    app_name, dev_name, strerror(errno));
            close(fd);
            return -1;
        }

        /*
         * Print the written hex data to stdout
         */
        fprintf(stdout, "Written to %s: ", dev_name);
        for (i = 0; i < data_len; i++) {
            fprintf(stdout, "%02X ", hex_data[i]);
        }
        fprintf(stdout, "\n");

        fflush(stdout); // output is printed immediately
        sleep(1); // Delay of 1 second
    }

    /*
     * Close the file descriptor (unreachable code, but good practice to include)
     */
    close(fd);
    return 0;
}
