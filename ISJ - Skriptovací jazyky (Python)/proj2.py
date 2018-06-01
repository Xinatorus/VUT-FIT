class Polynomial:
	
	def __init__(self, *args, **kwargs):
		"""
		@brief      { Konstruktor tridy Polynomial }
		"""
		if(args): 
			if(type(args[0])==list): #pokud se zadal list
				self.poly=list(args[0])
			else: #jinak se hodnoty postupne priradi 
				self.poly=[0]*int(len(args))
				for i in range(len(args)):
					self.poly[i] = args[i];
			
		if(kwargs): #pokud se zadaly (x0=1,x3=2,x1=-3,...)
			keys=list(kwargs.keys()) #nacteni klicu
			keys.sort(reverse=True) #serazeni podle velikosti sestupne
			maxIndex=keys[0][1:] #nejvyssi prvek, retezec za x

			#vyplnit pole
			self.poly=[0]*(int(maxIndex)+1) #vythovreni listu s nulovymi hodnotami
			for key,value in kwargs.items():
				self.poly[int(key[1:])] = value #ulozeni do listu podle mocniny x

	#textova reprezentace polynomu
	def __str__(self):
		"""
		@brief      { Textova reprezentace polynomu }
		
		@return     { String ve tvaru ax^n + ... }
		"""
		string=str() #prazdny string
		for i in reversed(range(len(self.poly))): #od nejvysiho radu
			if(self.poly[i]==0): #clen neexistuje/nulovy
				continue
			else:
				if(self.poly[i]>0 and i!=len(self.poly)-1): #kladne hodnoty, krome nejvyssiho clenu
					string=string+'+ '
				elif(self.poly[i]<0): #zaporne hodnoty
					string=string+'- '
				if(i==0): #posledni clen
					string=string+str(abs(self.poly[i]))
				else:
					if(self.poly[i]==1): #nevypisuje jednicky pred x
						string=string+'x'
					else:
						string=string+str(abs(self.poly[i]))+'x'
					if(i>1): #nevypisuje mocninu 1
						string=string+'^'+str(i)
					string=string+' '
		return string

	#scitani
	def __add__(self,next):
		"""
		@brief      { Funkce pro scitani dvou trid Polynomial }
		
		@param 		self  { 1. polynom }
		@param      next  { 2. polynom }
						
		
		@return     { Vraci novy polynom, ktery je souctem zadanych polynomu }
		"""
		#secteni jednotlivych hodnot a plozeni do noveho polynomu
		newPoly=Polynomial([x + y for x, y in zip(self.poly, next.poly)])
		return newPoly

	#umocnovani
	def __pow__(self,x):
		"""
		@brief      { Funkce pro vypocet mocniny polynomu }
		
		@param      self  { mocnitel }
		@param      x     { mocnina }
		
		@return     { Vraci novy polynom umocneny na x }
		"""
		newList=[0]*(int(len(self.poly)-1)*x+1) #vytvoreni noveho listu
		                                        #s velikosti podle max. mocniny
		old = list(self.poly) #meziclen
		for n in range(x-1):
			newList=[0]*(int(len(self.poly)-1)*x+1) #vynulovani 
			size=(len(self.poly)-1)*(n+1) #prepocet rozsahu pro meziclen
			for i in range(size+1): #smycka meziclenu
				for j in range(len(self.poly)): #smycka puvodniho polynomu
					index = i+j #nova mocnina
					newList[index]=newList[index]+old[i]*self.poly[j] #hodnota clenu
			old = list(newList) #prenastaveni meziclenu = mocniny > 2
		return Polynomial(newList)

	#vypocet hodnoty polynomu
	def at_value(self,*args):
		"""
		@brief      { Funkce pro vypocet hodnty polynomus }
		
		@param      self  { Polynom }
		@param      args  { Hodnoty, pro ktere se bude polynom vypocitavat. Max 2 hodnoty.
									Pokud byly zadany dve hodnoty, vysledkem je jejich rozdil. }
		
		@return     { Vraci hodnotu (INT) polynomu resp. rozdil dvou hodnot polynomu }
		"""
		if(len(args)==1): #jeden argument
			num=0
			for i in reversed(range(len(self.poly))): #sestupne
				num=num+(self.poly[i]*(int(args[0])**i)) #postupny vypocet 
			return num
		elif(len(args)==2): #dva argumenty, dela se rozdil
			num=0
			num=self.at_value(args[1])-self.at_value(args[0]) #rekurze (s jednim arg)
			return num
		else:
			return None

	#derivace
	def derivative(self):
		"""
		@brief      { Funkce pro derivaci }
		
		@return     { Vraci novy polynom, ktery je derivaci puvodniho polynomu }
		"""
		newList=[0]*int(len(self.poly)-1) #nulovany list
		for i in range(len(self.poly)-1): # 0 -> n-1, c' = 0
			newList[i]=self.poly[i+1]*(i+1) #snizeni mocniny a vynasobeni clenu
		return Polynomial(newList)
