import RPi.GPIO as GPIO
import time
#10 GPIO.BOARD

class Device:
	list_pin = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

class Switch(Device):
	def __init__(self,pininput,function=None,*args):
		if Device.list_pin[pininput]==0 :
			self.Function = function
			self.state = False
			Device.list_pin[pininput] =1
			self.Args =args
			self.PinInput = pininput
			if(GPIO.getmode()!=10):
				GPIO.setmode(GPIO.BOARD)
			GPIO.setup(self.PinInput,GPIO.IN,pull_up_down=GPIO.PUD_UP)

			
			try:
				GPIO.add_event_detect(self.PinInput,GPIO.BOTH,callback=self.LaunchFunction,bouncetime=5)
			except  RuntimeError :
				print("error")
			if(GPIO.input(self.PinInput)==0):
				self.state = True
			else:
				self.state = False
		else:
			print("Error pin already used")
			

	def LaunchFunction(self,a):
		time.sleep(0.001)

		if(GPIO.input(self.PinInput)==0):
			self.state = True
		else:
			self.state = False
		
		if self.Function !=None :
			self.Function(*self.Args)


	def SetFunction(self,function,*args):
		self.Function = function
		self.Args = args

	def Close(self):
		Device.list_pin[self.PinInput] =0
		GPIO.remove_event_detect(self.PinInput)
		GPIO.cleanup(self.PinInput)

 

class LightButton(Device):
	
	def __init__(self,pininput,pinLight,function=None,*args):
		if Device.list_pin[pininput]==0 and Device.list_pin[pinLight]==0:
			self.Function = function
			self.state = False
			Device.list_pin[pininput] =1
			Device.list_pin[pinLight]=1
			self.AutoSwitch = False
			self.Args =args
			self.PinInput = pininput
			self.PinLight = pinLight
			if(GPIO.getmode()!=10):
				GPIO.setmode(GPIO.BOARD)
			GPIO.setup(self.PinInput,GPIO.IN,pull_up_down=GPIO.PUD_UP)
			GPIO.setup(self.PinLight,GPIO.OUT)
			
			try:
				GPIO.add_event_detect(self.PinInput,GPIO.FALLING,callback=self.LaunchFunction,bouncetime=5)
			except  RuntimeError :
				print("error")
		else:
			print("Error pin already used")
			

	def LaunchFunction(self,a):
		if self.Function!=None:
			self.Function(*self.Args)

		if self.AutoSwitch==True :
			self.Switch()
	
	def SetAutoSwitch(self,value):
		self.AutoSwitch = value
	
	def On(self):
		self.state = True
		GPIO.output(self.PinLight,GPIO.HIGH)

	def Off(self):
		self.state = False
		GPIO.output(self.PinLight,GPIO.LOW)
	def Switch(self):
		if self.state ==False:
			self.On()
		else:
			self.Off()
	def SetFunction(self,function,*args):
		self.Function = function
		self.Args = args

	def Close(self):
		Device.list_pin[self.PinInput] =0
		Device.list_pin[self.PinLight]=0
		GPIO.remove_event_detect(self.PinInput)
		GPIO.cleanup(self.PinLight)
		GPIO.cleanup(self.PinInput)

 