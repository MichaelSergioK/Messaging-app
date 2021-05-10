import struct
VERS=1
REQ_HEADER_LENGTH = 22
ANS_HEADER_LENGTH=7
WAITING_MESSAGES_REPL_PSIZE=25
SEND_MESS_PYALOAD_WO_CONTENT=21

#unpacking the header of a request
def tcp_unpack_header_req(header):
    try:
        (clientID,version,code,payloadSize)=struct.unpack('!16sBBI',header[:REQ_HEADER_LENGTH])
        return clientID,version,code,payloadSize
    except BaseException as e:
        print("failed to unpack the header req")
        raise
#packing the header of a reply
def tcp_packet_header_reply(code,payloadSize):
    try:
        buffer=struct.pack('<BHI',VERS,code,payloadSize)
        return buffer
    except BaseException as e:
        print("failed to prepare the header packet")
        raise

#packing the reply for the public key request
def tcp_packet_req_for_pk_reply(clientID,public_key):
    try:
      buffer=struct.pack('<16s160s',clientID,public_key)
      return buffer
    except BaseException as e:
        print("failed to prepare the request for public key reply")
        raise
#packing the reply for the waiting messages request
def tcp_packet_waiting_mess_reply(message):
    try:
      exp='<16sIBI'+str(len(message.content))+'s'
      buffer=struct.pack(exp,message.fromClient,message.id,message.type,len(message.content),message.content)
      return buffer
    except BaseException as e:
        print("failed to prepare the request for waiting messages reply")
        print(str(e))

#packing the reply for the send message request
def tcp_packet_send_mess_reply(cid,mid):
    try:
        exp="<16sI"
        buffer=struct.pack(exp,cid,mid)
        return buffer
    except BaseException as e:
        print("failed to prepare the request for send message reply")
        raise

#unpacking the register request payload
def unpack_register_payload(payload,payloadSize,nameLength):
    try:
        expr='!'+str(nameLength)+'s'+str(payloadSize-nameLength)+'s'
        (name,publicKey)=struct.unpack(expr,payload)
        return name,publicKey
    except BaseException as e:
        print("failed to unpack the register request payload")
        raise
#unpacking the send message request payload
def unpack_send_mess_payload(payload,payloadSize):
    try:
        exp = "!16sBI"+ str(payloadSize-SEND_MESS_PYALOAD_WO_CONTENT) + "s"
        (id,type,content_size,content)=struct.unpack(exp,payload)
        return id,type,content_size,content
    except BaseException as e:
        print("failed to unpack the send message request payload")
        raise
