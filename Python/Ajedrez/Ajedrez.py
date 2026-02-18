from Variables import *
import sys

def dibujar_tablero():
	
	#Fondo
	vent.fill((0,0,0))

	#Cuadricula
	for cuadro in cuadricula1D: 
		pygame.draw.rect(vent, cuadro.color, cuadro.rect)

	#Movimiento y capts
	if ficha_sel != None:
		for cuad in ficha_sel_movs:
			pygame.draw.rect(vent, (0,0,(255+cuad.color[2])/2), cuad.rect)
		for cuad in ficha_sel_capts:
			pygame.draw.rect(vent, ((255+cuad.color[0])/2,0,0), cuad.rect)

	#Jaque
	if jaque_cuad != None: 
		pygame.draw.rect(vent, (0,(255+jaque_cuad.color[1])/2,0), jaque_cuad.rect)

	#Imagenes de las fichas
	for ficha in fichas:
		vent.blit(ficha.imagen, (ficha.cuad.rect.centerx-tamh//2, ficha.cuad.rect.centery-tamv//2))

	#Construccion de las lineas verticales que determinan la cantidad de cuadriculas en una fila(x)
	for i in range(1, canth_corr):
		if i == 1 or i == canth_corr-1:
			pygame.draw.line(vent, (255,255,255), (i*tamh, tamv), (i*tamh, venty-tamv))

		#Numeros de columna
		if i<canth_corr-1:
			texto = fuente.render(abecedario[i-1], 5, (255,255,255))
			vent.blit(texto, (i*tamh + tamh//3, 0))
			vent.blit(texto, (i*tamh + tamh//3, venty-tamv))
		
	#Construccion de las lineas horizontales que determinan la cantidad de cuadriculas en una columna(y)
	for i in range(1, cantv_corr):
		if i == 1 or i == cantv_corr-1:
			pygame.draw.line(vent, (255,255,255), (tamh, i*tamv), (ventx-tamh, i*tamv))
		
		#Numeros de fila
		if i<cantv_corr-1:
			texto = fuente.render(str(9-i), 5, (255,255,255))
			vent.blit(texto, (2*tamh//5, i*tamv + tamv//9))
			vent.blit(texto, (ventx-tamh//1.5, i*tamv + tamv//9))

	#Actualizar pantalla
	pygame.display.update()

##Funciones de tablero
def colocar_ficha(ficha, cuad):
	cuad.ficha = ficha
	ficha.cuad = cuad
	if fichas.count(ficha) == 0:
		fichas.append(ficha)
def mover_ficha(ficha, cuad):
	ficha.cuad.ficha = None
	colocar_ficha(ficha, cuad)
def borrar_ficha(ficha):
	ficha.cuad.ficha = None
	fichas.remove(ficha)
	del ficha

##Funciones de juego
def check_cuad_jaque(fichas_sim=fichas):
	for ficha in fichas_sim:
		if ficha.jugador != jug_actual:
			movs, com = ficha.posibles_movimientos()

			for cuad in com:
				if cuad.ficha.tipo == 4:
					return cuad
	return None
def preparar_turno():

	##Comprobar fichas
	for ficha in fichas:
		
		#Coronación del peón
		if ficha.tipo == 0 and ficha.cuad.numcoord[1] == 8-ficha.jugador*7:
			ficha.tipo = 5
			ficha.imagen = imagenes[ficha.jugador][ficha.tipo]

		#Calcular y corregir movimientos del jugador actual
		if ficha.jugador == jug_actual:
			movs, capts = ficha.posibles_movimientos()
			movs_inv = []
			capts_inv = []

			for cuad_mov in movs:
				cuad_ant = ficha.cuad

				mover_ficha(ficha, cuad_mov)

				if check_cuad_jaque() != None:
					movs_inv.append(cuad_mov)

				mover_ficha(ficha, cuad_ant)

			for cuad_capt in capts:
				fichas_sim = fichas.copy()
				ficha_borr = cuad_capt.ficha
				cuad_ant = ficha.cuad

				cuad_capt.ficha = None
				fichas_sim.remove(ficha_borr)
				
				mover_ficha(ficha, cuad_capt)

				if check_cuad_jaque(fichas_sim) != None:
					capts_inv.append(cuad_capt)

				mover_ficha(ficha, cuad_ant)
				
				cuad_capt.ficha = ficha_borr

			for mov in movs_inv:
				movs.remove(mov)
			for capt in capts_inv:
				capts.remove(capt)

			all_movs_capts[0].append(movs)
			all_movs_capts[1].append(capts)
		else:
			all_movs_capts[0].append(None)
			all_movs_capts[1].append(None)

	##Comprobar jaque y jaque mate
	global ganador, jaque_cuad

	jaque_cuad = check_cuad_jaque()
	if jaque_cuad != None:
		print(f"Jaque a rey {jaque_cuad.ficha.jugador} en {jaque_cuad.chesscoord}")
		
		can_exit_jaque = False
		
		for movs_or_capts in all_movs_capts:
			for mov_or_capt in movs_or_capts:
				if mov_or_capt != None and len(mov_or_capt) != 0:
					can_exit_jaque = True

		if not can_exit_jaque:
			ganador = int(not jug_actual)
			print(f"Jaque mate, ha ganado el jugador {ganador}")

	##Console Output
	if cons_output:
		fichas_cons = ["P","T","C","A","R","D"]
		colores_cons = ["b","n"]

		out = []

		for x in range(8):
			out.append([])
			for y in range(8):
				out[x].append("0")

		for ncolumna, columna in enumerate(cuadricula2D):
			for nfila, cuad in enumerate(columna):
				if cuad.ficha != None:
					out[nfila][ncolumna] = f"{fichas_cons[cuad.ficha.tipo]}{colores_cons[cuad.ficha.jugador]} "
				else:
					out[nfila][ncolumna]  = "-  "
		
		print("+-----------------------------+")
		print("| | A  B  C  D  E  F  G  H  | |")
		print("|-|-------------------------|-|")
		for nfila, fila in enumerate(out[::-1]):
			sys.stdout.write(f"|{nfila+1}| ")
			for cuad in fila:
				sys.stdout.write(str(cuad))
			sys.stdout.write(f"|{nfila+1}|")
			sys.stdout.write("\n")
		sys.stdout.flush()
		print("|-|-------------------------|-|")
		print("| | A  B  C  D  E  F  G  H  | |")
		print("+-----------------------------+")
def terminar_turno():
	global jug_actual, ficha_sel, ficha_sel_movs, ficha_sel_capts, cdominadas, jaque_cuad, all_movs_capts
	ficha_sel = None
	ficha_sel_movs = ficha_sel_capts = []
	all_movs_capts = [[],[]]

	jug_actual = not jug_actual
	print(f"\nTurno del jugador {int(jug_actual)} \n")

	preparar_turno()

##Crear Cuadrícula
mod = 0
for x in range(1, cantv_corr-1):
	
	mod = not mod
	
	for y in range(1, canth_corr-1):
		rect = pygame.Rect((x*tamh,y*tamv), (tamh, tamv))
		color = colores[(y+mod)%2]
		
		cuadrado = Cuadro((x, 9-y), f"[{abecedario[x-1]}{-(y-9)}]", rect, color)
		
		cuadricula2D[x-1][(9-y)-1] = cuadrado
		cuadricula1D.append(cuadrado)

##Crear Fichas
for jug in [0,1]:
	for i in range(0,8):
		colocar_ficha(Ficha(0,jug), cuadricula2D[i][1+5*jug])
	for i in range(1,4):
		colocar_ficha(Ficha(i,jug), cuadricula2D[i-1][7*jug])
		colocar_ficha(Ficha(i,jug), cuadricula2D[-i][7*jug])
	colocar_ficha(Ficha(4,jug), cuadricula2D[4-1][7*jug])
	colocar_ficha(Ficha(5,jug), cuadricula2D[5-1][7*jug])

##Comenzar primer turno
preparar_turno()

##Bucle Pincipal
while True:	
	#Actualizar la imagen
	dibujar_tablero()

	#Añadir un colisionador al mouse
	mouse_pos = pygame.mouse.get_pos()
	mouse_col = pygame.Rect((mouse_pos[0], mouse_pos[1]), (1, 1))

	#Controlador de eventos de pygame
	for event in pygame.event.get():
		
		#Cerrar el juego
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()

		#Realizar Jugada
		if event.type == pygame.MOUSEBUTTONDOWN:
			for cuad in cuadricula1D:
				if mouse_col.colliderect(cuad.rect) and pygame.mouse.get_pressed()[0]:
					
					#Seleccionar ficha
					if cuad.ficha != None and jug_actual == cuad.ficha.jugador:
						ficha_sel = cuad.ficha
						ficha_sel_movs = all_movs_capts[0][fichas.index(ficha_sel)]
						ficha_sel_capts = all_movs_capts[1][fichas.index(ficha_sel)]

					#Mover Ficha
					elif ficha_sel != None:
						
						if ficha_sel_movs.count(cuad) == 1:
							mover_ficha(ficha_sel, cuad)
							terminar_turno()
						elif ficha_sel_capts.count(cuad) == 1 and cuad.ficha.tipo != 4:
							borrar_ficha(cuad.ficha)
							mover_ficha(ficha_sel, cuad)
							terminar_turno()

						