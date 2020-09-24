# -*- coding: utf-8 -*-
"""
Created on Tue Mar 31 11:43:05 2020

@author: LukasNew
"""
import cv2
import numpy as np
#kernel = np.ones((2,2),np.uint8)
#kernel = np.ones((3,3),np.uint8)
img = cv2.imread('results/_just2shah_room.hdr',-1)
kernel	=	cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))# ksize[, anchor]	)


closing_hdr = cv2.morphologyEx(img, cv2.MORPH_CLOSE, kernel)

blackhat = cv2.morphologyEx(img, cv2.MORPH_BLACKHAT, kernel)

cv2.imshow('umu2',blackhat)
cv2.imshow('umu',closing_hdr)