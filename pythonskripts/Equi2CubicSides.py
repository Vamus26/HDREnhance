#----------------------------------------------------------------------------------------------------------
#
# takes an equirectangular image and converts this to a cube map image of the following format
#
#	+----+----+----+
#	| Y+ | X+ | Y- |
#	+----+----+----+
#	| X- | Z- | Z+ |
#	+----+----+----+
#
# which when unfolded should take the following format
#
#	+----+
#	| Z+ |
#	+----+----+----+----+
#	| X+ | Y- | X- | Y+ |
#	+----+----+----+----+
#	| Z- |
#	+----+
#
# Paul Reed
# 06/04/2017
#
# and from :https://raw.githubusercontent.com/dankex/compv/master/3d-graphics/skybox/cubemap-cut.py
#-------------------------------------------------------------------------------------------------------

from __future__ import print_function
from PIL import Image	# Python Imaging Library
import math				# Maths functions

infile_name = "ldrtarget-00001.png"
image = Image.open(infile_name)
inputWidth, inputHeight = image.size;
sqr = inputWidth/4.0
outputWidth = sqr*3
outputHeight = sqr*2
output = []

def getTheta(x,y):
	rtn = 0
	if(y<0):
		rtn =  math.atan2(y,x)*-1
	else:
		rtn = math.pi+(math.pi-math.atan2(y,x))
	return rtn

for loopY in range(1,int(outputHeight+1)):		# 1..height inclusive
	for loopX in range(1,int(outputWidth+1)):	# 1..width inclusive
		tx = 0
		ty = 0
		x = 0;
		y = 0;
		z = 0;
	
		if(loopY<sqr+1): # top half
			if(loopX<sqr+1): 		# top left box [Y+]
				tx = loopX
				ty = loopY
				x = tx-0.5*sqr
				y = 0.5*sqr
				z = ty-0.5*sqr
			elif(loopX<2*sqr+1):	# top middle [X+]
				tx = loopX-sqr
				ty = loopY
				x = 0.5*sqr
				y = (tx-0.5*sqr)*-1
				z = ty-0.5*sqr
			else:					# top right [Y-]
				tx = loopX-sqr*2
				ty = loopY
				x = (tx-0.5*sqr)*-1
				y = -0.5*sqr
				z = ty-0.5*sqr
		else:	# bottom half
			if(loopX<sqr+1): 		# bottom left box [X-]
				tx = loopX
				ty = loopY - sqr
				x = int(-0.5*sqr)
				y = int(tx-0.5*sqr)
				z = int(ty-0.5*sqr)
			elif(loopX<2*sqr+1):	# bottom middle [Z-]
				tx = loopX-sqr
				ty = loopY-sqr
				x = (ty-0.5*sqr)*-1
				y = (tx-0.5*sqr)*-1
				z = 0.5*sqr			# was -0.5 might be due to phi being reversed
			else:					# bottom right [Z+]
				tx = loopX-sqr*2
				ty = loopY - sqr
				x = ty-0.5*sqr
				y = (tx-0.5*sqr)*-1
				z = -0.5*sqr 		# was +0.5 might be due to phi being reversed
					
		# now find out the polar coordinates
		rho = math.sqrt(x*x+y*y+z*z)
		normTheta = getTheta(x,y)/(2*math.pi)			# /(2*math.pi) normalise theta
		normPhi = (math.pi-math.acos(z/rho))/math.pi	# /math.pi normalise phi
		# use this for coordinates 
		iX = normTheta*inputWidth
		iY = normPhi*inputHeight
		# catch possible overflows
		if(iX>=inputWidth):
			iX=iX-(inputWidth)
		if(iY>=inputHeight):
			iY=iY-(inputHeight)	
		output.append(image.getpixel((int(iX),int(iY))))
		
outputImage = Image.new("RGB",((int(outputWidth)),(int(outputHeight))), None)
outputImage.putdata(output)

#Split Cubemap to single sides
name_map = [ \
     ["right", "back", "left"],
     ["front", "bot", "top"]]

try:
    #im = Image.open(infile)
    im= outputImage
    width, height = im.size
    cube_size = width / 3
    
    for row in range(2):
        for col in range(3):
            if name_map[row][col] != "":
                sx = cube_size * col
                sy = cube_size * row
                cubeside=im.crop((sx, sy, sx + cube_size, sy + cube_size))
                filename = "_%s.png" % (name_map[row][col])
                #cubeside.save("pngsides/"+infile_name.split('.')[0]+filename)
                cubeside.save(infile_name.split('.')[0]+filename)

except IOError:
    pass


		
