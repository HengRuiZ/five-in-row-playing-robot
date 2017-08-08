import mySerial
import os
import readFile
import stringGenerator
from time import sleep
'''
#Serial Part
myserial = mySerial.mySerial('/dev/ttyUSB0', 9600)
print 'Serial Initialization Successed!'
#initial test
myserial.send('#8200300654456+666-999$')
print 'Send first information'
print (myserial.receive())
#sleep(8)
myserial.send('#@200300654456+666-999$')
print (myserial.receive())
#sleep(8)
myserial.send('#8200200654456+200-200$')
print (myserial.receive())
#sleep(8)
myserial.send('#a200300200200+666-999$')
print (myserial.receive())
#sleep(8)
myserial.closePort()
'''
#Serial Initialization
myserial = mySerial.mySerial('/dev/ttyUSB0', 9600)


#Imaging Processing



#Sound Processing


#Main A infinite loop
while True:
	os.cmd('') # System Command
	[isFinished,x,y] = readFile.readFile()
	if isFinished == True:
	    break
	else:
	    move(x,y)


def move(a,b): #Problems! Don't know the specific steps in one cycle!
    myserial.send(resetString())
    myserial.send(getString())
    myserial.send(graspString())
    myserial.send(turnToString(x,y))
    myserial.send(putDownString())
