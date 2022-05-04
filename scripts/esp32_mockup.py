import multiprocessing as mp
import socket
import random
import time
import math
import pybinn

client_count = 2
buffer_size = 4
max_packet_count = 2
send_delay = 1

port = 5555
fs = 48000
max_tsf_error = 25
max_signal = 65535
max_start_delay = 10000

class TestSocket:
    def __init__(self, host, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
    def send(self, msg):
        msg_bytes = pybinn.dumps(msg)
        print("str:",len(str(msg)),"send:",len(msg_bytes))
        MSGLEN = len(msg_bytes)
        totalsent = 0
        while totalsent < MSGLEN:
            sent = self.sock.send(msg_bytes[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

def f(start_delay,socket,mic_id):
    buffer = []
    tsf = start_delay
    signal = 0
    packet_count = 0
    sample_count = 0
    while True:
        signal += random.gauss(0,0.1)
        signal_int = int(max_signal*math.sin(signal))
        tsf_int = int(abs(tsf + random.gauss(0,max_tsf_error*5)))
        buffer.append([tsf_int,signal_int])
        sample_count += 1
        if len(buffer) == buffer_size:
            socket.send([mic_id,buffer])
            buffer = []
            packet_count += 1
            time.sleep(send_delay)
        if max_packet_count is not None and packet_count == max_packet_count:
            break
        tsf += 1e6/fs
    print(f"{packet_count=} {sample_count=}")

if __name__ == '__main__':
    processes = []
    for i in range(client_count):
        mic_id = i
        s = TestSocket('localhost',port)
        start_delay = random.uniform(0,max_start_delay)
        p = mp.Process(target=f,args=(start_delay,s,mic_id))
        p.start()
        processes.append(p)
    for p in processes:
        p.join()

