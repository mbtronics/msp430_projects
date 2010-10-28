import time, serial, virtkey

#commands
#--------
#next, toggle pause, volume up, volume down
#
#virtkey info: http://ubuntuforums.org/showthread.php?t=609804

Timeout = 5

print "Starting KISS V2.0 server..."
print

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

Key = virtkey.virtkey()


while 1:
    Time = time.time()
    Line = Port.readline()

    if Line != "":
        Line = Line.strip()
        print Line

        if Line == "next":
            Key.press_unicode(ord("n"))
            Key.release_unicode(ord("n"))
        elif Line == "toggle pause":
            Key.press_unicode(ord(" "))
            Key.release_unicode(ord(" "))
        elif Line == "volume up":
            Key.press_unicode(ord("+"))
            Key.release_unicode(ord("+"))
        elif Line == "volume down":
            Key.press_unicode(ord("-"))
            Key.release_unicode(ord("-"))
    else:
        if (time.time() - Time) < Timeout:
            OpenPort()



