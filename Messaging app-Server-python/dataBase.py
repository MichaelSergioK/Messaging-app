from  tables import *
import threading
from pathlib import Path
import sqlite3
from uuid import *



lock=threading.Lock();

class Db:
    def __init__(self,clients,messages):
        try:
            flag=False
            my_file = Path('info.db')
            if my_file.is_file():
                flag=True
            self.connect=sqlite3.connect('info.db')
            self.connect.text_factory = bytes
            if flag==False:
                self.create_table(CLIENT)
                self.create_table(MESSAGE)
            else:
                self.fetch_data_from_db()
        except Exception:
            self.connect.close()
            raise

    def create_table(self,table_type):
        if table_type==CLIENT:
            self.connect.executescript("""
            CREATE TABLE Clients(userId VARCHAR(16) NOT NULL PRIMARY KEY 
            ,userName VARCHAR(255) NOT NULL
            ,publicKey VARCHAR(160) NOT NULL
            );""")
        else:
            self.connect.executescript("""
            CREATE TABLE Messages(messId INTEGER NOT NULL PRIMARY KEY 
            ,fromClient VARCHAR(16) NOT NULL
            ,toClient VARCHAR(16) NOT NULL
            ,type INTEGER
            ,content BLOB);
            """)

    def fetch_data_from_db(self):
        cur=self.connect.cursor()
        cur.execute('SELECT * FROM Messages')
        messgs_rows=cur.fetchall()
        for mss in messgs_rows:
            messages.append(Message(mss[0],mss[1],mss[2],mss[3],mss[4]))
        print(messages)
        if len(messages)>0:
            Message.id_generator=messages[len(messages)-1].id
        cur.execute('SELECT * FROM Clients')
        client_rows = cur.fetchall()
        for cli in client_rows:
            print(len(cli[0]))
            clients.append(Clients(UUID(bytes=cli[0]), cli[1].decode('utf-8'), cli[2],""))
        cur.close()


    # check if the name exist in the clients list
    def is_name_exist(self,name):
        flag = False
        lock.acquire()
        for cli in clients:
            if cli.name == name:
                flag = True
        lock.release()
        return flag


    def SaveData(self,data,tableType):
        self.save_data_ram(data,tableType)
        self.save_data_db(data,tableType)

    def save_data_db(self,data,tableType):
        query=None
        if tableType==MESSAGE:
            query = ''' INSERT INTO Messages(messId,fromClient,toClient,type,content)
                         VALUES(?,?,?,?,?) '''

        else:
            query = ''' INSERT INTO Clients(userId,userName,publicKey)
                                    VALUES(?,?,?) '''
        self.execute_save(data,query,tableType)

    def execute_save(self,data,query,table_type):
        cur=self.connect.cursor()
        if(table_type==MESSAGE):
            try:
                cur.execute(query,(data.id,data.fromClient,data.toClient,data.type,data.content,))
            except Exception as e:
                print(str(e))
            #cur.execute(query, (data.fromClient,data.toClient,data.type,data.content,))
        else:
            cur.execute(query,[data.id.bytes,data.name,data.publicKey])
        self.connect.commit()
        cur.close()

    def save_data_ram(self,data,tableType):
        table = None
        if tableType == CLIENT:
            table = clients
        else:
            table = messages
        lock.acquire()
        table.append(data)
        print(data)
        lock.release()


    # returns a list thats contains the messages to the client with the id - cid
    def messages_to_client(self,cid):
        temp_list = []
        lock.acquire()
        for mess in messages:
            if mess.toClient == cid:
                temp_list.append(mess)
        lock.release()
        return temp_list

    # removing the messages for cid from the messages
    def erase_messages_handled(self,cid):
        self.erase_messages_handled_ram(cid)
        self.erase_messages_handled_db(cid)

    def erase_messages_handled_db(self,cid):
        sql = 'DELETE FROM Messages WHERE toClient=?'
        cur = self.connect.cursor()
        cur.execute(sql,(cid,))
        self.connect.commit()
        cur.close()

    def erase_messages_handled_ram(self,cid):
        lock.acquire()
        messages[:] = [mess for mess in messages if mess.toClient != cid]
        lock.release()
    # return a list that contains all the clients except the client with the id- cid
    def get_other_clients(self,cid):
        temp_list = []
        lock.acquire()
        for cli in clients:
            if cli.id.bytes != cid:
                temp_list.append(cli)
        lock.release()
        return temp_list

    # returns the client with the id - cid
    def get_client(self,cid):
        client = None
        lock.acquire()
        for cli in clients:
            if cli.id.bytes == cid:
                client = cli
                break
        lock.release()
        return client

#
# #check if the name exist in the clients list
# def is_name_exist(name):
#     flag=False
#     lock.acquire()
#     for cli in clients:
#         if cli.name==name:
#             flag= True
#     lock.release()
#     return flag
#
# #save data in table of table type
# def SaveData(data,tableType):
#     table = None
#     if tableType == CLIENT:
#         table = clients
#     else:
#         table = messages
#     lock.acquire()
#     table.append(data)
#     print(data)
#     lock.release()
#
#
# #returns a list thats contains the messages to the client with the id - cid
# def messages_to_client(cid):
#     temp_list=[]
#     lock.acquire()
#     for mess in messages:
#         if mess.toClient==cid:
#             temp_list.append(mess)
#     lock.release()
#     return temp_list
#
# #removing the messages for cid from the messages
# def erase_messages_handled(cid):
#     lock.acquire()
#     messages[:] = [mess for mess in messages if mess.toClient!=cid]
#     lock.release()
#
# #return a list that contains all the clients except the client with the id- cid
# def get_other_clients(cid):
#     temp_list=[]
#     lock.acquire()
#     for cli in clients:
#         if cli.id.bytes!=cid:
#             temp_list.append(cli)
#     lock.release()
#     return temp_list
#
# #returns the client with the id - cid
# def get_client(cid):
#     client=None
#     lock.acquire()
#     for cli in clients:
#         if cli.id.bytes==cid:
#             client=cli
#             break
#     lock.release()
#     return client
#
