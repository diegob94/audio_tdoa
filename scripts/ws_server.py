import asyncio
import websockets
import logging
import array
from math import log10

#logging.basicConfig(level=logging.DEBUG, format="%(asctime)s - %(levelname)s - %(message)s")


async def echo(websocket):
    dc = 0
    t = 0.995
    async for message in websocket:
#        print("received:",len(message),"bytes")
        samples = array.array('h',message).tolist()
        for sample in samples:
            dc = dc*t + sample*(1-t)
            sample = sample - dc
            #print(f'{dc:.2f},{sample:.2f}')
            print('#'*int(abs(sample)))

async def main():
    print("Starting server")
    async with websockets.serve(echo, "0.0.0.0", 4444):
        await asyncio.Future()  # run forever

asyncio.run(main())
