#!/usr/bin/python2.7
# -*-coding:Latin-1 -*

import sys
import math
from _tkinter import *
from Tkinter import *
import threading

class Viewer :
    
    def __init__(self, root, nbForets) :
        
        
        self.nbPix = 5
        self.space = 1
        self.width = self.nbPix*20*20*self.space
        self.height = self.nbPix*20*self.space
        self.nbForets = nbForets
        self.readLoop = True
        
        self.canvas = Canvas(root, width=self.width, height=self.height, bg="black")
        self.canvas.pack()
        
        
        self.thread = threading.Thread(None, self.readLine)
        self.thread.start()
    
    #Function to draw a node or a leaf on the tree corresponding canvas
    def drawNode(self, canvas, node, isLeaf) :
        for depth in range(20) :
            if (math.pow(2,depth) > (node+1)) :
                depth=depth-1
                offsetX=self.width/2 - pow(2,depth)/2*self.nbPix
                #size=(self.width-((pow(2,depth)-1)*self.space))/(pow(2,depth))
                posY=(self.nbPix+self.space)*depth
                #posX=(node+1-math.pow(2,depth))*(size)#modifier ici
                posX=(node+1-math.pow(2,depth))*(self.nbPix+self.space)+offsetX
                print posX
                if (isLeaf) :
                    canvas.create_rectangle(posX+self.space, posY, posX+self.nbPix+self.space, posY+self.nbPix, fill='red')
                else :
                    canvas.create_rectangle(posX+self.space, posY, posX+self.nbPix+self.space, posY+self.nbPix, fill='blue')
                break
            
            
    def readLine(self) :
        while self.readLoop :
            string = sys.stdin.readline()
            text = self.parseLine(string)
            if (text != 0) :
                print text

        
    def parseLine(self, string) :
        if string.find("py") != 0 :
            return string
        
        if string.find("nbTree") >= 0 :
            pos = string.find("nbTree")
            return "detected"
        elif string.find("nbNode") >= 0 :
            py, pos, num = string.split(" ")
            self.drawNode(self.canvas, eval(num), False)
            return 0
        elif string.find("nbLeaf") >= 0 :
            py, pos, num = string.split(" ")
            self.drawNode(self.canvas, eval(num), True)
            return "leeeeeeaaaaaf"
        elif string.find("fin") >= 0 :
            self.readLoop = False
            return "fin"
        else :
            return "Unknown variable"
        
    def stop(self) :
        self.readLoop = False
    