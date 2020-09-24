# PYTHON Codes for the Image Inpainting Problem
#
# Authors:
# Simone Parisotto          (email: sp751 at cam dot ac dot uk)
# Carola-Bibiane Schoenlieb (email: cbs31 at cam dot ac dot uk)
#      
# Address:
# Cambridge Image Analysis
# Centre for Mathematical Sciences
# Wilberforce Road
# CB3 0WA, Cambridge, United Kingdom
#  
# Date:
# October, 2018
#
# Licence: BSD-3-Clause (https://opensource.org/licenses/BSD-3-Clause)
#

from __future__ import division

import sys
sys.path.append("./lib")

import matplotlib.image as mpimg
import inpainting
import cv2
import numpy as np
### AMLE Inpainting

# create the corrupted image with the mask
img = cv2.imread('pRekonstruiert_hdr.hdr',-1)

maski = cv2.imread('151-comp-noright-noblend498-mask.hdr',-1)
#black = np.array([0,0,0])
#almostblack = np.array([100,100,100])
#kernel = np.ones((2,2),np.uint8)
kernel	=	cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
maski = cv2.morphologyEx(maski, cv2.MORPH_ERODE, kernel)
#maski = cv2.inRange(closing_hdr,black,almostblack );
# parameters
maxiter   = 1; 
tol       = 1e-14;
fidelity  = 10^9;   # weight on data fidelity (should usually be large).
alpha     = 1;      # regularisation parameters \alpha.
gamma     = 0.5;    # regularisation parameters \gamma.
epsilon   = 0.05;   # accuracy of Ambrosio-Tortorelli approximation of the edge set.

# inpainting
u = inpainting.mumford_shah(img,maski,maxiter,tol,fidelity,alpha,gamma,epsilon);
mpimg.imsave("./results/mumford_shah_output.png", u)
img = cv2.imread('156-comp-noright-noblend.hdr',-1)
maski = cv2.imread('156-comp-noright-noblend-mask.hdr',-1)
maski = cv2.morphologyEx(maski, cv2.MORPH_ERODE, kernel)
u = inpainting.mumford_shah(img,maski,maxiter,tol,fidelity,alpha,gamma,epsilon);



fidelity      = 10;
tol           = 1e-5;
maxiter       = 500;
dt            = 0.1;

# inpainting
u = inpainting.amle(img,maski,fidelity,tol,maxiter,dt)


cleanfilename = './dataset/amle_clean.png';
maskfilename  = './dataset/amle_mask.png';
input,mask    = inpainting.create_image_and_mask(cleanfilename,maskfilename);
mpimg.imsave("./dataset/amle_input.png", input[:,:,0], cmap="gray")

# parameters
fidelity      = 10^2; 
tol           = 1e-8;
maxiter       = 40000;
dt            = 0.01;

# inpainting
u = inpainting.amle(input,mask,fidelity,tol,maxiter,dt)


### Harmonic Inpainting

# create the corrupted image with the mask
cleanfilename = './dataset/harmonic_clean.png';
maskfilename  = './dataset/harmonic_mask.png';
input,mask    = inpainting.create_image_and_mask(cleanfilename,maskfilename);
mpimg.imsave("./dataset/harmonic_input.png", input)

# parameters
fidelity      = 10;
tol           = 1e-5;
maxiter       = 500;
dt            = 0.1;

# inpainting
u = inpainting.harmonic(input,mask,fidelity,tol,maxiter,dt)

### Mumford-Shah Inpainting

# create the corrupted image with the mask
cleanfilename = './dataset/mumford_shah_clean.png';
maskfilename  = './dataset/mumford_shah_mask.png';
input,mask    = inpainting.create_image_and_mask(cleanfilename,maskfilename);
mpimg.imsave("./dataset/mumford_shah_input.png", input)

# parameters
maxiter   = 20; 
tol       = 1e-14;
fidelity  = 10^9;   # weight on data fidelity (should usually be large).
alpha     = 1;      # regularisation parameters \alpha.
gamma     = 0.5;    # regularisation parameters \gamma.
epsilon   = 0.05;   # accuracy of Ambrosio-Tortorelli approximation of the edge set.

# inpainting
u = inpainting.mumford_shah(input,mask,maxiter,tol,fidelity,alpha,gamma,epsilon);

### Cahn-Hilliard Inpainting

# create the corrupted image with the mask
cleanfilename = './dataset/cahn_hilliard_clean.png';
maskfilename  = './dataset/cahn_hilliard_mask.png';
input,mask    = inpainting.create_image_and_mask(cleanfilename,maskfilename);
mpimg.imsave("./dataset/cahn_hilliard_input.png", input)

# parameters
maxiter  = 4000
fidelity = 10
dt       = 1

# inpainting
u = inpainting.cahn_hilliard(input,mask,maxiter,fidelity,dt)

### Transport Inpainting

# create the corrupted image with the mask
cleanfilename = './dataset/transport_clean.png';
maskfilename  = './dataset/transport_mask.png';
input,mask    = inpainting.create_image_and_mask(cleanfilename,maskfilename);
mpimg.imsave("./dataset/transport_input.png", input)

# parameters
maxiter          = 50;
tol              = 1e-5;
dt               = 0.1;
iter_inpainting  = 40; # number of steps of the inpainting procedure;
iter_anisotropic = 2;  # number of steps of the anisotropic diffusion;
epsilon          = 1e-10;

# inpainting
u = inpainting.transport(input,mask,maxiter,tol,dt,iter_inpainting,iter_anisotropic,epsilon)
