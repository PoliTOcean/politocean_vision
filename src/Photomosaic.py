import numpy as np, cv2

#Read images

img1 = cv2.imread('Path', 1) 
img2 = cv2.imread('Path', 1)
img3 = cv2.imread('Path', 1) #This image wil be placed on the second image.
img4 = cv2.imread('Path', 1)
img5 = cv2.imread('Path', 1)

imgVett=[img1,img2,img4,img5,img3]

#Saving the size of the images
h, w = img1.shape[:2] 

vis = np.zeros((2*h, 4*w, 3), np.uint8)

#Create the photomosaic
for i in range (4):
    vis[h:2*h, i*w:(i+1)*w] = imgVett[i]
    i+=1

vis[:h, w:2*w] = img3 #Add the img4 on the top of the img2

cv2.imshow("Photomosaic", vis) 
cv2.waitKey()
