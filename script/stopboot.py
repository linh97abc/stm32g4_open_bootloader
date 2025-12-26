import serial
import time

PORT = 'COM3'


class StopBoot:
    def __init__(self, port):
        self.ser = serial.Serial(port, 115200)

    def task_ping(self):
        seq = 0
        while True:
            print(f"Pinging seq {seq}...")
            seq += 1
            self.ser.write(b'StopBoot\n')
            time.sleep(0.1)
            response = self.ser.read_all()
            # print(f"Response: {response}")
            if response == b'OK\n':
                print("Ping successful!")
                return

StopBoot(PORT).task_ping()
