### serial_stream.c

Daemon to collect TEROS measurements from Arduino and log them to a file. 

Build instructions:
1. `gcc teroslogger.c -o teroslogger` #compile...should probably just change name to teroslogger.c instead of renaming it
2. `mv teroslogger /bin` #move executable to bin
3. `chown root /bin/teroslogger && chgrp root /bin/teroslogger` #make root the owner
4. `chmod u+s /bin/teroslogger` #set suid bit to make the daemon runnable

Test by running `teroslogger -t /dev/tty<ARDUINOUSB>`, it should create a log file wth sensor readings.
NOTE: for regular operation, it is recommended to use the soil_battery service to start logging upon boot instead of starting the teroslogger process by hand

### soil_battery.service

systemd service file, allows us to start power and TEROS logging upon boot. Can start/stop service manually like so:

`sudo systemctl <start|stop> soil_battery.service`

### soil_battery_start.sh and soil_battery_stop.sh

These are the scripts that soil_battery.service hooks into. Because of permissions, they should generally not be run manually, but instead invoked through systemctl (see above).
