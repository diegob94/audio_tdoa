import asyncio
import websockets
import pybinn
import logging

#logging.basicConfig(level=logging.DEBUG, format="%(asctime)s - %(levelname)s - %(message)s")


async def echo(websocket):
    async for message in websocket:
        print("received:",pybinn.loads(message))

async def main():
    async with websockets.serve(echo, "0.0.0.0", 4444):
        await asyncio.Future()  # run forever

asyncio.run(main())
