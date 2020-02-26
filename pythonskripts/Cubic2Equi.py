# -*- coding: utf-8 -*-
"""
Created on Thu Feb 20 11:55:17 2020

@author: LukasNew
from :https://github.com/PaulMakesStuff/Cubemaps-Equirectangular-DualFishEye
"""
from PIL import Image	# Python Imaging Library
import math				# Maths functions
import cv2

#back = image.open("targetsam_101_0286_back.png")
#front = image.open("targetsam_101_0286_front.png")
#right = image.open("targetsam_101_0286_right.png")
#left = image.open("targetsam_101_0286_left.png")
#top = image.open("targetsam_101_0286_top.png")
#bot = image.open("targetsam_101_0286_bot.png")
back = Image.open("pngresults/back.png")
front = Image.open("pngresults/front.png")
right = Image.open("pngresults/right.png")
left = Image.open("pngresults/left.png")
top = Image.open("pngresults/ldr_target-00001_top.png")
bot = Image.open("pngresults/bot.png")

squareLength = back.size[0]
halfSquareLength = squareLength/2
outputWidth = squareLength*4
outputHeight = squareLength*2
output = []

def unit3DToUnit2D(x,y,z,faceIndex):
	if(faceIndex=="X+"):
		x2D = y+0.5
		y2D = z+0.5
	elif(faceIndex=="Y+"):
		x2D = (x*-1)+0.5
		y2D = z+0.5
	elif(faceIndex=="X-"):
		x2D = (y*-1)+0.5
		y2D = z+0.5
	elif(faceIndex=="Y-"):
		x2D = x+0.5
		y2D = z+0.5
	elif(faceIndex=="Z+"):
		x2D = y+0.5
		y2D = (x*-1)+0.5
	else:
		x2D = y+0.5
		y2D = x+0.5	
	# need to do this as image.getPixel takes pixels from the top left corner.
	y2D = 1-y2D
	
	return (x2D,y2D)

def projectX(theta,phi,sign):
	x = sign*0.5
	faceIndex = "X+" if sign==1 else "X-"
	rho = float(x)/(math.cos(theta)*math.sin(phi))
	y = rho*math.sin(theta)*math.sin(phi)
	z = rho*math.cos(phi)
	return (x,y,z,faceIndex)
	
def projectY(theta,phi,sign):
	y = sign*0.5
	faceIndex = "Y+" if sign==1 else "Y-"
	rho = float(y)/(math.sin(theta)*math.sin(phi))
	x = rho*math.cos(theta)*math.sin(phi)
	z = rho*math.cos(phi)
	return (x,y,z,faceIndex)
	
def projectZ(theta,phi,sign):
	z = sign*0.5
	faceIndex = "Z+" if sign==1 else "Z-"
	rho = float(z)/math.cos(phi)
	x = rho*math.cos(theta)*math.sin(phi)
	y = rho*math.sin(theta)*math.sin(phi)
	return (x,y,z,faceIndex)
	
def getColour(x,y,index):
	if(index=="X+"):
		return back.getpixel((x,y))
	elif(index=="X-"):
		return front.getpixel((x,y))
	elif(index=="Y+"):
		return left.getpixel((x,y))
	elif(index=="Y-"):
		return right.getpixel((x,y))
	elif(index=="Z+"):
		return top.getpixel((x,y))
	elif(index=="Z-"):
		return bot.getpixel((x,y))
	
def convertEquirectUVtoUnit2D(theta,phi):
	# calculate the unit vector
	x = math.cos(theta)*math.sin(phi)
	y = math.sin(theta)*math.sin(phi)
	z = math.cos(phi)
	
	# find the maximum value in the unit vector
	maximum = max(abs(x),abs(y),abs(z))
	xx = x/maximum
	yy = y/maximum
	zz = z/maximum
	
	# project ray to cube surface
	if(xx==1 or xx==-1):
		(x,y,z, faceIndex) = projectX(theta,phi,xx)
	elif(yy==1 or yy==-1):
		(x,y,z, faceIndex) = projectY(theta,phi,yy)
	else:
		(x,y,z, faceIndex) = projectZ(theta,phi,zz)
	
	(x,y) = unit3DToUnit2D(x,y,z,faceIndex)
	x*=squareLength
	y*=squareLength
	x = int(x)
	y = int(y)

	return {"index":faceIndex,"x":x,"y":y}
	
# 1. loop through all of the pixels in the output image
for loopY in range(0,int(outputHeight)):		# 0..height-1 inclusive
    for loopX in range(0,int(outputWidth)):
        # 2. get the normalised u,v coordinates for the current pixel
        U = float(loopX)/(outputWidth-1)		# 0..1
        V = float(loopY)/(outputHeight-1)		# no need for 1-... as the image output needs to start from the top anyway.		
		# 3. taking the normalised cartesian coordinates calculate the polar coordinate for the current pixel
        theta = U*2*math.pi
        phi = V*math.pi
        # 4. calculate the 3D cartesian coordinate which has been projected to a cubes face
        cart = convertEquirectUVtoUnit2D(theta,phi)
        # 5. use this pixel to extract the colour
        output.append(getColour(cart["x"],cart["y"],cart["index"]))
		
 # 6. write the output array to a new image file
outputImage = Image.new("RGB",((int(outputWidth)),(int(outputHeight))), None)
outputImage.putdata(output)
outputImage.save("pngresults/EQUI_from_Cubicpythontopright.png")