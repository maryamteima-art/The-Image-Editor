i. Application Overview (i.e. What are the controls? Why this design?)

The interface is split into four major parts. 
	A) General Controls, which includes basic brightness, contrast and saturation sliders
	B) ColorBalance Control, which contains the UpperTone, LowerTone, R,G,B, and Strength slides
	C) Complex-Stylization Controls, which includes Posterize (or Tritone), Halftone, and Cartoon (Outline & Tinting) filters
	D) File Management, which includes Loading, Saving and Eyedropper tool

General controls involve the basic image editing tools, hence they are grouped together because of simplicity in understanding. 

ColorBalance, had multiple sliders to manipulate from for customizability purposes. Users are given the option to choose the range of Tones (shadows, midtons and highlights) to manipulate, and are given seperate sliders for the RGB levels and strength of the color-grading. If it was combined with "General Controls" it would have confused the users since there would have been too many sliders in the same location for different features.

Complicated filters such as the halftone and posterization enabled users to choose the specific colors thorugh a color wheel which took a lot of space, therefore, it was decided to put these giant interfaces on the far left and far right side of the UI to avoid confusion. The other filters had simpler designs (one call to action), so they were combined together. Titles were used to seperate them from one-another.

File Management was combined with the eyedropper since they deal with users wanting to access the image itself (access the image's colors, and file locations).


REFERENCES:
About The Author Richard Moss The founder of Cyotek. (2017, February 27). Finding nearest colors using euclidean distance. The Cyotek Blog. https://www.cyotek.com/blog/finding-nearest-colors-using-euclidean-distance 
Antoniadis, W. by: P. (2022, November 4). How to compute the similarity of Colours. Baeldung on Computer Science. https://www.baeldung.com/cs/compute-similarity-of-colours 
Antoniadis, W. by: P. (2023, June 19). How to convert an RGB image to a grayscale. Baeldung on Computer Science. https://www.baeldung.com/cs/convert-rgb-to-grayscale 
Grayscale to RGB conversion. Online Tutorials, Courses, and eBooks Library. (n.d.). https://www.tutorialspoint.com/dip/grayscale_to_rgb_conversion.htm 
OFSYSTEMUTILS. openFrameworks. (n.d.-a). https://openframeworks.cc/documentation/utils/ofSystemUtils/#show_ofSystemSaveDialog 
OFSYSTEMUTILS. openFrameworks. (n.d.). https://openframeworks.cc/documentation/utils/ofSystemUtils/#show_ofSystemLoadDialog 

Saxena, S. (2021, March 18). Edge detection: Extracting the edges from an image. Analytics Vidhya. https://www.analyticsvidhya.com/blog/2021/03/edge-detection-extracting-the-edges-from-an-image/#:~:text=We%20can%20compare%20neighboring%20pixel,is%20not%20at%20the%20edge 
Sobel edge detector. Feature Detectors - Sobel Edge Detector. (n.d.). https://homepages.inf.ed.ac.uk/rbf/HIPR2/sobel.htm 
Wikimedia Foundation. (2023a, May 29). Color difference. Wikipedia. https://en.wikipedia.org/wiki/Color_difference 
Wikimedia Foundation. (2023b, September 5). Sobel operator. Wikipedia. https://en.wikipedia.org/wiki/Sobel_operator 
 
