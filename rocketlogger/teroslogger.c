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
    
    return 0;

    struct termios tty;
    memset (&tty, 0, sizeof tty);

    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error fetching tty config, %i\n",errno);
    }
    /* Set Baud Rate */
    cfsetospeed (&tty, (speed_t)B1200);
    cfsetispeed (&tty, (speed_t)B1200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
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


    /* Flush Port, then applies attributes */
    tcflush( serial_port, TCIFLUSH );
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error configuring tty, %i\n", errno);
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

    printf("Synced\n");
    sprintf(filename, "%s/TEROSoutput-%lu-f%i.csv", logpath, (unsigned long) time(NULL), num);
    // Create child process so we can kill the parent and do setsid() on the child
    // in order to free the controlling terminal
    process_id = fork();
    // Indication of fork() failure
    if (process_id < 0) {
        printf("fork failed!\n");
        // Return failure in exit status
        exit(1);
    }

    // PARENT PROCESS. Need to kill it. 
    if (process_id > 0) {
        // write PID to pid file so we can kill logging later
        pidfile = fopen(pidpath, "wb"); // ?? is there only one logging process per device?
        printf("process_id of child process %d\n", process_id);
        fprintf(pidfile, "%d", process_id);
        fflush(pidfile);
        fclose(pidfile);
        // return success in exit status
        exit(0);
    }

    //unmask the file mode ?? why don't we do this earlier (e.g. line 53 before opening serial_port)?
    umask(0);
    //set new session. this makes the child independent from the controlling terminal
    sid = setsid();
    if (sid < 0) {
        // Return failure
        exit(1);
    }

    close(STDIN_FILENO);  // ?? why do we do this? does it improve performance? 
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    outfile = fopen(filename, "wb");
    //TEROS-12 OUTPUT FORMAT: samples(ADDR/RAW/TMP/EC): 0+1870.34+21.1+0
    char header[] = "timestamp,sensorID,raw_VWC,temp,EC\n";
    fwrite(header, sizeof(char), strlen(header), outfile); // write CSV header

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
