import os
import glob
import time
import lcdlib as lcd

LCD_LINE_1 = 0x80 # LCD RAM adress for 1st line
LCD_LINE_2 = 0xC0 # LCD RAM adress for 2nd line

#intialise display
lcd.init(25,24,23,17,18,22,16)

os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

base_dir = '/sys/bus/w1/devices/'
device_folder = glob.glob(base_dir + '28*')[0]
device_file = device_folder + '/w1_slave'

def read_temp_raw():
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines

#CELSIUS CALCULATION
def read_temp_c():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = int(temp_string) / 1000.0 
        temp_c = str(round(temp_c, 1
        return temp_c

#FAHRENHEIT CALCULATION
def read_temp_f():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_f = (int(temp_string) / 1000.0) * 9.0 / 5.0 + 32.0 
        temp_f = str(round(temp_f, 1)) 
        return temp_f

while True:

    lcd.string("Temp: " + read_temp_c() + chr(223) + "C",LCD_LINE_1)
    lcd.string("           " + read_temp_f() + chr(223) + "F",LCD_LINE_2)