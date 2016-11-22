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


class TCPTalks:
    def __init__(self,IP):
        self.adresse = 0
        self.client = 0
        self.server = 0
        self.host = [ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][0]
        self.MySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if(IP != '0'):
            try:
                self.MySocket.bind((self.host,25565))
            except socket.error:
                print("error2")
            self.ordre = 1
            self.ip = IP
            self.client = Client()


        if(IP =='0'):
            try:
                self.MySocket.bind((self.host,25566))
            except socket.error:
                print("error2")
            self.ordre = 0
            self.ip = '0'
            self.client = Client()

    def send(self,variable):
        self.server.send(pickle.dumps(variable))

    def connect(self):
        if(self.ordre == 0):
            
            print("connection ...\nServer A ...")
            self.MySocket.listen(5)
            self.server, self.adresse = self.MySocket.accept()  
            self.ip = self.adresse[0]
            print("OK\nServer B ...")
            time.sleep(1)
            self.client.connexion(self.ip,25565)
            self.client.start()
            print("Ok\n connection established")
        if(self.ordre==1):
            print("connection ...\n Server A ...")
            self.client.connexion(self.ip,25566)
            self.client.start()
            self.MySocket.listen(5)
            self.server, self.adresse = self.MySocket.accept()  
            self.ip = self.adresse[0]
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





premier = TCPTalks('0')
premier.connect()
premier.send("estest")
time.sleep(5)
premier.send(["si ca marque",456423,"c est cool"])