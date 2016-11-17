#!/usr/bin/python3
#-*- coding: utf-8 -*-
#pickle.dumps
import os
from socket import *
import time
from threading import Thread
import socket
import pickle

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
        variable = pickle.dumps(variable)
        self.server.send(variable)

        
        
    def connect(self):
        if(self.ordre == 0):
            print("connection ...\nServer A ...")
            
 
            self.ip = self.server.connexion()
            print("OK\nServer B ...")
            time.sleep(1)
            self.client.connexion(self.ip,25565)
            self.client.start()
            print("Ok\n connection established")
        if(self.ordre==1):
            print("connection ...\n Server A ...")
            self.client.connexion(self.ip,25566)
            self.client.start()
            self.server.connexion()
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

        if(port ==25566):
            print("Ok \n connection established")
        

    def run(self):
        while True:
            
            time.sleep(0.5)
            var = self.MySocket.recv(4096)
            print(pickle.loads(var))



class Server():
    def __init__(self,port):
        self.adresse = 0
        self.port = port
        self.client = 0
        self.connexion = False

        self.host = gethostbyname(gethostname())
        self.MySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.MySocket.bind((self.host,self.port))
        except socket.error:
            print("error2")
    def send(self,var):
        self.client.send(var)

    def connexion(self):
        self.MySocket.listen(5)
        self.client, self.adresse = self.MySocket.accept()    
        self.connexion = True
        return(self.adresse[0])

    def getip(self):
        if(self.connexion == False):
            return(0)
        else:
            return(self.adresse[0])




premier = PythonCom('0')
premier.connect()
        

    







