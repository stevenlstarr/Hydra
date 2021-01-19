import serial

ser = serial.Serial('COM3', '115200')
ser.write(b'led=on\n')

hello = ser.readline()
print(hello)