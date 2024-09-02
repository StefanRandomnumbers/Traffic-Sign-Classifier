# Traffic-Sign-Classifier
A computer vision project utilizing various filter techniques to preprocess an image to finally use a Multilayered Perceptron Neuralnetwork to classify the regions in an image that display one of the following german traffic signs: Stoppschild, Vorfahrt, Vorfahrtstraße, Vorfahrt achten. The project was designed to function with a small set of data for the NN, so that good preprocessing is needed to get good results, because the project focuses more on the computer vision aspect. 


## How to use 
Easiest way install is to download the standalone build. 

Once the GUI is running, the first thing to do is to train the Neuralnetwork on the provided traingdata, which consists of 63 preprocessed images of trffic signs and 353 images without a traffic sign. 
Use the menue options under "Verarbeiten" to start and stop the training of the NN. The training takes can be stopped at any time, however it should run for about 1 minute or until the test error displayed in the console window is lower than 0.05.

After the NN is trained it is possible to load any image into the GUI to process it and classify if there are any of the 4 traffic signs present. Loading images can be done via the menue "Datei" -> "Open". Then press F5 or use the menue "Verarbeiten -> "Start". 
The programm will then preprocess the image and display the different steps in the sidebar of the GUI, the different steps can be clicked and the image of the given processing step will be displayed.


## How it works 
The process of classifieing traffic signs in the image has been handcrafted to the specifics of the task. 
First the regions of interests where a traffic sign could be have to be identefied, this is done by the following process:
- filter the image for a specific range of red and yellow color, this will result in a binary image where the red or yellow pixels are white and the rest is black (this was chosen due to the fact that the 4 prossible traffic signs are all either red or yellow)
- morphology operation erode and then dilate, this is also known as an open operation (this will remove any one pixel that is not part of a given pixel mass, so it will remove some noise from the image)
- find all contours in the filtered image, in other words all pixels that are white
- merge the contours that are fully inside the range of another contour
- discard any contours that consists of less than 32x32 pixels

This process will find any regions of interest that are big enough and are in the wanted color range. 
After this the found contours are processed to be ready for the NN classifier:
- floodfill the contrours to be solid mass of white pixels in the binary image and use it for a pixel wise bollean operator AND with the original image (this is done to make sure that no pixels that dont belong to the given contour are inside the regen of interest
- transform the regen of interest to a grey sclae image and resize it to be 32x32 pixels (the NN is set to have 1024 neurons on its input layer, this is exactly the pixel amount of an image that is 32 by 32 pixels)
- feed all the regions of interest that are left into the classifier NN
