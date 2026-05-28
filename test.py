import serial
import time

PORT = "/dev/tty.usbserial-0001"
BAUD = 1_000_000

ser = serial.Serial(PORT, BAUD, timeout=0.2)

pkt = bytes([0xFF, 0xFF, 0x01, 0x02, 0x01, 0xFB])

ser.reset_input_buffer()
print("TX:", pkt.hex(" "))
ser.write(pkt)
ser.flush()
time.sleep(0.05)

resp = ser.read(32)
print("RX len:", len(resp))
print("RX:", resp.hex(" "))

ser.close()
