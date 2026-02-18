import pygame, sys
pygame.init()
"""
Implementación del juego de la vida
"""

#--Variables de Ventana--#
ventx, venty = 801, 801
vent = pygame.display.set_mode((ventx, venty))
clock = pygame.time.Clock()
caption = "Life"

#--Variables del Juego--#
width = 100
height = 100

def prepare_cellgrid():
	cg = []
	for x in range(width+2):
		cg.append([])
		for y in range(height+2):
			if x!=0 and x!=width+1 and y!=0 and y!=height+1:
				cg[x].append(0)
			else:
				cg[x].append(0)
	return cg

cellgrid = prepare_cellgrid()

#--Variables de Visualización--#
cellx = ventx//width 
celly = venty//height

cellpix = ventx%width //2 
cellpiy = venty%height //2

rects = []
for x in range(1,width+1):
	rects.append([])
	for y in range(1,height+1):
		rects[x-1].append(pygame.Rect((cellpix+(x-1)*cellx+1,cellpiy+(y-1)*celly+1,cellx-1,celly-1)))

playing = False 
playing_fps = 10

#--Funciones--#
def play_step():
	new_cellgrid = prepare_cellgrid()
	for y in range(1,height+1):
		for x in range(1,width+1):
			ady_cells = 0
			
			for dx in [-1,0,1]:
				for dy in [-1,0,1]:
					if cellgrid[x+dx][y+dy]==1 and (dx!=0 or dy!=0):
						ady_cells += 1

			if cellgrid[x][y]==0 and ady_cells==3:
				new_cellgrid[x][y] = 1
			elif cellgrid[x][y]==1 and ady_cells!=2 and ady_cells!=3:
				new_cellgrid[x][y] = 0
			else:
				new_cellgrid[x][y] = cellgrid[x][y]
	return new_cellgrid

def draw_step():
	if not playing:
		for x in range(0,width+1):
			cellrx = cellpix+x*cellx
			pygame.draw.line(vent, (100,100,100), (cellrx,cellpiy), (cellrx,venty-cellpiy))
		for y in range(height+1):
			cellry = cellpiy+y*celly
			pygame.draw.line(vent, (100,100,100), (cellpix,cellry), (ventx-cellpix,cellry))
		
	for x in range(1,width+1):
		for y in range(1,height+1):
			val = cellgrid[x][y]
			colors = ((0,0,0),(255,255,255))
			pygame.draw.rect(vent, colors[val], rects[x-1][y-1])

def check_edit():
	global cellgrid
	mouse_col = pygame.Rect(pygame.mouse.get_pos(), (1,1))

	for x in range(1,width+1):
		for y in range(1,height+1):
			if rects[x-1][y-1].colliderect(mouse_col):
				cellgrid[x][y] = not cellgrid[x][y]

#--Bucle Principal--#
while True:
	vent.fill((0,0,0))
	
	if playing:
		cellgrid = play_step()
		clock.tick(playing_fps)
	else:
		clock.tick(0)

	draw_step()

	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()
		if event.type == pygame.KEYDOWN and event.key==pygame.K_SPACE:
			playing = not playing
		if not playing and event.type == pygame.MOUSEBUTTONDOWN and pygame.mouse.get_pressed()[0]:
			check_edit()

	pygame.display.set_caption(caption + f" - {int(clock.get_fps())} fps")
	pygame.display.update()