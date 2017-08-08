import serial

class mySerial(object):
    def __init__(self, serialPort, speed):
        self.mySerialPort = serial.Serial(serialPort, speed, timeout = 1.0)

    def send(self, data):
        self.mySerialPort.write(data)

    def receive(self): 
        while True:  
            data = self.mySerialPort.read(20)
            if data == '':
                print 'Pending'
                continue
            else:
                break
        return data

    def closePort(self):
        self.mySerialPort.close()