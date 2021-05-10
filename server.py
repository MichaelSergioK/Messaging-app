# import socket
# import threading
# from tables import *
# from _datetime import datetime
# import uuid
# from dataBase import *
# from Packeting import*
# import sys
#import atexit
#
#
#
# MAX_NAME_LENGTH=255
# NULL=0
# REGISTER_REQ=100
# REGISTER_REPL=1000
# CLIST_REQ=101
# CLIST_REPL=1001
# PUBLIC_KEY_REQ=102
# PUBLIC_KEY_REPL=1002
# SEND_MESS_REQ=103
# SEND_MESS_REPL=1003
# REQ_FOR_SKEY=1
# SEND_SKEY=2
# TEXT_MESSAGE=3
# WAITING_MESSAGES_REQ=104
# WAITING_MESSAGES_REPL=1004
# PUBLIC_KEY_LENGTH=160
#
# ERROR_REPL=9000
#
# ID_LENGTH=16
#
#
# def currTime():
#     curr=datetime.now()
#     # dd/mm/YY H:M:S
#     dt_string = curr.strftime("%d/%m/%Y %H:%M:%S")
#     return dt_string
#
#
# #handle the requests from the clients
# def handle_requests(conn):
#  try:
#     header = conn.recv(REQ_HEADER_LENGTH) #recive request header
#     clientID , version, code, payloadSize=tcp_unpack_header_req(header)
#     payloadSize=socket.ntohl(payloadSize)#I unpacked the header in big endian so here I transform it back to litle endian
#     payload=None
#     if(payloadSize>0):
#         payload=conn.recv(payloadSize)
#     code_to_check=code
#     target = None
#     func_name=None
#     if code_to_check == REGISTER_REQ:
#         target = register
#         func_name = "register"
#     elif code_to_check == CLIST_REQ:
#         target = req_for_clients_list
#         func_name = "client list request"
#     elif code_to_check == PUBLIC_KEY_REQ:
#         target = req_for_public_key
#         func_name = "public key request"
#     elif code_to_check == SEND_MESS_REQ:
#         target =send_message
#         func_name = "send message request"
#     elif code_to_check == WAITING_MESSAGES_REQ:
#         target = req_for_waiting_mess
#         func_name = "waiting messages request"
#     else:
#         raise BaseException
#     target(clientID,payloadSize,payload,conn,func_name)
#     conn.close()
#  except BaseException as e:
#      print(str(e))
#      func_name = "failed to unpacking header request"
#      failed(conn,func_name)
#
# #returns the name of the client without the extra charachters
# def name_extrac(name):
#     counter = 0
#     extracedName=""
#     while name[counter] != NULL:  # calculate the name length
#         extracedName+=chr(name[counter])
#         counter += 1
#       # null terminated
#     return extracedName
#
# #send error code to the client
# def send_error(conn,func):
#     replyPacket = tcp_packet_header_reply(ERROR_REPL, 0)
#     print(func+" failed")
#     conn.sendall(replyPacket)
#
# #register request- saves the client name , generate an uuid and send back the uuid
# def register(clientID,payloadSize,payload,conn,func_name):
#     name,publicKey=unpack_register_payload(payload,payloadSize,MAX_NAME_LENGTH)
#     name=name_extrac(name)
#     if is_name_exist(name):#this client name is occupied
#         send_error(conn,func_name)
#         return
#     lastSeen=currTime()
#     clientID = uuid.uuid4()
#     cl=Clients(clientID,name,publicKey,lastSeen)
#     buffer=tcp_packet_header_reply(REGISTER_REPL, ID_LENGTH)
#     replyPacket=buffer
#     replyPacket+=clientID.bytes
#     conn.sendall(replyPacket)
#     SaveData(cl, CLIENT)
#     print(func_name)
#
#
# #handle a request for clients list- returns all the clients names and id's except the client thats send the request
# def req_for_clients_list(clientID,payloadSize,payload,conn,func_name):
#     buffer=tcp_packet_header_reply(CLIST_REPL,(len(clients)-1)*(ID_LENGTH+MAX_NAME_LENGTH))
#     replyPacket=buffer
#     get_client(clientID).lastSeen=currTime()#the requesting client last seen
#     clients_list=get_other_clients(clientID)
#     for cli in clients_list:
#           replyPacket+=cli.id.bytes
#           replyPacket += cli.name.encode('utf-8')
#           replyPacket+=((MAX_NAME_LENGTH-len(cli.name))*'\0').encode() #client expect for 255 length name
#     conn.sendall(replyPacket)
#     print(func_name)
#
# #handles a request for a public key
# def req_for_public_key(clientID,payloadSize,payload,conn,func_name):
#     if get_client(payload)==None: #client requested not exist
#         send_error(conn,func_name)
#         return
#     public_key=get_client(payload).publicKey
#     buffer=tcp_packet_header_reply(PUBLIC_KEY_REPL,PUBLIC_KEY_LENGTH+ID_LENGTH)
#     replyPacket=buffer
#     buffer=tcp_packet_req_for_pk_reply(clientID,public_key)
#     replyPacket+=buffer
#     conn.sendall(replyPacket)
#     print(func_name)
#
# #calculates the payload size of the reply for the waiting messages request
# def calculate_wmess_repl_size(clientID):
#     repl_psize=0
#     messages_to_cli=messages_to_client(clientID)
#     for mess in messages_to_cli:
#          repl_psize += WAITING_MESSAGES_REPL_PSIZE + len(mess.content)
#     return repl_psize
#
# #handles the request for waiting messages
# def req_for_waiting_mess(clientID,payloadSize,payload,conn,func_name):
#     repl_psize=calculate_wmess_repl_size(clientID)#calculet the size of all the messages to return
#     buffer=tcp_packet_header_reply(WAITING_MESSAGES_REPL,repl_psize)
#     replyPacket=buffer
#     temp_list=messages_to_client(clientID)#gets the messages for the requesting client
#     erase_messages_handled(clientID)#erase the waiting messages for the requesting client
#     for mess in temp_list:
#         buffer=tcp_packet_waiting_mess_reply(mess)
#         replyPacket+=buffer
#     print(len(replyPacket))
#     conn.sendall(replyPacket)
#     print(func_name)
#
# #handles the send message request
# def send_message(clientID,payloadSize,payload,conn,func_name):
#     id,type,content_size,content=unpack_send_mess_payload(payload,payloadSize)
#     if not any(cli.id.bytes == id for cli in clients):#if the destination client not exist
#         send_error(conn,func_name)
#         return
#     SaveData(Message(Message.id_generator+1,clientID,id,type,content),MESSAGE)
#     buffer=tcp_packet_header_reply(SEND_MESS_REPL,ID_LENGTH+4)#4 represent the num of bytes for the message id
#     replyPacket=buffer
#     buffer=tcp_packet_send_mess_reply(id,Message.id_generator)
#     replyPacket+=buffer
#     conn.sendall(replyPacket)
#     print(func_name)
#
# #send an error to the client
# def failed(conn,func_name):
#     send_error(conn,func_name)
#
#
# #the server reads the port from the file and listen at this port to clients
# #for every client there  a thread is dispatched and hanndle the client request
# def main():
#     try:
#         file = open("port.info.txt", "r")
#         port = file.read()
#         file.close()
#
#         with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serversocket:
#             serversocket.bind(('', int(port)))
#             serversocket.listen()
#             while True:
#                 conn , addr =serversocket.accept();
#                 threading.Thread(target=handle_requests(conn),args=(conn,)).start()
#     except FileNotFoundError as e:
#         print(str(e))
#         print("the file 'port.info' doesnt exist!")
#     except OSError as e:
#         print(str(e))
#         print("connection with client failed")
#
# if __name__=='__main__':
#     main()

