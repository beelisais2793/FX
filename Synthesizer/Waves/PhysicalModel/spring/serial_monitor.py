"""
/*
 This file is part of Virtual Robot's Pine Synthesizer. 

Copyright <2022> <Brian Gunnison>

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

Monitors COM port for messages from USB serial port
"""
import time


try:
    import serial # https://pythonhosted.org/pyserial/
    import serial.tools.list_ports
except:
    print('Serial not supported in Python, try: "pip install pyserial"')
    exit(1)

SERIAL_PORT_NAME = 'COM5' 
SERIAL_PORT_SPEED = 115200   


def fatal_exit(msg):
    print(f'Error - {msg}')
    print('Press any key to continue...')
    n = input()
    exit(1)


def log(msg):
    print(f'{msg}')



def list_serial_ports():
    """
    prints out a list of available serial ports
    """
    try:
        ports = serial.tools.list_ports.comports()
    except:
        fatal_exit('Error listing COM ports')


    my_port = None
    for port in ports:
        try:
            my_port = serial.Serial(port=port.device, baudrate=SERIAL_PORT_SPEED)
        except:
            log(f'Cannot open port "{port}", make sure no other apps have this port open')
            continue    # try next port

        log(f'Port "{port}" available')
        
        my_port.close()
        my_port = None
        continue


def open_serial_port(port_name=SERIAL_PORT_NAME, port_speed=SERIAL_PORT_SPEED):
    """
    Opens a serial COM port
    """

    while True:
        try:
            # timeout for readline
            port = serial.Serial(port_name, port_speed, timeout=2.0) 
        except IOError:
            log(f'Serial port {port_name} unavailable')
            time.sleep(1)
            continue
        break

    log(f'Opened serial port: {port_name}, speed: {port_speed}')
    return port



def get_response(port):
    """
    reads a message and prints the results
    """
    if port is None or port.is_open == False:
        fatal_exit('Port is not opened?')

    # we read a character at a time and end when we receive the '\n' (0x0A)
    response = ''
    while True:
        # Read bytes till we get a '\n'
        try:
            c = port.read()
            # if we get a non ascii character (unlikely) this will convert it
            # to something we can deal with
            c = c.decode('utf-8','ignore')  
        except:
            return None

        if c == '':
            continue   
            
        if c == '\n':   # indicates end of response
            break

        response += c

        if len(response) > 128:
            fatal_exit(f'Response too big {response}')

    log(f'MSG>> {response}')
    return response

   
def run_monitor():

    log('List available serial ports')
    list_serial_ports()    

    log('Open port')
    port = open_serial_port()
    if port is None:
       fatal_exit('Failed opening port')

    while True:
        msg = get_response(port)
        if msg is None:
            port.close()
            port = open_serial_port()  
        time.sleep(0.1)
    

if __name__ == "__main__":
    run_monitor()


