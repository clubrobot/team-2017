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


class TCPTalks(Thread):
    def __init__(self,IP):
        Thread.__init__(self)
        self.MySocket2 = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.adresse = 0
        self.port2 = 0
        self.client = 0
        self.server = 0
        if os.name == 'nt':
            self.host = [ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][0]
        if os.name == 'posix':
            a = os.popen("""ifconfig | awk '/inet adr/ {gsub("adr:", "", $2); print $2}'""").readlines()
            a.remove('127.0.0.1\n')
            self.host = a[0][0:-1]

        self.MySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if(IP != '0'):
            try:
                self.MySocket.bind((self.host,25565))
            except socket.error:
                print("error2")
            self.ordre = 1
            self.ip = IP



        if(IP =='0'):
            try:
                self.MySocket.bind((self.host,25566))
            except socket.error:
                print("error2")
            self.ordre = 0
            self.ip = '0'


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
            self.port2 = 25565
            self.connexion()
            self.start()
            print("Ok\n connection established")
        if(self.ordre==1):
            print("connection ...\n Server A ...")
            self.port2 = 25566
            self.connexion()

            self.start()
            self.MySocket.listen(5)
            self.server, self.adresse = self.MySocket.accept()  
            self.ip = self.adresse[0]
            print("OK\nServer B ...")

    def connexion(self):
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

    def run(self):
        while True:
            
            time.sleep(1)
            print(pickle.loads(self.MySocket2.recv(8096)))
            print("efse")






premier = TCPTalks('192.168.1.14')
premier.connect()
premier.send("estest")
time.sleep(5)
premier.send(["si ca marque",456423,"c est cool"])