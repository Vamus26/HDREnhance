# -*- coding: utf-8 -*-
"""
Created on Thu Apr  9 10:31:46 2020

@author: LukasNew
"""
from __future__ import division

import sys
sys.path.append("./lib")

import matplotlib.image as mpimg
import inpainting
import cv2

# load the image with the mask
img = cv2.imread('Outdoor0003.hdr',-1)
maski = cv2.imread('Outdoor0001m.hdr',-1)
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
maski = cv2.morphologyEx(maski, cv2.MORPH_CLOSE, kernel)
#maski = cv2.morphologyEx(maski, cv2.MORPH_ERODE, kernel)
# parameters
maxiter   = 1; 
tol       = 1e-14;
fidelity  = 10^9;   # weight on data fidelity (should usually be large).
alpha     = 1;      # regularisation parameters \alpha.
gamma     = 0.5;    # regularisation parameters \gamma.
epsilon   = 0.05;   # accuracy of Ambrosio-Tortorelli approximation of the edge set.

# inpainting
u = inpainting.mumford_shah(img,maski,maxiter,tol,fidelity,alpha,gamma,epsilon);
cv2.imwrite("./results/Outdoor0001filled.hdr",u)
mpimg.imsave("./results/Outdoor0001filled.png", u)
#img = cv2.imread('156-comp-noright-noblend.hdr',-1)
#maski = cv2.imread('156-comp-noright-noblend-mask.hdr',-1)
#maski = cv2.morphologyEx(maski, cv2.MORPH_ERODE, kernel)
#u = inpainting.mumford_shah(img,maski,maxiter,tol,fidelity,alpha,gamma,epsilon);
