import pygame, sys, math
import pygame._camera_vidcapture as pyvc

from numba import cuda
import numpy as np

pygame.init()
pyvc.init()
camera = pyvc.Camera()
ventx, venty = 640, 480
vent = pygame.display.set_mode((ventx, venty))
clock = pygame.time.Clock()

photo_surf = None

@cuda.jit(device=True)
def get_gpu_pos():
	pos = cuda.blockIdx.x*cuda.blockDim.x + cuda.threadIdx.x
	x = pos%ventx
	y = pos//ventx
	return x,y

@cuda.jit
def edges(inp, out):
	x,y = get_gpu_pos()
	
	w = (-1,-1,-1, -1,8,-1, -1,-1,-1) #Weights
	p = 0
	if x!=0 and x!=ventx-1 and y!=0 and y!=venty-1:
		for dy in [-1,0,1]:
			for dx in [-1,0,1]:
					srgb = inp[x+dx, y+dy]
					out[x, y, 0]+=srgb[0]*w[p]
					out[x, y, 1]+=srgb[1]*w[p]
					out[x, y, 2]+=srgb[2]*w[p]
					p+=1
	out[x, y, 0] = max(0, out[x, y, 0])
	out[x, y, 1] = max(0, out[x, y, 1])
	out[x, y, 2] = max(0, out[x, y, 2])
	
@cuda.jit
def gaussian_blur(inp, out):
	x,y = get_gpu_pos()

	w = (16,8,16, 8,4,8, 16,8,16) #Weights
	pos = 0
	if x!=0 and x!=ventx-1 and y!=0 and y!=venty-1:
		for dy in [-1,0,1]:
			for dx in [-1,0,1]:
					srgb = inp[x+dx, y+dy]
					out[x, y, 0]+=srgb[0]//w[pos]
					out[x, y, 1]+=srgb[1]//w[pos]
					out[x, y, 2]+=srgb[2]//w[pos]
					pos+=1

@cuda.jit
def black_and_white(inp, out):
	x,y = get_gpu_pos()

	media = (inp[x,y,0]+inp[x,y,1]+inp[x,y,2])/3
	
	if media>30:
		out[x,y,0] = 255
		out[x,y,1] = 255
		out[x,y,2] = 255
	else:
		out[x,y,0] = 0
		out[x,y,1] = 0
		out[x,y,2] = 0

@cuda.jit
def bnw_denoise(inp, out):
	x,y = get_gpu_pos()

	noise = True
	if x!=0 and x!=ventx-1 and y!=0 and y!=venty-1:
		for dy in [-1,0,1]:
			for dx in [-1,0,1]:
				if (dx!=0 or dy!=0) and inp[x+dx,y+dy,0] == 255:
					noise=False
					break

	if noise:
		out[x,y,0] = 0
		out[x,y,1] = 0
		out[x,y,2] = 0

while photo_surf==None:
	vent.fill((0,0,0))
	
	s = camera.get_image()

	effect_funcs = (edges,black_and_white,bnw_denoise)

	inp = cuda.to_device(pygame.surfarray.array3d(s))
	out = cuda.device_array(shape=(ventx,venty,3), dtype=np.int16)
	for f in effect_funcs:
		f[ventx, venty](inp, out)
		inp = out
		
	out = out.copy_to_host()

	pygame.surfarray.blit_array(vent, out)

	for event in pygame.event.get():
		if  event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()
		if event.type == pygame.KEYDOWN and event.key==pygame.K_SPACE:
			photo_surf = vent

	clock.tick(0)
	pygame.display.set_caption(f" - {int(clock.get_fps())} fps")
	pygame.display.update()