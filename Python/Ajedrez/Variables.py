import pygame
pygame.init()

##Constantes
ventx, venty = 800, 800 #Ventana_X, Ventana_Y
vent = pygame.display.set_mode((ventx, venty)) #Ventana

canth = cantv = 8 #*Cantidad de cuadriculas en una fila(x) y columna(y)
canth_corr = cantv_corr = canth + 2 #Cantidad de cuadriculas en una fila(x) y columna(y) corregidas con margen
tamh = tamv = ventx//canth_corr #Tamaño horizontal(x) y vertical(y) de cada cuadricula (corregido)

tam_fuente = (tamh+tamv)//3 #Tamaño de la fuente
fuente = pygame.font.SysFont("Arial", tam_fuente) #Fuente estándar

colores = [(255,255,255),(0,0,0)] #Colores de las fichas y el tablero
imagenes = [
			[
			pygame.image.load("Sprites/PeonB.png"),
			pygame.image.load("Sprites/TorreB.png"),
			pygame.image.load("Sprites/CaballoB.png"),
			pygame.image.load("Sprites/AlfilB.png"),
			pygame.image.load("Sprites/ReyB.png"),
			pygame.image.load("Sprites/DamaB.png")
			],
			[
			pygame.image.load("Sprites/PeonN.png"),
			pygame.image.load("Sprites/TorreN.png"),
			pygame.image.load("Sprites/CaballoN.png"),
			pygame.image.load("Sprites/AlfilN.png"),
			pygame.image.load("Sprites/ReyN.png"),
			pygame.image.load("Sprites/DamaN.png")
			]
		   ]

abecedario = ["A","B","C","D","E","F","G","H"] #Letras de columna

##Variables
cuadricula1D = [] #Lista de cuadrículas (1D)

cuadricula2D = [] #Lista de cuadrículas (2D)
for x in range(8):
	cuadricula2D.append([])
	for y in range(8):
		cuadricula2D[x].append([0,0])

fichas = [] #Lista de fichas en el tablero

jug_actual = 0 #Jugador jugando

all_movs_capts = [[],[]] #[movimientos] y [capturas] de las fichas del jugador actual

ficha_sel = None #Ficha seleccionada
ficha_sel_movs = [] #Posibles movimientos de la ficha seleccionada
ficha_sel_capts = [] #Posibles capturas de la ficha seleccionada

jaque_cuad = None #Cuadro que contiene al rey en jaque
ganador = None #Ganador de la partida

cons_output = False

##Clases principales
class Ficha:
	def __init__(self, tipo, jugador):
		self.tipo = tipo
		self.jugador = jugador
		self.imagen = imagenes[jugador][tipo]
		self.cuad = None

	def posibles_movimientos(self):
		x = self.cuad.numcoord[0]
		y = self.cuad.numcoord[1]
		movs = []
		capts = []

		def comprobar(cuad_targ, p):
			stop=False

			if cuad_targ.ficha == None:
				movs.append(cuad_targ)
				p+=1
			elif cuad_targ.ficha.jugador != self.jugador:
				capts.append(cuad_targ)
				stop=True
			else:
				stop=True
			return stop, p;

		if self.tipo == 0:
			pymod = 0
			if self.jugador == 0:
				pymod = 1
			else:
				pymod = -1

			if y == 5*self.jugador + 2:
				for p in range(1,3):
					cuad_targ = cuadricula2D[x-1][(y+pymod*p)-1]
					if cuad_targ.ficha == None: movs.append(cuad_targ)
					else: break 
			elif y+pymod in range(1,9):
				cuad_targ = cuadricula2D[x-1][(y+pymod)-1]
				if cuad_targ.ficha == None: movs.append(cuad_targ)
			
			for p in [1,-1]:
				if x+p in range(1,9) and y+pymod in range(1,9):
					cuad_targ = cuadricula2D[(x+p)-1][(y+pymod)-1]
					if cuad_targ.ficha != None and cuad_targ.ficha.jugador != self.jugador:
						capts.append(cuad_targ)

		if self.tipo == 1:
			for mx in [-1,1]:
				p=1
				while x+p*mx in range(1,9):
					cuad_targ = cuadricula2D[(x+p*mx)-1][y-1]
					stop, p = comprobar(cuad_targ, p)
					if stop: break
			for my in [-1,1]:
				p=1
				while y+p*my in range(1,9):
					cuad_targ = cuadricula2D[x-1][(y+p*my)-1]
					stop, p = comprobar(cuad_targ, p)
					if stop: break

		if self.tipo == 2:
			for px in [-2, 2]:
				for py in [1, -1]:
					if x+px in range(1,9) and y+py in range(1,9):
						cuad_targ = cuadricula2D[(x+px)-1][(y+py)-1]
						comprobar(cuad_targ, 0)
			for py in [-2, 2]:
				for px in [1, -1]:
					if x+px in range(1,9) and y+py in range(1,9):
						cuad_targ = cuadricula2D[(x+px)-1][(y+py)-1]
						comprobar(cuad_targ, 0)
		
		if self.tipo == 3:
			for mx in [-1,1]:
				for my in [-1,1]:
					p=1
					while x+p*mx in range(1,9) and y+p*my in range(1,9):
						cuad_targ = cuadricula2D[(x+p*mx)-1][(y+p*my)-1]
						stop, p = comprobar(cuad_targ, p)
						if stop: break

		if self.tipo == 4:
			for px in range(-1,2):
				for py in range(-1,2):
					if x+px in range(1,9) and y+py in range(1,9):
						cuad_targ = cuadricula2D[(x+px)-1][(y+py)-1]
						comprobar(cuad_targ, 0)

		if self.tipo == 5:
			for mx in range(-1,2):
				for my in range(-1,2):
					p=1
					while x+p*mx in range(1,9) and y+p*my in range(1,9):
						cuad_targ = cuadricula2D[(x+p*mx)-1][(y+p*my)-1]
						stop, p = comprobar(cuad_targ, p)
						if stop: break

		return movs, capts
class Cuadro:
	def __init__(self, numcoord, chesscoord, rect, color):
		self.numcoord = numcoord
		self.chesscoord = chesscoord
		self.rect = rect
		self.color = color

		self.ficha = None
