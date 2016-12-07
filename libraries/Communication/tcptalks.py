#!/usr/bin/python3
#-*- coding: utf-8 -*-
import os
import time
from threading import Thread, RLock, Event
import socket
import pickle
import subprocess

#envoie de commande CMD et liste d'attente
MySocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server = 0

class TCPTalks(Thread):
    def __init__(self,IP):
        global MySocket
        global server
        Thread.__init__(self)
        self.library = lib()
        self.adresse = 0
        self.running = Event()
        self.port2 = 0
        self.waitingdict = dict()
        self.client = 0
        self.lock_dict = RLock()
        self.server = 0
        self.host = self.getip()
        self.MySocket2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if(IP != '0'):
            try:
                MySocket.bind((self.host,25565))
            except socket.error:
                print("Error, port 25565 already used")
            self.ordre = 1
            self.ip = IP
        if(IP =='0'):
            try:
                MySocket.bind((self.host,25566))
            except socket.error:
                print("Error, port 25566 already used")
            self.ordre = 0
            self.ip = '0'

    def getip(self):
        if os.name == 'nt':
            return([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][0])
        if os.name == 'posix':
            a = os.popen("""ifconfig | awk '/inet adr/ {gsub("adr:", "", $2); print $2}'""").readlines()
            a.remove('127.0.0.1\n')
            return(a[0][0:-1])



    def send(self,variable):
        global MySocket
        global server
        if type(variable) != type([1,2]):

            print('error type, you give ' + str(type(variable)) + "but is watting <Class list>")
            return()
        else:
            try:
                server.send(pickle.dumps(variable))
            except socket.error:
                return()


    def connect(self):
        global MySocket
        global server
        if(self.ordre == 0):
            print("In waiting")
            MySocket.listen(5)
            server, self.adresse = MySocket.accept()  
            self.ip = self.adresse[0]
            time.sleep(1)
            self.port2 = 25565
            self.connection()
            self.start()
            print("Connection established")

        if(self.ordre==1):
            self.port2 = 25566
            self.connection()
            self.start()
            MySocket.listen(5)
            server, self.adresse = MySocket.accept()  
            self.ip = self.adresse[0]
            print("Connection established")

    def close(self):
        global MySocket
        self.send([10,'off'])
        MySocket.close()
        self.MySocket2.close()
        self.library = {}
        print("Connexion closed")
        self.running.set()


    def connection(self):
        marqueur1 = 0
        marqueur2 = True
        while(marqueur2):
            marqueur2 = False
            try:
                self.MySocket2.connect((self.ip, self.port2))
            except:
                print('error1')
                marqueur2 = True
                marqueur1 = marqueur1 + 1
                time.sleep(0.2)
            if(marqueur1 >5): 
                return()

    def getqueue(self,id):
        self.lock_dict.acquire()
        try:
            queue = self.waitingdict[opcode]
        except KeyError:
            queue = self.waitingdict[opcode] = Queue()
        finally:
            self.queues_lock.release()
        return(queue)


    def get(self,id):
        try:
            var = self.getqueue(id).get_nowait()
        except Empty:
            var = 'none'
        return var


    def run(self):
        while not self.running.is_set():
            
            time.sleep(0.5)
            try:
                rcv_Var = pickle.loads(self.MySocket2.recv(8096))
            except socket.error or self.running.is_set():
                marqueur =0
            else:
                if len(rcv_Var)==3 and rcv_Var[0]==2:
                   self.library[rcv_Var[1]] = rcv_Var[2]
                elif len(rcv_Var)==2:
                    self.getqueue(rcv_Var[1]).put(rcv_Var[2])
                if marqueur ==-1:
                    print(rcv_Var)
        print("Connection closed")

            
class lib(dict):

    def __getattr__(self,name):
        for k in self:
            if k == name:
                return(self[name])
        return('none')
    def __setattr__(self,name,value):

        global MySocket
        global server
        self[name] = value
        print(value)
        print(name)
        var = [2,name,value]
        server.send(pickle.dumps(var))

            

