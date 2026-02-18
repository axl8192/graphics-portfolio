"""

Nota: No arrastrar la ventana mientras el péndulo está en movimiento, ya que altera el dt al 
pausarse el pendulo.update(), pero no el timer.

"""

import pygame, sys
from pygame import gfxdraw
import math
pygame.init()

ventx, venty = 800, 800
vent = pygame.display.set_mode((ventx, venty))
clock = pygame.time.Clock()
caption = "Simple"

class Pendulum:
	def __init__(self, pivot_pos, l, angle, w, r, air_res=0.01, g=9.8):
		
		#Main Vars
		self.pivot_pos = pivot_pos #Pivot Position [dm,dm]
		self.pivot_x, self.pivot_y = pivot_pos #Pivot x,y [dm,dm]
		self.l = l #String Length [dm] (1dm = 1px)
		self.ball_pos = (pivot_pos[0], pivot_pos[1]+l) #Ball Position [dm,dm]
		self.angle = angle #Angle [rad]
		self.w = w #Angular Velocity [rad/s]
		self.ti = 0 #Initial Time [sec]
		self.r = r #Ball Radius [dm]

		#Enviromental Vars
		self.g = g #Gravity acceleration (module) [m/s^2]
		self.air_res = air_res #Air resistance coefficient [no units]

		#Dragging Vars
		self.dragging = False
		self.ddx = 0
		self.ddy = 0

	def update(self):
		mx,my = pygame.mouse.get_pos()
		if not self.dragging:
			if (mx-self.ball_pos[0])**2+(my-self.ball_pos[1])**2<=self.r**2 and pygame.mouse.get_pressed()[0]:
				self.dragging = True
				self.w = 0
				self.ddx = mx-self.ball_pos[0]
				self.ddy = my-self.ball_pos[1]
			else:
				tf = pygame.time.get_ticks()
				dt = (tf - self.ti)/1000
				self.ti = tf

				self.w += (-self.air_res*self.w + (-self.g/(self.l/100))*math.sin(self.angle))*dt
				self.angle += self.w*dt
				
		else:
			if pygame.mouse.get_pressed()[0]:
				self.angle = math.atan2(mx-self.ddx-self.pivot_x, my-self.ddy-self.pivot_y)
			else:
				self.dragging = False
				self.ti = pygame.time.get_ticks()
		
		self.ball_pos = (int(self.pivot_x+self.l*math.sin(self.angle)), int(self.pivot_y+self.l*math.cos(self.angle)))
		pygame.draw.line(vent, (255,255,255), self.pivot_pos, self.ball_pos)
		gfxdraw.aacircle(vent, self.ball_pos[0], self.ball_pos[1], self.r, (255,255,255))

pendulum = Pendulum((400,400),100, math.pi+0.01,0,20)
while True:
	vent.fill((0,0,0))

	for event in pygame.event.get():
		if  event.type == pygame.QUIT:
			pygame.quit()
			sys.exit()

	pendulum.update()
	
	clock.tick()
	pygame.display.set_caption(caption + f" - {int(clock.get_fps())} fps")
	pygame.display.update()