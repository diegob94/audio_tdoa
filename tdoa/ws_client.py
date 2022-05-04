#!/usr/bin/env python

import numpy as np
import asyncio
import websockets
import pybinn

packet_size = 100

async def main():
    while True:
        try:
            async with websockets.connect("ws://localhost:5000/tx1") as websocket:
                t = 0
                while True:
                    y = np.random.rand(packet_size)
                    x = np.arange(t,t+packet_size)
                    assert len(x) == len(y)
                    data = [[float(i),float(j)] for i,j in zip(x,y)]
                    print("send:",np.array(data).shape)
                    try:
                        await websocket.send(pybinn.dumps(data))
                    except websockets.exceptions.ConnectionClosedError:
                        print("Connection closed, reconnecting in 5 seconds...")
                        break
                    await asyncio.sleep(1)
                    t = x[-1]
        except ConnectionRefusedError:
            print("Connection refused, retrying in 5 seconds...")
        await asyncio.sleep(5)

asyncio.run(main())
