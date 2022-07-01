#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/** Baud rate of serial port */
#define BAUD_RATE B1200

uint8_t outbuf[50];
char *logpath = "/home/rocketlogger/soil_battery";
char *pidpath = "/run/teroslogger.pid";

// TODOs:
//  - better format checking before fwrite (some lines in logfile end up f'd up occasionally)
// PS: I have no idea what I'm doing!

int main(int argc, char** argv){
    /* -------------- */
    /* Parse CLI args */
    /* -------------- */

    /* Display help prompt */
    int help = 0;
    /* Verbose logging */
    int verbose = 0;
    /* Path to tty */
    char *tty_path = NULL;
    /* Output file name */
    char * output_file = NULL;

    opterr = 0;

    int c;
    while ((c = getopt (argc, argv, "hvo:")) != -1) {
        switch (c) {
            case 'h':
                help = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'o':
                output_file = optarg;
                break;
            case '?':
                if (optopt == 'o')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    if (help) {
        printf("Usage: %s [-h] [-o file] [tty]\n\n", argv[0]);
        printf("Log TEROS-12 Soil Moisture sensor data read from <tty> to csv file");
        printf("Options:\n");
        printf("  -o <file>\tpath to output log file\n");
        return 0;
    }

    // Remaining options
    int optsrem = argc - optind;
    if (optsrem > 1) {
        printf("Too many arguments!\n");
        return 1;
    }
    else if (optsrem < 1) {
        printf("Too few arguments!\n");
        return 1;
    }
    else {
        tty_path = argv[optind];
    }

    if (verbose) {
        printf("Reading from %s\n", tty_path);
        if (output_file) {
            printf("Logging output to %s\n", output_file);
        }
    }

    /* ---------------- */
    /* Open serial port */
    /* ---------------- */

    // Open USB Port in readonly
    int serial_port = open(tty_path, O_RDONLY);
    // Check for errors
    if (serial_port < 0) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    /* --------------------- */
    /* Configure serial port */
    /* --------------------- */

    struct termios tty;

    // Read existing settings
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("%s\n", strerror(errno));
        return 1;
    }


    // 8-N-1
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;

    // No flow control
    tty.c_cflag &= ~CRTSCTS;

    // Turn on READ and ignore ctrl lines
    tty.c_cflag |= CREAD | CLOCAL;

    // Non-canonical mode (process as characters come in)
    tty.c_lflag &= ~ICANON;

    // Disable eching
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;

    // Disable signal characters
    tty.c_lflag &= ~ISIG;

    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    // Disable special handling of bytes (allows only raw data)
    tty.c_iflag &= ~(IGNBRK | BRKINT |PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    // TODO: Allow infinite wait time for character to account for any amount
    // of delays in reading from the serial port
    // 10 sec read timeout
    tty.c_cc[VTIME] = 100;
    tty.c_cc[VMIN] = 0;

    /* Set Baud Rate */
    cfsetospeed (&tty, BAUD_RATE);
    cfsetispeed (&tty, BAUD_RATE);

    // Apply settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    /* ---------------- */
    /* Log data to file */
    /* ---------------- */

    FILE * outfile = fopen(output_file, "w");

    /* Output Buffer */
    char outbuf[256];

    /* Input buffer */
    char * inbuf = outbuf;

    // Discard output until newline
    do {
        read(serial_port, inbuf, 1);
    }
    while (*inbuf != '\n');

    while (1) {
        // Store time
        int num_char = sprintf(inbuf, "%lu,", (unsigned long) time(NULL));
        inbuf += num_char;

        while(1) {
            // Read single byte of data
            num_char = read(serial_port, inbuf, sizeof(char)); 

            // Check for errors on read
            if (num_char < 0) {
                printf("%s\n", strerror(errno));
            }

            // Convert + to , for csv formatting
            if (*inbuf == '+') {
                *inbuf = ',';
            }

            // Stop reading on line ending
            if (*inbuf == '\n') {
                ++inbuf;
                break;
            }

            ++inbuf;
        }

        // Write to output buffer and flush
        fwrite(outbuf, sizeof(char), inbuf - outbuf, outfile);
        fflush(outfile);
        // Reset pointer position 
        inbuf = outbuf;
    }

    fclose(outfile);

    return 0;
}
