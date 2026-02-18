import pygame, sys
pygame.init()
"""
Resolutor gráfico de las n torres de hanoi
"""

ndisks = 10
D1 = [ndisks-i for i in range(ndisks)]
D2, D3 = [], []

ventx, venty = 800,800
vent = pygame.display.set_mode((ventx,venty))

pegy_inf = venty//2 + 200
pegy_sup = 300

hdisk = max((pegy_inf-pegy_sup)//(2*ndisks), 5)

wdisk_min,wdisk_max = 50,200

pause = True
mov = -1

font = pygame.font.Font(None, 50)

def get_input():
	global pause
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()
		if event.type == pygame.KEYDOWN:
			if event.key == pygame.K_SPACE:
				pause=not pause

def draw_hanoi():
	global pause, mov
	mov += 1
	
	vent.fill((0,0,0))
	for pegnum,peg in enumerate([D1,D2,D3]):
		pegx = (pegnum+1)*ventx//4
		pygame.draw.line(vent,(255,255,255),(pegx,pegy_inf),(pegx,pegy_sup))

		for nposd,d in enumerate(peg):
			wdisk = (d-1)*(wdisk_max-wdisk_min)//(ndisks-1) + wdisk_min
			x = pegx - wdisk//2
			y = pegy_inf - hdisk*(nposd+1) + 1
			color = (d-1)*255//(ndisks-1)
			pygame.draw.rect(vent,(0,255,color),(x,y,wdisk,hdisk))
			pygame.draw.rect(vent,(155,61,0),(x,y,wdisk,hdisk),1)
	vent.blit(font.render(f"{mov}",True,(255,255,255)), (30,30))
	get_input()
	
	while pause==True:
		get_input()
		pygame.draw.rect(vent,(255,255,255),(ventx-60,20,20,50))
		pygame.draw.rect(vent,(255,255,255),(ventx-30,20,20,50))
		pygame.display.update()

	pygame.display.update()
	pygame.time.wait(10)

def hanoi(n, origen, destino, auxiliar):
	if n>0:
		hanoi(n-1,origen,auxiliar,destino)
		destino.append(origen.pop())
		draw_hanoi()
		hanoi(n-1,auxiliar,destino,origen)

draw_hanoi()
hanoi(ndisks,D1,D3,D2)
while True:
	get_input()
