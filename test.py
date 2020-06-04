import socket


TCP_IP = '127.0.0.1'
TCP_PORT = 8989
BUFFER_SIZE = 1024
MESSAGE = """ The pthread_detach() function marks the thread identified by thread as detached.  When a detached thread terminates, its resources are
       automatically released back to the system without the need for
       another thread to join with the terminated thread.\n"""

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
sent = s.send(MESSAGE.encode('utf-8'))
print(f"Sent {sent} bytes")
data = s.recv(BUFFER_SIZE)
print(f"Message from server : {data}")
