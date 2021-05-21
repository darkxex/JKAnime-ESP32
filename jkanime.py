import urequests
from machine import Pin, I2C
import ssd1306
import time
i2c = I2C(-1, scl=Pin(22), sda=Pin(21))
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

def cap_sentence(s):
  return ' '.join(w[:1].upper() + w[1:] for w in s.split(' '))

def do_connect():
    import network
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
        sta_if.connect('CamelloVTR', 'Cieloestrellado37')
        while not sta_if.isconnected():
            pass
    print('network config:', sta_if.ifconfig())
    
do_connect()
initial = False;
response = urequests.get('https://jkanime.net/')

series = []
programa = []
oled.text("RipNotify 2", 20, 30)
oled.show()
while True:
    
    decoder = response.raw.readline().decode("utf-8")
    
    first = decoder.find("listadoanime-home")
    if (first != -1):
        initial = True
    
    
    ser = decoder.find("destacados spad")
    if(initial == True):
        counter = decoder.find("https://jkanime.net/")
        counter2 = decoder.find('"',counter)
        title = decoder[counter:counter2]
        title = title.replace("https://jkanime.net/","")
        title = title.replace("/"," ")
        title = title.replace("-"," ")
        title = cap_sentence(title)
        if (counter != -1):
           
            series.append(title)
            

        counter = decoder.find("<span>")
        fecha = decoder.replace("<span>","").strip()
        if (counter != -1):
            
          
            programa.append(fecha)
            
    if (ser != -1):
        break
#Termina el cutter
while True:
    for x in range(30):
        for mod in range(30,-8*len(series[x]),-4):
            oled.fill(0)
            oled.text(programa[x],45,10)
            oled.text(series[x], mod, 30)
            oled.line(0, 22, 128, 22, 1) 
            oled.show()
            time.sleep_ms(40)
        
        
        
        
        
