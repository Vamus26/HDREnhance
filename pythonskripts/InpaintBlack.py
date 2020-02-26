# -*- coding: utf-8 -*-
"""
Created on Fri Feb 14 16:22:24 2020

@author: LukasNew
"""

import numpy as np
import cv2
img  = cv2.imread('17_02/FIX/closed_combined_target_original_top.png')# queryImage

#mask = cv2.imread('cubtopano/best290/combined_best.jpg',0)
black = np.array([0,0,0])
almostblack = np.array([5,5,5])
mask = cv2.inRange(img,black,almostblack );
res = cv2.bitwise_and(img,img, mask= mask) 
#mask.setTo(255, img == 0);
cv2.imshow('mask',mask) 
cv2.imshow('res',res) 
dst = cv2.inpaint(img,mask,15,cv2.INPAINT_TELEA)
cv2.imshow('dst',dst)
dst2 = cv2.inpaint(img,mask,15,cv2.INPAINT_NS)
cv2.imshow('dst2',dst2)
cv2.imwrite('17_02/FIX/inpaintedTELEA_35_05_closed.png', dst)
cv2.imwrite('17_02/FIX/inpaintedNS_35_05_closed.png', dst2)
