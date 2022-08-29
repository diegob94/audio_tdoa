import asyncio
import websockets
import logging
import array
from math import log10
from websockets.exceptions import ConnectionClosedError

#logging.basicConfig(level=logging.DEBUG, format="%(asctime)s - %(levelname)s - %(message)s")


async def echo(websocket):
    audio_id = None
    dc = 0
    t = 0.995
    tag = f"{websocket.id}_{audio_id}:"
    while True:
        if websocket.closed:
            break
        tag = f"{websocket.id}_{audio_id}:"
        try:
            message = await websocket.recv()
        except ConnectionClosedError as e:
            break
#        print("received:",len(message),"bytes")
        try:
            audio_id,timestamp,samples = array.array('H',message[:2]).tolist()[0], array.array('q',message[2:2+8]).tolist()[0], array.array('h',message[2+8:]).tolist()
        except ValueError:
            print(tag,"Error: message cannot be decoded")
            continue
        print(tag,len(message),audio_id,timestamp,len(samples))
        continue
        samples = array.array('h',message).tolist()
        for sample in samples:
            dc = dc*t + sample*(1-t)
            sample = sample - dc
            #print(f'{dc:.2f},{sample:.2f}')
            print('#'*int(abs(sample)))
    print(tag,"Connection closed!!!!!!!!!!!")

async def main():
    print("Starting server")
    async with websockets.serve(echo, "0.0.0.0", 4444):
        await asyncio.Future()  # run forever

asyncio.run(main())
