import pygame, sys
from numba import cuda
import numpy as np
pygame.init()

ventx, venty = 640, 480
vent = pygame.display.set_mode((ventx, venty))
clock = pygame.time.Clock()
caption = ""

fnt = pygame.font.SysFont("Arial",200)

playing = False

@cuda.jit
def play_step(inp, out):
	pos = cuda.blockIdx.x*cuda.blockDim.x + cuda.threadIdx.x
	x = pos%ventx
	y = pos//ventx
	
	if x!=0 and x!=ventx-1 and y!=0 and y!=venty-1:
		ady_cells = 0
		
		for dx in [-1,0,1]:
			for dy in [-1,0,1]:
				if inp[x+dx, y+dy, 0]==255 and (dx!=0 or dy!=0):
					ady_cells += 1

		if inp[x, y, 0]==0 and ady_cells==3:
			out[x, y, 0] = 255
			out[x, y, 1] = 255
			out[x, y, 2] = 255
		elif inp[x, y, 0]==255 and ady_cells!=2 and ady_cells!=3:
			out[x, y, 0] = 0
			out[x, y, 1] = 0
			out[x, y, 2] = 0
		else:
			out[x, y, 0] = inp[x, y, 0]
			out[x, y, 1] = inp[x, y, 1]
			out[x, y, 2] = inp[x, y, 2]

#--Funciones de preparacion de la superficie inicial--#
def blit_photo():
	import pycam_get_surface
	vent.blit(pycam_get_surface.photo_surf,(0,0))

def blit_text():
	vent.fill((0,0,0))
	a = fnt.render("Axl", 0, (255,255,255))
	vent.blit(a,(ventx//2-a.get_width()//2,venty//2-a.get_height()//2))

#Preparar la superficie inicial
blit_text()

while True:
	if playing:
		inp = cuda.to_device(pygame.surfarray.array3d(vent))
		out = cuda.device_array(shape=(ventx,venty,3), dtype=np.int16)
		play_step[ventx,venty](inp, out)

		out = out.copy_to_host()
		pygame.surfarray.blit_array(vent, out)

	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()
		if event.type == pygame.KEYDOWN and event.key==pygame.K_SPACE:
			playing = not playing
	
	clock.tick()
	pygame.display.set_caption(caption + f" - {int(clock.get_fps())} fps")
	pygame.display.update()