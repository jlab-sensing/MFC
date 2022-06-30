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

uint8_t inbuf[1];
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

    char *tty_path = NULL;
    int index;
    int c;
    int num = 0;

    /* Output file name */
    char * output_file = NULL;

    opterr = 0;

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

    /*
    tty.c_cflag = CS8;     // 8-bit chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 100;            // 10 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= 0;						// ?? does this actually do anything?
    tty.c_cflag &= ~CSTOPB;					// use only one stop bit
    tty.c_cflag &= ~CRTSCTS;				// ?? not in POSIX... is this necessary?
    */

    // Apply settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    
    while (1) {
        char read_buf[256];
        memset(read_buf, '\0', sizeof(read_buf));
        
        int num_bytes = read(serial_port, read_buf, sizeof(read_buf));

        if (num_bytes < 0) {
            printf("%s\n", strerror(errno));
        }

        printf("Read %i bytes. Recv: %s\n", num_bytes, read_buf);
    }


    FILE* outfile;
    FILE* pidfile;
    char filename[100];
    pid_t process_id = 0;
    pid_t sid = 0;

    /* keep reading until start of line */
    do {
        read(serial_port, inbuf, sizeof inbuf);
    } while (inbuf[0] != '\n');

    // finally start reading and logging measurements
    int marker_state = 0;
    char logstr[80];
    //memset(outbuf, 0, sizeof(outbuf));
    //memset(logstr, 0, sizeof(logstr));
    // do this forever??
    while (1) {
        read(serial_port, inbuf, sizeof inbuf); // read in a byte
        if (inbuf[0] == '\n') { // if we reached the end of a measurement
            if (marker_state > 0) {
                // if this wasn't an empty line, toss a null terminator to the output string,
                // slap a time stamp on our measurement, and write that bad boi to file
                outbuf[marker_state] = 0; // this should let us avoid having to memset outbuf every time
                sprintf(logstr, "%lu,%s\n", (unsigned long) time(NULL), outbuf);
                fwrite(logstr, sizeof(char), strlen(logstr), outfile); 
                fflush(outfile);

                // then clear our buffers, reset our writing index, and wait a sec before reading again
                //memset(outbuf, 0, sizeof(outbuf));
                //memset(logstr, 0, sizeof(logstr));
                marker_state = 0;
                sleep(1);
            }
        } else if (marker_state > sizeof(outbuf)){ //oops, overflow
                                                   //printf("marker state %i\n", marker_state);
                                                   // ?? should we reject the entire measurement and jump to the next newline? right now this just
                                                   // ?? truncates the measurement and returns the latter piece, which may be junk?
            marker_state = 0;
            //memset(outbuf, 0, sizeof(outbuf)); // this is unnecessary now that we are null terminating outbuf
        } else {
            // we just write the new byte into our outbuf, replacing '+' with ',' for csv, and increment 
            // the writing index
            if (inbuf[0] == '+') {
                outbuf[marker_state] = ',';
            } else {
                outbuf[marker_state] = inbuf[0];
            }
            marker_state += 1;
        }
    }

    fclose(outfile); // ?? is this ever run?
    return 0;
}
