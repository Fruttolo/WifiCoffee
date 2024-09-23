import asyncio
import websockets

server_address = "ws://localhost:8081"
COFFEE_MACHINE_ID = "COFFEE_MACHINE_ID"
COFFEE_MACHINE_TOKEN = "COFFEE_MACHINE_TOKEN"

async def on_message_callback(websocket, message):
    print(f"Ricevuto messaggio: {message}")
    if message == "MAN_ON":
        print("MAN_ON")
    elif message == "MAN_OFF":
        print("MAN_OFF")
    elif message == "BLU_ON":
        print("BLU_ON")
    elif message == "BLU_OFF":
        print("BLU_OFF")
    elif message == "ACC_ON":
        print("ACC_ON")
    elif message == "ACC_OFF":
        print("ACC_OFF")
    elif message.startswith("Associate"):
        global COFFEE_MACHINE_TOKEN
        _, token = message.split()
        COFFEE_MACHINE_TOKEN = token
        print(f"ID e TOKEN associati a: {COFFEE_MACHINE_ID} {COFFEE_MACHINE_TOKEN}")
        await websocket.send(f'{{ "ID": "{COFFEE_MACHINE_ID}", "TOKEN": "{COFFEE_MACHINE_TOKEN}" }}')
    elif message == "Ping":
        print("Ping ricevuto")
        await websocket.send("Pong")
    elif message == "Pong":
        print("Pong ricevuto")
        await websocket.send("Ping")

async def on_events_callback(websocket):
    async for message in websocket:
        await on_message_callback(websocket, message)

async def main():
    async with websockets.connect(server_address) as websocket:
        print("Connesso al WebSocket server")
        await websocket.send(f'{{ "ID": "{COFFEE_MACHINE_ID}", "TOKEN": "{COFFEE_MACHINE_TOKEN}" }}')
        await on_events_callback(websocket)

asyncio.get_event_loop().run_until_complete(main())