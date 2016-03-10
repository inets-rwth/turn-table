import serial
import threading
import time
import sys

class control:
    def __init__(self, com_port_name):
        self.curr_pos = 0
        self.com_port = com_port_name
        self.serial_port = serial.Serial()
        self.serial_port.baudrate = 19200
        self.serial_port.port = self.com_port
        self.serial_port.bytesize = serial.EIGHTBITS
        self.serial_port.parity = serial.PARITY_NONE
        self.serial_port.stopbits = serial.STOPBITS_ONE
        self.serial_port.timeout = 0
        self.operation_in_progress = False
        self.curr_ans = ""

    def open(self):
        self.serial_port.open()
        self.run_input_thread = True
        self.input_thread = threading.Thread(target=self.monitor_input)
        self.input_thread.start()
        self.load_settings()

    def load_settings(self):
        self.operation_in_progress = True
        self.serial_port.write("get spr\r\n")
        while(self.operation_in_progress):
            time.sleep(0.1)
        self.spr = int(self.curr_ans)
        print("settings loaded. spr = " + str(self.spr))

    def close(self):
        while self.operation_in_progress:
            time.sleep(1)
        print 'closing connection'
        self.serial_port.close()
        self.run_input_thread = False
        if self.input_thread.isAlive():
            print 'joining thread'
            self.input_thread.join()
            print 'thread exited'

    def move_to(self, pos_deg):
        diff = pos_deg - self.curr_pos
        self.curr_pos = pos_deg
        if diff == 0:
            return
        diff_steps = (self.spr/360.0)*diff
        print "moving: " + str(float(diff)) + ' deg => ' + str(diff_steps) + ' steps'
        if abs((int(diff_steps) - diff_steps)) > 0.000001:
            print "WARNING: I can't move by this amount accurately"
            print "WARNING: moving " + str(int(diff_steps)) + " steps instead"
        self.operation_in_progress = True
        self.serial_port.write("move " + str(int(diff_steps))+"\r\n")
        while self.operation_in_progress:
            time.sleep(0.1)

    def trigger_remote_table(self):
        self.operation_in_progress = True
        self.serial_port.write("trig\r\n")
        while self.operation_in_progress:
            time.sleep(0.1)

    def set_transmission_ratio(self, trans):
        self.operation_in_progress = True
        self.serial_port.write("set trans " + str(trans) + "\r\n")
        while self.operation_in_progress:
            time.sleep(0.1)

    def monitor_input(self):
        curr_line = ''
        while self.run_input_thread and self.serial_port.isOpen():
            try:
                 curr_byte = self.serial_port.read()
                 if curr_byte != '' and self.operation_in_progress:
                     if curr_byte != '\n' and curr_byte != '\r':
                         curr_line += str(curr_byte)
                     if curr_byte == '\r':
                         self.curr_ans = curr_line
                         curr_line = ""
                         self.operation_in_progress = False
            except:
                curr_line = ""
                self.operation_in_progress = False
                break
        print 'exiting input thread'

if __name__ == "__main__":
    pos = sys.argv[1]

    con = control("/dev/ttyACM0")
    con.open()
    con.move_to(float(pos))
    con.close()

