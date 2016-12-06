#!/usr/bin/python3
#-*- coding: utf-8 -*-
import os
import time
from threading import Thread
import socket
import pickle

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
        self.running = True
        self.port2 = 0
        self.waiting = []
        self.client = 0
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
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.connect(("8.8.8.8", 80))
            s.close()
            return s.getsockname()[0]



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
            
    def cmd(self,variable):
        if type(variable) != type(""):
            print('error type, you give ' + str(type(variable)) + "but is watting <Class String>")
            return()
        self.send([3,0,variable])

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
        self.running = False


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

    def getType(self,liste):
        if len(liste) == 3 and liste[0] == 2:  #gestion de varaible
            return(2)
        if len(liste) ==3 and liste[0] == 3 and liste[1]==0:
            return(3)
        if len(liste) ==2 and liste[0]==10 and liste[1] == "off":
            return(10)
        return(-1)

    def run(self):
        while self.running:
            
            time.sleep(0.5)
            try:
                rcv_Var = pickle.loads(self.MySocket2.recv(8096))
            except socket.error or self.running == False:
                
                marqueur =0
            else:
                marqueur = self.getType(rcv_Var)
            if  marqueur==2:
                self.library[rcv_Var[1]] =rcv_Var[2]
            if marqueur ==3:
                self.send([3,1,os.popen(rcv_Var[2],'r').read()])
            if marqueur ==10:
                self.close()
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
            

