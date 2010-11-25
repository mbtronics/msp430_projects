import time, serial, os
from datetime import datetime

Timeout = 5

Port = serial.Serial()

def OpenPort():
    global Port
    Port.close()

    PortOpen = False
    while PortOpen == False:
        try:
            Port = serial.Serial('/dev/ttyACM0', 2400, timeout=Timeout)
            PortOpen = True
            print "Serial port opened"
        except:
            print "Serial port could not be opened, trying again in 1 sec"
            time.sleep(1)

OpenPort()

while 1:
    Time = time.time()
    Line = Port.readline()

    if Line != "":
        Line = Line.strip()
        Now = datetime.now()
        print str(Now) + " : " + Line

    else:
        if (time.time() - Time) < Timeout:
            OpenPort()



