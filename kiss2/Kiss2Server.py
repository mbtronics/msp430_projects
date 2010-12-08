import time, serial, virtkey, gtk

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

def simulate_keys(keys):
    """ simulate the keys using python-virtkey
        :param keys: (modifiers, keysym); returned by keystroke_to_x11
    """
    modifiers, key = keys

    v = virtkey.virtkey()
    if modifiers:
        v.lock_mod(modifiers)
    try:
        v.press_keysym(key)
        v.release_keysym(key)
    finally:
        if modifiers:
            v.unlock_mod(modifiers)

def keystroke_to_x11(keystroke):
    """ convert "CTRL+Shift+T" to (1<<2 | 1<<0, 28)
        :param keystroke: The keystroke string.
                         - can handle at least one 'real' key
                         - only ctrl, shift and alt supported yet (case-insensitive)
        :returns: tuple: (modifiers, keysym)
    """
    modifiers = 0
    key = ""
    splitted = keystroke.split("+")
    for stroke in splitted:
        lstroke = stroke.lower()
        if lstroke == "ctrl" or lstroke == "control":
            modifiers |= (1 << 2)
        elif lstroke == "shift":
            modifiers |= (1 << 0)
        elif lstroke == "alt":
            modifiers |= (1 << 3) # TODO: right?
        else: # is a ordinary key (Only one)
            key = gtk.gdk.keyval_from_name(stroke)
    return (modifiers, key)

OpenPort()


while 1:
    Time = time.time()

    try:
        Line = Port.readline()
    except serial.serialutil.SerialException:
        OpenPort()

    if Line != "":
        Line = Line.strip()
        print Line

        if Line == "next":
            simulate_keys(keystroke_to_x11('CTRL+SHIFT+N'))
        elif Line == "toggle pause":
            simulate_keys(keystroke_to_x11('CTRL+SHIFT+P'))
        elif Line == "volume up":
            simulate_keys(keystroke_to_x11('CTRL+SHIFT+U'))
        elif Line == "volume down":
            simulate_keys(keystroke_to_x11('CTRL+SHIFT+D'))
    else:
        if (time.time() - Time) < Timeout:
            OpenPort()



