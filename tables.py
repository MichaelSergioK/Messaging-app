
#represents a message in the chat app
class Message:
    id_generator=0
    def __init__(self,id,fromClient,toClient,type,content):
        self.id=id
        self.fromClient=fromClient
        self.toClient=toClient
        self.type=type
        self.content=content
    def __str__(self):
        return "id: "+str(self.id)+ " ,from client: "+str((self.fromClient).hex())+ " ,to client: "+str((self.toClient).hex())+",type: "+str(self.type)+"\ncontent: "+str(self.content.hex())

#represents a client in the chat app
class Clients:
    def __init__(self,id,name,publicKey,lastSeen):
        self.id=id
        self.name=name
        self.publicKey=publicKey
        self.lastSeen=lastSeen

    def __str__(self):
        return "name: "+self.name+ " ,id: "+self.id.hex+ " ,pk: "+str(self.publicKey)#+" ,last seen: "+self.lastSeen


clients=[]
messages=[]
CLIENT=1
MESSAGE=2