#!/usr/bin/python3
#-*- coding: utf-8 -*-
import os
import time
from threading import Thread
import socket
import pickle

#gestion des erreurs
#gestion liste de reception 
#création flux


class PythonCom:
    def __init__(self,IP):
        if(IP != '0'):
            self.ordre = 1
            self.ip = IP
            self.client = Client()
            self.server = Server(25565)


        if(IP =='0'):
            self.ordre = 0
            self.ip = '0'
            self.server= Server(25566)
            self.client = Client()

    def send(self,variable):
        self.server.send(pickle.dumps(variable))
        
    def connect(self):
        if(self.ordre == 0):
            print("connection ...\nServer A ...")
            self.server.connect()
            while (self.server.getip()==0):
                time.sleep(1)
            self.ip = self.server.getip()
            print("OK\nServer B ...")
            time.sleep(1)
            self.client.connexion(self.ip,25565)
            self.client.start()
            self.server.stop()
            print("Ok\n connection established")
        if(self.ordre==1):
            print("connection ...\n Server A ...")
            self.client.connexion(self.ip,25566)
            self.client.start()
            self.server.connect()
            while (self.server.getip()==0):
                time.sleep(1)
            print("OK\nServer B ...")





            
class Client(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.ip = '0'
        self.order = -1
        self.MySocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)


    def connexion(self, ip, port):
        self.ip = ip 
        self.port = port
        marqueur1 = 0
        marqueur2 = True
        print(ip)
        while(marqueur2):
            marqueur2 = False
            try:
                self.MySocket.connect((self.ip, self.port))
            except:
                print('error1')
                marqueur2 = True
                marqueur1 = marqueur1 + 1
            if(marqueur1 >5): 
                return()

    def run(self):
        while True:
            
            time.sleep(1)
            print(pickle.loads(self.MySocket.recv(8096)))
            print("efse")




class Server:
    def __init__(self,port):
        self.adresse = 0
        self.port = port
        self.client = 0
        self.connexion = False

        self.host = [ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][0]
        self.MySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.MySocket.bind((self.host,self.port))
        except socket.error:
            print("error2")

    def getip(self):
        if(self.connexion == False):
            return(0)
        else:
            return(self.adresse[0])

    def send(self,variable):
        self.client.send(variable)

    def stop(self): 
        self.Terminated = True

    def connect(self):
        self.MySocket.listen(5)
        self.client, self.adresse = self.MySocket.accept()    
        #print("Client connécté , IP = %s et Port = %s" % (self.adresse[0] , self.adresse[1]))
        self.connexion = True


premier = PythonCom('0')
premier.connect()
premier.send("estest")
time.sleep(5)
premier.send(["si ca marque",456423,"c est cool"])