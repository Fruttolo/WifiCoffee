# create a test file for the websocket server
import asyncio
import websockets

async def hello():
    uri = "ws://localhost:8080"
    async with websockets.connect(uri) as websocket:
        await websocket.send("CoffeeMachine1")
        while True:
            response = await websocket.recv()
            print(response)

asyncio.get_event_loop().run_until_complete(hello())