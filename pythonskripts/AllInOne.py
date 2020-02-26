# -*- coding: utf-8 -*-
"""
Created on Mon Feb 24 16:56:31 2020

@author: LukasNew
"""

import numpy as np
import cv2
import glob
import os

#overall settings
sift = cv2.xfeatures2d.SIFT_create()
FLANN_INDEX_KDTREE = 0
index_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
search_params = dict(checks=50)   # or pass empty dictionary
flann = cv2.FlannBasedMatcher(index_params,search_params)
MIN_MATCH_COUNT = 2
MAX_WARP_AMOUNT = 4
#stitcher settings
kernel = np.ones((2,2),np.uint8)
black = np.array([0,0,0])
almostblack = np.array([1,1,1])#check 1,1,1 better black original ares, but more artifacts
almostwhite = np.array([255,255,255]) 

#read sides and store them
all_images_to_compare = []
image_titles = []
for f in glob.iglob("pngsides/*"):
    image = cv2.imread(f)
    image_titles.append(f)
    all_images_to_compare.append(image)   

def createBestMatchesList(targetIm,all_images_to_compare, image_titles):
    kp1, des1 = sift.detectAndCompute(targetIm,None)
    amount_good_points =[]
    images_to_comp_filtered=[]
    image_titles_filtered=[]
    good_points=[]
    
    for image_to_compare, title in zip(all_images_to_compare, image_titles):
        if image_to_compare is None:
            print("image_to_compare is None")
            continue
        if targetIm.shape == image_to_compare.shape:
           des2 =[]
           kp2, des2 = sift.detectAndCompute(image_to_compare, None)
           if  des1 is None:
               print("No features found in des1.")
               break
           if des2 is None:
               print("No features found in: ."+ title)
               continue
           try:
               matches = flann.knnMatch(des1, des2, k=2) 
           except AssertionError:
               pass    
           good_points_tmp = []
           for m, n in matches:
               if m.distance < 0.8*n.distance: # the higher the more features 0.6 pretty good
                   good_points_tmp.append(m)
           print("Good Matches:", len(good_points_tmp))
           number_keypoints = 0
           if len(kp1) >= len(kp2):
               number_keypoints = len(kp1)
           else:
               number_keypoints = len(kp2)
           print("Title: " + title)
           percentage_similarity = len(good_points_tmp) / number_keypoints * 100
           print("Similarity: " + str(int(percentage_similarity)) + "%")
           good_points.append(good_points_tmp)
           amount_good_points.append(len(good_points_tmp))
           images_to_comp_filtered.append(image_to_compare)
           image_titles_filtered.append(title)
        else:
           print("Shape is different") 
    matchList= list(zip(amount_good_points,good_points,images_to_comp_filtered, image_titles_filtered))
    return(sorted(matchList, key=lambda x: x[0],reverse=True))     
    
def calculateHomographyAndWarp(a2,a3,a4,counter,img1):
    if len(a2)>MIN_MATCH_COUNT:
        kp1, des1 = sift.detectAndCompute(img1,None)
        best_kp, des2 = sift.detectAndCompute(a3, None)
        src_pts = np.float32([ best_kp[m.trainIdx].pt for m in a2 ]).reshape(-1,1,2)
        dst_pts = np.float32([ kp1[m.queryIdx].pt for m in a2 ]).reshape(-1,1,2)
        transMat, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC,5.0) 
    else:
        print ("ERROR: Not enough matches are found - %d/%d" % (len(a2),MIN_MATCH_COUNT))

    if len(a2)>MIN_MATCH_COUNT:   
        if transMat is None:
           print ("ERROR: Homography is empty!")
           return()
        else:
            #warp perspective
            im_out = cv2.warpPerspective(a3, transMat, (img1.shape[1], img1.shape[0]))
            name =a4.split('.')[0];
            name = name.split('\\')[1];
            name = "bot/Warped_BEST"+str(counter)+"_"+ name+".png"
            print(name)
            #cv2.imwrite(name, im_out)#uncomment if you want to store the side fragments
            return(im_out)
            
def stitchSides(warpSides,infile_name):
    first_image=[]
    current_stitch=[]
    for image_to_stitch in warpSides:
        if image_to_stitch is None:
            print("image_to_stitch is None")
            continue
        if len(first_image)==0:
            first_image = image_to_stitch
            continue
        if len(current_stitch)==0:
            mask = cv2.inRange(image_to_stitch,almostblack,almostwhite);
            current_stitch = first_image.copy() 
            current_stitch[np.where(mask == 255)] = image_to_stitch[np.where(mask == 255)]
            continue
        
        mask = cv2.inRange(current_stitch,black,almostblack)
        current_stitch_tmp = current_stitch.copy()
        current_stitch_tmp[np.where(mask == 255)] = image_to_stitch[np.where(mask == 255)]
        current_stitch = current_stitch_tmp.copy()
    
    if len(current_stitch)!=0:
        closing = cv2.morphologyEx(current_stitch, cv2.MORPH_CLOSE, kernel)
        cv2.imwrite("pngresults/STITCHED"+infile_name.split('/')[1],closing)
        #cv2.imwrite("pngresults/top.png", closing)
        print("stitch done")
        return(closing)
    return()
    

    
def warpAndStitch(infile_name):
    infile_name = infile_name.replace(os.sep, '/')
    print(infile_name)

    img1 = cv2.imread(infile_name)    # queryImage  
    #find best matching sides to it 
    sortedList = createBestMatchesList(img1,all_images_to_compare, image_titles)
    #warp the top X sides
    warpSides=[]
    counter=1
    for a1,a2,a3,a4 in sortedList:
        if counter<=MAX_WARP_AMOUNT:
            sideTmp= calculateHomographyAndWarp(a2,a3,a4,counter,img1)
            if sideTmp is not None:
                warpSides.append(sideTmp)
            counter= counter+1
        else:
            break
    #Stitch the warped sides to one cube side  
    if len(warpSides)!=0:   
        stitchedAndClosedSide= stitchSides(warpSides,infile_name)
        
#PROGRAM START    
#read image sides
for f in glob.iglob("targetsides/*"):   
    warpAndStitch(f)