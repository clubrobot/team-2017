import RPi.GPIO as GPIO
#10 GPIO.BOARD

class LightButton:
	list_pin = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
	def __init__(self,pininput,pinLight,function,*args):
		if LightButton.list_pin[pininput]==0 and LightButton.list_pin[pinLight]==0:
			self.Function = function
			self.state = False
			LightButton.list_pin[pininput] =1
			LightButton.list_pin[pinLight]=1
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
		LightButton.list_pin[self.PinInput] =0
		LightButton.list_pin[self.PinLight]=0
		GPIO.remove_event_detect(self.PinInput)
		GPIO.cleanup(self.PinLight)
		GPIO.cleanup(self.PinInput)

 
