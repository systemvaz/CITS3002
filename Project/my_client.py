import socket
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ('localhost', 4444)
print('Connecting to %s port %s' % server_address)
sock.connect(server_address)
print('-------------------------')
print('1: Join Game')
print('2: Quit')
option = raw_input('Enter option number: ')
type(option)


message = 'INIT'
print('Sending INIT message')
sock.sendall(message.encode())

while True:
    # Look for the response
    amount_received = 0
    amount_expected = 1

    while amount_received < amount_expected:
        data = sock.recv(1024)
        amount_received += len(data)
        mess = data.decode()
        if "WELCOME" in mess:
            print("Welcome received")
            print("--> %s" % mess)
        elif "START" in mess:
            print("The games have begun")
            print("--> %s" % mess)
            # sock.sendall('100,MOV,CON,1'.encode()) # Client has ID 231
        elif "ELIM" in mess:
            print("We lost, closing connection")
            exit = True
            break

print ('closing socket')
message = 'QUIT'
sock.sendall(message)
sock.close()
