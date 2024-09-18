import asyncio
import websockets

accendi = False
manopola = False
secondi_aspettare = 20

server_address = "ws://localhost:8081"
coffee_machine_id = "COFFEE_MACHINE_ID_2"
coffee_machine_token = "COFFEE_MACHINE_TOKEN"

async def connect_to_websocket():
    while True:
        try:
            async with websockets.connect(server_address) as websocket:
                print("Connesso al WebSocket server")
                await websocket.send(f'{{ "ID": "{coffee_machine_id}", "TOKEN": "{coffee_machine_token}" }}')
                await webSocketEvent(websocket)
        except (websockets.exceptions.ConnectionClosedError, ConnectionRefusedError) as e:
            print(f"Connessione fallita: {e}. Riprovo tra 5 secondi...")
            await asyncio.sleep(5)

async def webSocketEvent(websocket):
    global accendi, manopola, secondi_aspettare

    async for message in websocket:
        print(f"Ricevuto messaggio: {message}")
        if message == "Accendi" or message == "Spegni":
            accendi = True
            print("Accensione eseguita")
        elif message == "Manopola":
            manopola = True
            print("Manopola girata")
        elif message.startswith("SetSecondi"):
            _, new_seconds = message.split()
            secondi_aspettare = int(new_seconds)
            print(f"Secondi da aspettare impostati a: {secondi_aspettare}")

async def main():
    await connect_to_websocket()

if __name__ == "__main__":
    asyncio.run(main())