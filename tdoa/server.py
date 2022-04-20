import multiprocessing as mp
from multiprocessing.context import Process
import zmq
import pybinn

class DataCollectorProcess(mp.Process):
    def __init__(self,buffer_size):
        self.buffer_size = buffer_size
        super().__init__()
    def run(self):
        context = zmq.Context()
        socket = context.socket(zmq.STREAM)
        socket.bind("tcp://*:5555")
        buffer = {}
        connections = set()
        total_data = 0
        while True:
            id = socket.recv().hex()
            payload = socket.recv()
            #print("DEBUG:",id,repr(payload))
            if payload == b'':
                if id in connections:
                    print(f"connection closed: {id}")
                else:
                    print(f"connection accepted: {id}")
                    connections.add(id)
                continue
            mic_id,data = pybinn.loads(payload)
            total_data += len(data)
            buffer.setdefault(mic_id,[]).extend(data)
            if total_data >= self.buffer_size:
                print(f"send: {buffer}")
                total_data = 0
                buffer = {}
            print(f"{total_data=}")

if __name__ == "__main__":
    buffer_size = 20
    p = DataCollectorProcess(buffer_size)
    p.start()
    p.join()
