import asyncio
import websockets

server_address = "ws://localhost:8081/"
coffee_machine_id = "COFFEE_MACHINE_ID"
coffee_machine_token = "COFFEE_MACHINE_TOKEN"

async def on_message(websocket, message):
    print(f"Ricevuto messaggio: {message}")
    if message == "Accendi" or message == "Spegni":
        print("Accensione eseguita")
    elif message == "Manopola":
        print("Manopola girata")
    elif message.startswith("SetSecondi"):
        new_seconds = int(message.split()[1])
        print(f"Secondi da aspettare impostati a: {new_seconds}")
    elif message.startswith("Associate"):
        token = message.split()[1]
        print(f"ID e TOKEN associati a: {coffee_machine_id} {token}")
        await websocket.send(f'{{ "ID": "{coffee_machine_id}", "TOKEN": "{token}" }}')
    elif message.startswith("Wifi"):
        ssid_new, password_new = message.split()[1:3]
        print(f"Nuove credenziali WiFi impostate a: {ssid_new} {password_new}")
        # Simulate WiFi connection
        await asyncio.sleep(5)
        print("Connesso al WiFi")

async def on_event(websocket, path):
    async for message in websocket:
        await on_message(websocket, message)

async def connect():
    async with websockets.connect(server_address) as websocket:
        print("Connesso al WebSocket server")
        await websocket.send(f'{{ "ID": "{coffee_machine_id}", "TOKEN": "{coffee_machine_token}" }}')
        await on_event(websocket, server_address)

async def main():
    while True:
        try:
            await connect()
        except Exception as e:
            print(f"Errore: {e}")
            print("Tentativo di riconnessione al WebSocket server...")
            await asyncio.sleep(5)

if __name__ == "__main__":
    asyncio.run(main())