import socket
from _datetime import datetime
import uuid
from dataBase import *
from Packeting import*
import sqlite3
import atexit




MAX_NAME_LENGTH=255
NULL=0
REGISTER_REQ=100
REGISTER_REPL=1000
CLIST_REQ=101
CLIST_REPL=1001
PUBLIC_KEY_REQ=102
PUBLIC_KEY_REPL=1002
SEND_MESS_REQ=103
SEND_MESS_REPL=1003
REQ_FOR_SKEY=1
SEND_SKEY=2
TEXT_MESSAGE=3
WAITING_MESSAGES_REQ=104
WAITING_MESSAGES_REPL=1004
PUBLIC_KEY_LENGTH=160

ERROR_REPL=9000

ID_LENGTH=16

def exit_handler(db):
    db.connect.close()

def currTime():
    curr=datetime.now()
    # dd/mm/YY H:M:S
    dt_string = curr.strftime("%d/%m/%Y %H:%M:%S")
    return dt_string


#handle the requests from the clients
def handle_requests(conn,database):
 try:
    header = conn.recv(REQ_HEADER_LENGTH) #recive request header
    clientID , version, code, payloadSize=tcp_unpack_header_req(header)
    payloadSize=socket.ntohl(payloadSize)#I unpacked the header in big endian so here I transform it back to litle endian
    payload=None
    if(payloadSize>0):
        payload=conn.recv(payloadSize)
    code_to_check=code
    target = None
    func_name=None
    if code_to_check == REGISTER_REQ:
        target = register
        func_name = "register"
    elif code_to_check == CLIST_REQ:
        target = req_for_clients_list
        func_name = "client list request"
    elif code_to_check == PUBLIC_KEY_REQ:
        target = req_for_public_key
        func_name = "public key request"
    elif code_to_check == SEND_MESS_REQ:
        target =send_message
        func_name = "send message request"
    elif code_to_check == WAITING_MESSAGES_REQ:
        target = req_for_waiting_mess
        func_name = "waiting messages request"
    else:
        raise BaseException
    target(clientID,payloadSize,payload,conn,func_name,database)
    conn.close()
 except BaseException as e:
     print(str(e))
     func_name = "failed to unpacking header request"
     failed(conn,func_name)
     raise

#returns the name of the client without the extra charachters
def name_extrac(name):
    counter = 0
    extracedName=""
    while name[counter] != NULL:  # calculate the name length
        extracedName+=chr(name[counter])
        counter += 1
      # null terminated
    return extracedName

