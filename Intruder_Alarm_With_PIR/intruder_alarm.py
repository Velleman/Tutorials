import RPi.GPIO as GPIO
import time
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import smtplib

GPIO.setmode(GPIO.BCM)

Pir = 17
Relais = 5

GPIO.setup(Pir, GPIO.IN)
GPIO.setup(Relais, GPIO.OUT)

print("Startup")
time.sleep(2)
print("Ready")

while True:
    if GPIO.input(Pir):
        msg = MIMEMultipart()
        msg['From'] = "Your Email Adress"
        msg['To'] = "Email Adress Receiver"
        msg['Subject'] = "Intruder ALARM!"
        
        body = "The text of your message"
        msg.attach(MIMEText(body, 'html'))

      
        server = smtplib.SMTP('smtp.gmail.com', 587)
        server.starttls()
        server.login(msg['From'], "Your Password")
        server.sendmail(msg['From'], msg['To'], msg.as_string())
        server.quit()
        print ("Movement detected!" + (time.strftime(" %H:%M:%S")))
        GPIO.output(Relais, GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(Relais, GPIO.LOW)
        time.sleep(0.2)
        GPIO.output(Relais, GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(Relais, GPIO.LOW)
        time.sleep(0.2)
        GPIO.output(Relais, GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(Relais, GPIO.LOW)
        time.sleep(5)
    else:
        GPIO.output(Relais, GPIO.LOW)
        
            
 
