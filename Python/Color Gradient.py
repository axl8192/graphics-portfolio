import pygame, sys
pygame.init()

ventx, venty = 800,800
vent = pygame.display.set_mode((ventx,venty))
mousex, mousey = pygame.mouse.get_pos()

clock = pygame.time.Clock()
caption = "Color Grad"

fnt = pygame.font.Font(None, 30)

class Slider:
	def __init__(self, pos, width, min_value, max_value, r=10):
		self.pos = pos
		self.x = pos[0]
		self.y = pos[1]
		self.width = width
		self.blobx = self.x
		self.blobr = r
		self.dragging = False
		self.min_value = min_value
		self.max_value = max_value
		self.value = self.min_value

	def update(self, color, event_list):
		pygame.draw.line(vent, color, (self.x, self.y), (self.x+(self.width-1), self.y))
		pygame.draw.circle(vent, color, (self.blobx, self.y), 10)
		pygame.draw.circle(vent, (255,255,255), (self.blobx, self.y), 10, 1)

		for event in event_list:
			if event.type==pygame.MOUSEBUTTONDOWN and event.button==1 and (mousex-self.blobx)**2+(mousey-self.y)**2<=self.blobr**2:
				self.dragging = True
			elif event.type==pygame.MOUSEBUTTONUP and event.button==1:
				self.dragging = False

		if self.dragging:
			if mousex>=self.x and mousex<=self.x+(self.width-1):
				self.blobx = mousex
			elif mousex>self.x+(self.width-1):
				self.blobx = self.x+(self.width-1)
			elif mousex<self.x:
				self.blobx = self.x
			self.value = (self.blobx-self.x)/(self.width-1) * (self.max_value-self.min_value) + self.min_value
class CheckBox:
	def __init__(self, pos, size, checked=False):
		self.pos = pos
		self.x = pos[0]
		self.y = pos[1]
		self.size = size
		self.checked = checked

	def update(self, color, eventlist):
		pygame.draw.rect(vent, color, (self.x, self.y, self.size, self.size), 1)

		for event in eventlist:
			if event.type==pygame.MOUSEBUTTONDOWN and event.button==1 and mousex>=self.x and mousex<=self.x+self.size and mousey>=self.y and mousey<=self.y+self.size:
				self.checked = not self.checked
		if self.checked:
			d = self.size//4
			pygame.draw.rect(vent, color, (self.x+d, self.y+d, self.size-2*d, self.size-2*d))

#Inicializar GUI
rs = Slider((20,20),256,0,255)
gs = Slider((20,50),256,0,255)
bs = Slider((20,80),256,0,255)

rchk = CheckBox((330,10), 20)
gchk = CheckBox((330,40), 20)
bchk = CheckBox((330,70), 20)

while True:
	vent.fill((0,0,0))
	mousex, mousey = pygame.mouse.get_pos()
	event_list = pygame.event.get()

	rs.update((255,0,0), event_list)
	gs.update((0,255,0), event_list)
	bs.update((0,0,255), event_list)
	
	rchk.update((255,0,0), event_list)
	gchk.update((0,255,0), event_list)
	bchk.update((0,0,255), event_list)

	vent.blit(fnt.render(f"{int(rs.value)*(not rchk.checked)}", True, (255,0,0)), (290,10))
	vent.blit(fnt.render(f"{int(gs.value)*(not gchk.checked)}", True, (0,255,0)), (290,40))
	vent.blit(fnt.render(f"{int(bs.value)*(not bchk.checked)}", True, (0,0,255)), (290,70))

	lcolors = [rs.value, gs.value, bs.value]
	for chkn, chk in enumerate([rchk, gchk, bchk]):
		if chk.checked:
			lcolors[chkn] = None
	
	xi, yi, yf = 200, 200, 250
	for dx in range(0, 256):
		xcolor = [None, None, None]
		for ncolor, color in enumerate(lcolors):
			if color == None:
				xcolor[ncolor] = dx
			else:
				xcolor[ncolor] = color

		pygame.draw.line(vent, xcolor, (xi+dx,yi), (xi+dx,yf))

	for event in event_list:
		if event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()

	clock.tick(0)
	pygame.display.set_caption(caption + f" - {int(clock.get_fps())} fps")
	pygame.display.update()