#send error code to the client
def send_error(conn,func):
    replyPacket = tcp_packet_header_reply(ERROR_REPL, 0)
    print(func+" failed")
    conn.sendall(replyPacket)

#register request- saves the client name , generate an uuid and send back the uuid
def register(clientID,payloadSize,payload,conn,func_name,database):
    name,publicKey=unpack_register_payload(payload,payloadSize,MAX_NAME_LENGTH)
    name=name_extrac(name)
    if database.is_name_exist(name):#this client name is occupied
        send_error(conn,func_name)
        return
    lastSeen=currTime()
    clientID = uuid.uuid4()
    cl=Clients(clientID,name,publicKey,lastSeen)
    buffer=tcp_packet_header_reply(REGISTER_REPL, ID_LENGTH)
    replyPacket=buffer
    replyPacket+=clientID.bytes
    conn.sendall(replyPacket)
    database.SaveData(cl, CLIENT)
    print(func_name)


#handle a request for clients list- returns all the clients names and id's except the client thats send the request
def req_for_clients_list(clientID,payloadSize,payload,conn,func_name,database):
    buffer=tcp_packet_header_reply(CLIST_REPL,(len(clients)-1)*(ID_LENGTH+MAX_NAME_LENGTH))
    replyPacket=buffer
    database.get_client(clientID).lastSeen=currTime()#the requesting client last seen
    clients_list=database.get_other_clients(clientID)
    for cli in clients_list:
          replyPacket+=cli.id.bytes
          replyPacket += cli.name.encode('utf-8')
          replyPacket+=((MAX_NAME_LENGTH-len(cli.name))*'\0').encode() #client expect for 255 length name
    conn.sendall(replyPacket)
    print(func_name)

#handles a request for a public key
def req_for_public_key(clientID,payloadSize,payload,conn,func_name,database):
    if database.get_client(payload)==None: #client requested not exist
        send_error(conn,func_name)
        return
    public_key=database.get_client(payload).publicKey
    buffer=tcp_packet_header_reply(PUBLIC_KEY_REPL,PUBLIC_KEY_LENGTH+ID_LENGTH)
    replyPacket=buffer
    buffer=tcp_packet_req_for_pk_reply(clientID,public_key)
    replyPacket+=buffer
    conn.sendall(replyPacket)
    print(func_name)

#calculates the payload size of the reply for the waiting messages request
def calculate_wmess_repl_size(clientID,database):
    repl_psize=0
    messages_to_cli=database.messages_to_client(clientID)
    for mess in messages_to_cli:
         repl_psize += WAITING_MESSAGES_REPL_PSIZE + len(mess.content)
    return repl_psize

#handles the request for waiting messages
def req_for_waiting_mess(clientID,payloadSize,payload,conn,func_name,database):
    repl_psize=calculate_wmess_repl_size(clientID,database)#calculet the size of all the messages to return
    buffer=tcp_packet_header_reply(WAITING_MESSAGES_REPL,repl_psize)
    replyPacket=buffer
    temp_list=database.messages_to_client(clientID)#gets the messages for the requesting client
    database.erase_messages_handled(clientID)#erase the waiting messages for the requesting client
    for mess in temp_list:
        buffer=tcp_packet_waiting_mess_reply(mess)
        replyPacket+=buffer
    print(len(replyPacket))
    conn.sendall(replyPacket)
    print(func_name)

#handles the send message request
def send_message(clientID,payloadSize,payload,conn,func_name,database):
    id,type,content_size,content=unpack_send_mess_payload(payload,payloadSize)
    if database.get_client(id)==None:#if the destination client not exist
        send_error(conn,func_name)
        return
    database.SaveData(Message(Message.id_generator+1,clientID,id,type,content),MESSAGE)
    Message.id_generator+=1;
    buffer=tcp_packet_header_reply(SEND_MESS_REPL,ID_LENGTH+4)#4 represent the num of bytes for the message id
    replyPacket=buffer
    buffer=tcp_packet_send_mess_reply(id,Message.id_generator)
    replyPacket+=buffer
    conn.sendall(replyPacket)
    print(func_name)

#send an error to the client
def failed(conn,func_name):
    send_error(conn,func_name)


#the server reads the port from the file and listen at this port to clients
#for every client there  a thread is dispatched and hanndle the client request
def main():
    try:
        file = open("port.info.txt", "r")
        port = file.read()
        file.close()
        database=Db(clients,messages)
        atexit.register(exit_handler,database)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serversocket:
            serversocket.bind(('', int(port)))
            serversocket.listen()
            while True:
                conn , addr =serversocket.accept();
                threading.Thread(target=handle_requests(conn,database),args=(conn,database)).start()
    except FileNotFoundError as e:
        print(str(e))
        print("the file 'port.info' doesnt exist!")
    except OSError as e:
        print(str(e))
        print("connection with client failed")
    except sqlite3.DatabaseError as e:
        print(str(e))
        print("connection with database failed")

if __name__=='__main__':
    main()
    print()