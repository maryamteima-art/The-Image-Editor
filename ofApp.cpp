#include "ofApp.h"
#include <cmath>
//--------------------------------------------------------------
void ofApp::setup(){
	
	//--------- BASIC SETUP ---------
	ofSetFrameRate(60);
	m_origImg.load("image.png");

	//Moving the original image data to edited image to duplicate
	m_edittedImg = m_origImg;

	//edge_mask.allocate(m_edittedImg.getWidth(), m_edittedImg.getHeight(), OF_IMAGE_COLOR);
	//All pixels are set to non edges
	//edge_mask.setColor(ofColor(0, 0, 0)); 

	//--------- GUI SETUP ---------
	m_gui.setup();

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	// --------- DRAWING IMAGES ---------

	//Order of operations, Translate --> Rotate --> Scale

	//Drawing Original Image
	ofPushMatrix(); //ENTER ORIG IMG MATRIX
	ofTranslate(795,0);
	ofScale(0.1);
	m_origImg.draw(0, 0);
	ofPopMatrix(); //EXIT ORIG IMG MATRIX

	//Drawing Edited Image
	ofPushMatrix(); //ENTER EDIT IMG MATRIX
	ofTranslate(300, 200);
	ofScale(0.24);
	m_edittedImg.draw(0, 0);
	ofPopMatrix(); //EXIT EDIT IMG MATRIX



	//--------- EYEDROPPER ------------ 
	if (eyedropper) {
		int mouse_x = ofGetMouseX();
		int mouse_y = ofGetMouseY();

		int editImg_x = 300;
		int editImg_y = 200;
		float editImg_scale = 0.24;

		//Mouse is within image bounds
		//Need to take into account the translation and scaling done in the matrix tranformations
		if (mouse_x >= editImg_x && mouse_x <= editImg_x + m_edittedImg.getWidth() * editImg_scale 
			&& mouse_y >= editImg_y && mouse_y <= editImg_y + m_edittedImg.getHeight() * editImg_scale) {

			//Convert the mouse coordinates back to the screen's coordinates 
			//The translations and scaling was image coordinates (which now have their own shifted-coord system), thus the pixels now take up a different space
			//BUT the mouse requires the screen's coord system, thus we need to revert the changes to mouse_pos so that we can properly obtain the colors that were scaled & moved

			int rel_x = (mouse_x - editImg_x) / editImg_scale;
			int rel_y = (mouse_y - editImg_y) / editImg_scale;

			eyeCol = m_edittedImg.getColor(rel_x, rel_y);

			//Draw the color on Rect
			ofSetColor(eyeCol);

			//Draw the rectangle-preview
			ofDrawRectangle(mouse_x, mouse_y, 100,100);

			//Set window back to white (to avoid tinting the other elements)
			ofSetColor(255, 255, 255);

		}


	}


	// --------- DRAWING GUI ---------


	m_gui.begin();
		//Positioning the Panel (y,x) & Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(795, 400));
		ImGui::SetNextWindowSize(ImVec2(205, 100));

		ImGui::Begin("Load & Save");
			//LOAD IMAGE 
			if (ImGui::Button("Load Image")) {
				loadImg();
			}

			//SAVE IMAGE
			if (ImGui::Button("Save Image")) {
				saveImg();
			}
		ImGui::End();

		//Positioning the Panel (y,x) & Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(795, 300));
		ImGui::SetNextWindowSize(ImVec2(205, 100));


		// ----------- ADDING INPUTS -------------- 
		
		//name of window goes inside begin()
		ImGui::Begin("Eyedropper");
			
			//Button enables eyedropper
			if (ImGui::Button("Activate Eyedropper")) {
				eyedropper = true;
			}

			//Cancel Button
			if (ImGui::Button("Cancel Eyedropper")) {
				eyedropper = false;
				//Ensures mouse doesn't read clicking when canceling
				click = false;
			}
			//Text showcasing obtained color
			if (click && eyedropper) {
				
				//reset clicking & eyedropper
				eyedropper = false;
				click = false;

			}
			
			//Map color to a normalized vector
			ImVec4 eyeColVect(eyeCol.r / 255.0f, eyeCol.g / 255.0f, eyeCol.b / 255.0f, 1.0f);

			//Showcase the color in button
			ImGui::ColorButton("Selected Color", eyeColVect);



		ImGui::End();
	
		//Positioning the Panel (y,x) & Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(300, 100));


		ImGui::Begin("General Image Controls");

		//To access members we use adresses
		//SliderName, membervariable, minval, maxval
		
		//BRIGHTNESS
		if (ImGui::SliderFloat("Brightness", &m_brightness, -255.0f, 255.0f)) {
			Brightness(m_brightness);
		};

		//CONTRAST
		if (ImGui::SliderFloat("Contrast", &m_contrast, -255.0f, 255.0f)) {
			Contrast(m_contrast);
		
		};
		
		//SATURATION
		if (ImGui::SliderFloat("Saturation", &m_saturation, -255.0f, 255.0f)) {
			Saturation(m_saturation);
		
		};


		ImGui::End();


		//Positioning the Panel (y,x) and Sizing (x,y)
		//ImGui::SetNextWindowPos(ImVec2(905, 0));
		//ImGui::SetNextWindowSize(ImVec2(300, 940));

		//OTHER FILTERS

		//Positioning the Panel (y,x) and Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(1300, 0));
		ImGui::SetNextWindowSize(ImVec2(300, 950));
		
		ImGui::Begin("Other Filters");

			////YELLOW-BLUE TINT
				//the -40 - 40 ranges gave the best tinting results without washing out the image. It's simply a tested/experimental value
			ImGui::Text("<-Blue Tint /Yellow Tint->");
			if (ImGui::SliderFloat("<-Y/B->", &m_YBtint, -40.0f, 40.0f)) {
				YellowBlue(m_YBtint);
			};

			//SOBEL FILTER
			ImGui::Text("\n\n		Cartoonify/Outline");
			ImGui::Text("Fine->Broad");
			if (ImGui::SliderFloat(" ", &m_sobstr, 20, 100)) {

				//Save the editted image before applying the outline-effect
				m_presobelImg = m_edittedImg;
			};

			if (ImGui::Button("Apply Outline")) {
				sobel(m_sobstr);
			}

			if (ImGui::Button("Reset Outline")) {
				//Revert the image to its pre-outline state
				m_edittedImg = m_presobelImg;
			}

			//COLOR SPLASH
			ImGui::Text("\n\nColorSplash (aka ColorPop)\n\n**MUST CHOOSE ACCURACY FOR FILTER TO WORK**");

			ImGui::Text("Grayscales Image except chosen color\n\nThe higher the accuracy,\nthe more lenient (broad)\nthe color selection is\n\n");
			//Moving the accuracy activates the filter
			//50-440 was the tested range that chose colors. If wanted 100% exact, change 50 to 30

			ImGui::Text("		Accuracy\n");
			if (ImGui::SliderFloat("Accuracy ", &m_accuracy, 50, 440) ||
				ImGui::ColorPicker3("Color to Pop ", (float*)&m_splashCol)) {

				//Save the editted image before applying the splash-effect
				m_presplashImg = m_edittedImg;
			};
		
			if (ImGui::Button("Apply Splash")) {
				colorsplash(m_splashCol, m_accuracy);
			}


			if (ImGui::Button("Reset Splash")) {
				//Revert the image to its pre-splash state
				m_edittedImg = m_presplashImg;
			}
		ImGui::End();


		//Positioning the Panel (y,x) and Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(1000, 0));
		ImGui::SetNextWindowSize(ImVec2(300, 950));

		
		//COMPLEX-LARGE PANELED FILTERS

		ImGui::Begin("Halftone Filter");

			//HALFTONE FILTER
			ImGui::Text("\nHalftone:\n**MUST APPLY DETAIL FOR EFFECT TO WORK**\n**USE SHADED IMG FOR BEST RESULTS**\n1) Applies circles ontop of Image Tones\n2)Halftone Colors are customizable\n3) Higher Detail = Larger Circles");

			ImGui::Text("\n		Detail");


			//Color Wheels or Slider for Halftone2 filter
			//30-60 was the tested range for halftone with best results
			if (ImGui::SliderFloat(" ", &m_detail, 30.0f, 70.0f) || ImGui::ColorPicker3("Shadow\nCircles", (float*)&m_halfShadCol) || 
				ImGui::ColorPicker3("MidTone\nCircles", (float*)&m_halfMidCol) || ImGui::ColorPicker3("Highlight\nCircles", (float*)&m_halfHighCol)) {
				
				//Save the editted image before applying the halftone
				m_prehalftoneImg = m_edittedImg;
			}

			//Button enables applying the filter
			if (ImGui::Button("Apply Halftone")) {
				halftone2(m_detail, m_halfShadCol, m_halfMidCol, m_halfHighCol);
			}

			//Reset Button
			if (ImGui::Button("Reset Halftone")) {
				//Revert the image to its pre-halftone state
				m_edittedImg = m_prehalftoneImg;
			}



		ImGui::End();


		//Positioning the Panel (y,x) & Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(0, 100));
		ImGui::SetNextWindowSize(ImVec2(300, 950));
		
		//TRITONE FILTER
		ImGui::Begin("Tritone Filter - Posterize");

		
		//Color Wheels for tritone filter
		if (ImGui::ColorPicker3("Shadow", (float*)&m_triShadCol) || ImGui::ColorPicker3("Midtones", (float*)&m_triMidCol) ||
			ImGui::ColorPicker3("Highlights", (float*)&m_triHighCol)) {
			//Save editted image before applying posterize
			m_preposterImg = m_edittedImg;

		}

		if (ImGui::Button("Apply Posterize")) {
			tritone(m_triShadCol, m_triMidCol, m_triHighCol);
		}

		if (ImGui::Button("Reset Posterize")) {
			//Revert the image to its pre-splash state
			m_edittedImg = m_preposterImg;
		}
	

		ImGui::End();


		//COLORBALANCE - If users wanted to grade both, shadows, midtones & highlights, this function can be duplicated three times for customizability on the 3 tones
		
		//Positioning the Panel (y,x) & Sizing (x,y)
		ImGui::SetNextWindowPos(ImVec2(300, 0));
		ImGui::SetNextWindowSize(ImVec2(495, 200));




		ImGui::Begin("Color Balance");
		ImGui::Text("Shadows Tonal Range = 0-85\nMidtones Tonal Range = 86-170\nHighlights Tonal Range = 171-255\n\n");
		
		//If tones, red, green, blue pr strength sliders are touched, apply colorbalance
		if (ImGui::SliderFloat("Starting Tone", &m_lowertone, 0.0f, 255.0f) || ImGui::SliderFloat("Ending Tone", &m_uppertone, 0.0f, 255.0f) ||ImGui::SliderFloat("Red", &m_rIntensity, 0.0f, 255.0f) || ImGui::SliderFloat("Green", &m_gIntensity, 0.0f, 255.0f) || ImGui::SliderFloat("Blue", &m_bIntensity, 0.0f, 255.0f) || ImGui::SliderFloat("Grading Strength", &m_strength, 0.1, 1)) {
			ColBalance(m_rIntensity, m_gIntensity, m_bIntensity, m_lowertone, m_uppertone, m_strength);
		};

		ImGui::End();

	m_gui.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (button == OF_MOUSE_BUTTON_LEFT && eyedropper) {  
		click = true;
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

// -------- CUSTOM FUNCTIONS -------------

//---------- LOADING IMAGE ------------------

void ofApp::loadImg() {
	ofFileDialogResult result = ofSystemLoadDialog("Select an image");
	if (result.bSuccess) {
		m_origImg.load(result.getPath());
		m_edittedImg = m_origImg;
	}
	//If it doesn't load, send error message in terminal
	if (!m_origImg.load(result.getPath()) || !m_edittedImg.load(result.getPath())) {
		ofLogError("Could not load image");
	}
}

//---------- SAVE IMAGE -----------------
void ofApp::saveImg() {
	ofFileDialogResult result = ofSystemSaveDialog("image.png", "Save your image");
	if (result.bSuccess) {
		m_edittedImg.save(result.getPath());
	}
}

//---------- BRIGHTNESS FUNCTION ---------------
void ofApp::Brightness(float intensity) {
	ofColor origCol;
	ofColor newCol;

	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			//Getting the original color
			origCol = m_origImg.getColor(x,y);

			//Storing the obtained colors to newcolor variable with added intesnity being slided
			//The clamp ensures the variable we're changing is in range of 0-255
			newCol.set(ofClamp(origCol.r + intensity, 0.0f, 255), ofClamp(origCol.g + intensity, 0.0f, 255), ofClamp(origCol.b + intensity, 0.0f, 255));

			//Storing the newcol to actual image
			m_edittedImg.setColor(x, y, newCol);
		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();

}


//---------- CONTRAST FUNCTION ---------------
void ofApp::Contrast(float intensity) {
	ofColor origCol;
	ofColor newCol;

	//Contrast Factor Math Formula: (259*(255+colorIntensity)) / (259*(255-colorIntensity)) 
	//(FOR ME TLDR): division on +- ranges

	const float Cfactor = (259 * (255 + intensity)) / (259 * (255 - intensity));

	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			//Getting the original color
			origCol = m_origImg.getColor(x, y);

			//Storing the obtained colors to newcolor variable with added intesnity being slided
			//The clamp ensures the variable we're changing is in range of 0-255

			//general formula for each pixel is:  Cfactor * (pixel.colorchannel-128)+128
			//.colorchannel means red,green,or blue 
			newCol.set(ofClamp(Cfactor * (origCol.r - 128) + 128, 0.0f, 255), ofClamp(Cfactor * (origCol.g - 128) + 128, 0.0f, 255), ofClamp(Cfactor * (origCol.b - 128) + 128, 0.0f, 255));

			//Storing the newcol to actual image
			m_edittedImg.setColor(x, y, newCol);
		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();

}

//------------ SATURATION FUNCTION ----------------
void ofApp::Saturation(float intensity) {
	ofColor origCol;
	ofColor newCol;

	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			//Getting the original color
			origCol = m_origImg.getColor(x, y);

			//Getting saturation = Need to convert rgb to hsv
			float h, s, v;
			///Getting the HSB values and storing them into the h,s,v floats
			origCol.getHsb(h,s,v);

			//Adjusting saturation based on userinput
			s = ofClamp(s+intensity, 0,255);

			//Revert To Rgb space
			ofColor newCol = ofColor::fromHsb(h,s,v);

			//Saving pixels to Image itself
			m_edittedImg.setColor(x,y,newCol);
			}
		}
	//Updates Cpu data to Gpu
	m_edittedImg.update();
	
}

//------------ COLORBALANCE FUNCTION --------------
void ofApp::ColBalance(float r_intensity, float g_intensity, float b_intensity, float lower_tone, float upper_tone, float grade_strength) {
	ofColor origCol;
	ofColor newCol;

	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			//Getting the original color
			origCol = m_origImg.getColor(x, y);
			
			//Getting brightness
			float orig_brightness = origCol.getBrightness();
			
			//If current pixel is in tonal range specified by user, apply grading (strength of grading is specified by user as well)

			if (orig_brightness> lower_tone && orig_brightness<upper_tone) {

				//Storing the obtained colors to newcolor variable with added intesnity being slided
				//The clamp ensures the variable we're changing is in range of 0-255
				//The color intesnity is multiplied with the grade_strength first to ensure image doesn't crunch to black or white extremes
				newCol.set(ofClamp((origCol.r + (r_intensity*grade_strength)), 0.0f, 255), ofClamp((origCol.g + (g_intensity*grade_strength)), 0.0f, 255), ofClamp((origCol.b + (b_intensity*grade_strength)), 0.0f, 255));

				//Storing the newcol to actual image
				m_edittedImg.setColor(x, y, newCol);
			}


		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();

}


//----------- YELLOW BLUE TINTING FUNCTION ------------ 
void ofApp::YellowBlue(float intensity) {
	ofColor origCol;
	ofColor newCol;

	//Contrast Factor Math Formula: (259*(255+colorIntensity)) / (259*(255-colorIntensity)) 
	//(FOR ME TLDR): division on +- ranges

	const float Cfactor = (259*(255+intensity))/(259 * (255 - intensity));

	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			//Getting the original color
			origCol = m_origImg.getColor(x, y);

			//Storing the obtained colors to newcolor variable with added intesnity being slided
			//The clamp ensures the variable we're changing is in range of 0-255

			//general formula for each pixel is:  Cfactor * (pixel-128)+128
			//By multiplying the Cfactor to red and green channels while enabling contrast changes to blue only, we get tinting from blue to yellow
			//Decreasing intensity results in blue tinting, while increasing intensity results in yellow tinting

			newCol.set(ofClamp(Cfactor*(origCol.r), 0.0f, 255), ofClamp(Cfactor*(origCol.g), 0.0f, 255), ofClamp(Cfactor*(origCol.b-128)+128, 0.0f, 255));

			//Storing the newcol to actual image
			m_edittedImg.setColor(x, y, newCol);
		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();

}

//COMIC HALFTONE FILTER - Uses Sobel filter & then manipulates pixels to circles

//SOBEL FILTER
//It looks at the nearby pixel's brightness to determine if the current pixel consitutes as an edge 
//If the change/jump in brightness is high eneough, the algorithm determines its an edge
void ofApp::sobel(float intensity) {
	ofColor origCol;
	ofColor newCol;

	//Loop through Pixels
	//Loop thorugh height

	for (int y = 1; y < m_edittedImg.getHeight() - 1; y++) {
		//Loop through width 
		for (int x = 1; x < m_edittedImg.getWidth() - 1; x++) {

			//Gx vector-compoenent looks into the x-direction of an image (Detects Horizontal lines)
			//Here we get/analyze the pixel's brightness to get the changes in intesnity
			//Gx in Math is this [-1 0 1]
			//					 [-2 0 2]
			//					 [-1 0 1]
			// (the numbers represent their "importance" (or weight) to the gradient selection)
			// In code, the numbers on the outside [- m_orgImg]
			//									   [-2 m_origImg]
			//									   [- m_origImg]
			// 
			//									   [+ m_origImg]
			//									   [+2 m_origImg]
			//									   [+ m_origImg]
			// 
			// represents the first column of Gx [-1]
			//							         [-2]
			//							         [-1]
			// 
			// and the third column of Gx		 [1]
			//									 [2]
			//									 [1]
			//(columns are for horizontal changes in matrices)
			// The numbers within the getcolor brackets represents which pixel we're comparing in relation to the one we're currently in
			// (Ex. x-1,y-1 compares the pixel in the top-left of the current one we're on)
			// Pixels on the left side are subtracted from the pixels on the right side
			float gx = -m_origImg.getColor(x - 1, y - 1).getBrightness() 
					- 2 * m_origImg.getColor(x - 1, y).getBrightness() 
					- m_origImg.getColor(x - 1, y + 1).getBrightness() 
					+ m_origImg.getColor(x + 1, y - 1).getBrightness() 
					+ 2 * m_origImg.getColor(x + 1, y).getBrightness() 
					+ m_origImg.getColor(x + 1, y + 1).getBrightness();

			//Gy vector-compenent looks into the y-direction of an image (Vertical Line Detection) 
			//Gy in Math is this [-1 -2 -1]
			//					 [0   0  0]
			//					 [1   2  1]
			// (the numbers represent the pixel's "importance" (or weight) to the gradient selection)
			// In code, the numbers on the outside [- m_orgImg]
			//									   [-2 m_origImg]
			//									   [- m_origImg]
			// 
			//									   [+ m_origImg]
			//									   [+2 m_origImg]
			//									   [+ m_origImg]
			// 
			// represents the first row of Gy    [-1]
			//							         [-2]
			//							         [-1]
			// 
			// and the third row of Gy   		 [1]
			//									 [2]
			//									 [1]
			//(rows are for vertical changes in matrices)
			// The numbers within the getcolor brackets represents which pixel we're comparing in relation to the one we're currently in
			// (Ex. x-1,y-1 compares the pixel in the top-left of the current one we're on)
			//pixels on the top are subracted from the pixels below it
			float gy = -m_origImg.getColor(x - 1, y - 1).getBrightness() 
					- 2 * m_origImg.getColor(x, y - 1).getBrightness() 
					- m_origImg.getColor(x + 1, y - 1).getBrightness() 
					+ m_origImg.getColor(x - 1, y + 1).getBrightness() 
					+ 2 * m_origImg.getColor(x, y + 1).getBrightness() 
					+ m_origImg.getColor(x + 1, y + 1).getBrightness();


			//GradMag determines which edges get outlines
			//This comes from the sobel equation's gradient magnitude, it detects the edge strength of the Gx and Gy vector components
			// Gvect = [gx]
			//	       [gy]
			// Magnitude of Gvect = srt[gx^2 + gy^2]
			//The higher the value the stronger the edge
			float gradMag = sqrt(gx * gx + gy * gy);

			//If the caluclated gradient magnitude (edge strength) for the pixel is within the intensity strength specified by the user (intensity), "apply outline"
			if (gradMag > intensity) {
				//"Apply outline" means Turn the pixel to black (to create the outline)
				newCol.set(0, 0, 0);
				//Map the pixel to the masked pixel as well (Pixel in the masked image gets updated as an edge)
				//edge_mask.setColor(x, y, ofColor(255, 255, 255));
			}
			else {
				//Otherwise keep the pixel colored (no outline)
				//newCol = m_origImg.getColor(x, y);
				
				//Convert non-edges to white 
				newCol.set(255,255,255);

				//Keep it as a non-edge
				//edge_mask.setColor(x, y, ofColor(0, 0, 0));
			}
			//update editted image with these outline changes we just performed
			m_edittedImg.setColor(x, y, newCol);
		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();
}

//OUTDATED DIDN'T WORK BUT DIDN'T WANT TO REMOVE EITHER JUST IN CASE
void ofApp::halftone(float detail) {
	//sobel()
	ofColor origCol;
	ofColor newCol;


	//Detail is simply how many pixels are combined/clumped to a singular circle
	//It determines how many "circles" there are in the halftone effect
	const int block_size = detail;

	// Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y += block_size) {

		// Looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x += block_size) {
			
			//Calculate Avg color in the block
			ofColor avgCol = AvgBlockCol(x, y, block_size);

			// Calculate the average brightness
			float avgBrightness = avgCol.getBrightness();

			// To create the gradient effect, the brighter the pixel the larger the circle
			// Thus brightness is used to scale the circle radius
			//OfMap allows scaling from 0-255 brightness range
			float rad = ofMap(avgBrightness, 0, 255, 0, block_size);

			// Draw a circle with the average color and scaled radius
			convertToCircle(x, y, rad, avgBrightness);
		}
	}

	// Updates Cpu data to Gpu
	m_edittedImg.update();
	//edge_mask.update();
}


//HALFTONE2 - MORE ADVANCED & IMPROVED
//Halftone draws circles on top of the original Image
//Users are able to define the color of teh halftone that's applied to Shadows, Midtones and Highlights regions
void ofApp::halftone2(float detail, ImVec4 Shadvect, ImVec4 Midvect, ImVec4 Highvect) {
	const int block_size = detail;
	ofColor origCol;
	ofColor newCol;
	ofColor avgCol;

	//Defining Shadows, Midtones, Highlights
	const float SHADOW_RANGE = 85;
	const float MIDTONES_RANGE = 170;


	//Clamping Vectors to ensure they're [0,1]
	Shadvect.x = ofClamp(Shadvect.x, 0, 1);
	Shadvect.y = ofClamp(Shadvect.y, 0, 1);
	Shadvect.z = ofClamp(Shadvect.z, 0, 1);

	Midvect.x = ofClamp(Midvect.x, 0, 1);
	Midvect.y = ofClamp(Midvect.y, 0, 1);
	Midvect.z = ofClamp(Midvect.z, 0, 1);

	Highvect.x = ofClamp(Highvect.x, 0, 1);
	Highvect.y = ofClamp(Highvect.y, 0, 1);
	Highvect.z = ofClamp(Highvect.z, 0, 1);


	//Converting the colorwheel vector to OfColor
	ofColor triShadCol = ofColor(Shadvect.x * 255, Shadvect.y * 255, Shadvect.z * 255);
	ofColor triMidCol = ofColor(Midvect.x * 255, Midvect.y * 255, Midvect.z * 255);
	ofColor triHighCol = ofColor(Highvect.x * 255, Highvect.y * 255, Highvect.z * 255);

	//Looping Through Pixels

	//Looping through height with block size increments to save processing time
	for (int y = 0; y < m_edittedImg.getHeight(); y += block_size) {

		//looping through width with block size increments to save processing time
		for (int x = 0; x < m_edittedImg.getWidth(); x += block_size) {

			//Getting the Avg Color of blocks
			avgCol = AvgBlockCol(x, y, block_size);

			// Calculate the average brightness of blocks
			float avgBrightness = avgCol.getBrightness();

			//Getting Block Size Radius based on the pixels brightness
			//The brighter (more highlighted) the pixel, the smaller the circle
			float rad = (avgBrightness / 255.0f) * block_size;


			//if pixel is Shadows (don't halftone it)
			if (avgBrightness < SHADOW_RANGE) {

				//(Setter!) Set the block's colors to Halftone color decided through color wheel
				newCol = triShadCol;

				//(Implementor!) Fill the Block with that color
				// If wanted pixel halftone effect, uncomment fillBlock
				//fillBlock(x, y, block_size, newCol);
				convertToCircle(x, y, rad, triShadCol);

			}
			//If pixels is Midtone create halftone
			else if (avgBrightness < MIDTONES_RANGE) {
				convertToCircle(x, y, rad, triMidCol);
			}
			else {
				convertToCircle(x, y, rad, triHighCol);
			}
		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();

}

//TRITONE
//Since the color picker is used as opposed to slider, this will directly deal with the wheel as opposed to parameters
void ofApp::tritone(ImVec4 Shadvect, ImVec4 Midvect, ImVec4 Highvect) {
	ofColor origCol;
	ofColor newCol;

	//Defining Shadows, Midtones, Highlights
	const float SHADOW_RANGE = 85;
	const float MIDTONES_RANGE = 170;


	//Clamping Vectors to ensure they're [0,1]
	Shadvect.x = ofClamp(Shadvect.x, 0, 1);
	Shadvect.y = ofClamp(Shadvect.y, 0, 1);
	Shadvect.z = ofClamp(Shadvect.z, 0, 1);

	Midvect.x = ofClamp(Midvect.x, 0, 1);
	Midvect.y = ofClamp(Midvect.y, 0, 1);
	Midvect.z = ofClamp(Midvect.z, 0, 1);

	Highvect.x = ofClamp(Highvect.x, 0, 1);
	Highvect.y = ofClamp(Highvect.y, 0, 1);
	Highvect.z = ofClamp(Highvect.z, 0, 1);

	//Converting the colorwheel vector to OFColor
	ofColor triShadCol = ofColor(Shadvect.x * 255, Shadvect.y * 255, Shadvect.z * 255);
	ofColor triMidCol = ofColor(Midvect.x * 255, Midvect.y * 255, Midvect.z * 255);
	ofColor triHighCol = ofColor(Highvect.x * 255, Highvect.y * 255, Highvect.z * 255);


	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			//Getting the original color
			origCol = m_origImg.getColor(x, y);

			//Getting pixels brightness to determine if its a highlight, shadow or midtone
			float origBrightness = origCol.getBrightness();

			//if Shadows 
			if (origBrightness < SHADOW_RANGE) { 
				newCol = triShadCol;
			}
			//If Midtone 
			else if (origBrightness < MIDTONES_RANGE) { 
				newCol = triMidCol;
			}
			//If Highlight
			else {
				newCol = triHighCol;
			}
			//Storing the newcol to actual image
			m_edittedImg.setColor(x, y, newCol);
		}
	}
	//Updates Cpu data to Gpu
	m_edittedImg.update();

}

//COLOR-SPLASH FILTER
//Convert whole image to grayscale except for color speciifed by user
//Accuracy determines if user wants only exact color to be chosen, or various shades of the color for stylization
void ofApp::colorsplash(ImVec4 colvect, float accuracy) {
	ofColor origCol;
	ofColor newCol;

	//Defining Shadows, Midtones, Highlights
	const float SHADOW_RANGE = 85;
	const float MIDTONES_RANGE = 170;


	//Clamping Vectors to ensure they're [0,1]
	colvect.x = ofClamp(colvect.x, 0, 1);
	colvect.y = ofClamp(colvect.y, 0, 1);
	colvect.z = ofClamp(colvect.z, 0, 1);

	//Converting the colorwheel vector to OFColor
	ofColor targcol = ofColor(colvect.x * 255, colvect.y * 255, colvect.z * 255);
	

	//Looping Through Pixels

	//Looping through height
	for (int y = 0; y < m_edittedImg.getHeight(); y++) {

		//looping through width
		for (int x = 0; x < m_edittedImg.getWidth(); x++) {
			origCol = m_origImg.getColor(x,y);

			//Distance determines how close/similar the colors
			float col_dist = colDistance(origCol, targcol);

			//If color is within the accuracy
			if (col_dist < accuracy) {
				//Keep pixel's color
				newCol = origCol;
			}
			else {
				//Convert pixel to grayscale
				//Formula for reference: 0.3*R + 0.59*G + 0.11*B
				int gray = origCol.r * 0.3 + origCol.g * 0.59 + origCol.b * 0.11;
				newCol = ofColor(gray,gray,gray);
			}
			//Setting/Mapping color to editted image 
			m_edittedImg.setColor(x, y, newCol);
		}
	}
	//Update
	m_edittedImg.update();

}


//HELPER FUNCTIONS

//AVGBLOCKCOL
//For Halftone Effect - Gets the average color in a block of pixels

ofColor ofApp::AvgBlockCol(int x_start, int y_start, int block_size) {
	//Sums of red, green, blue and pixel counter to get avg color
	int r_sum = 0, g_sum = 0, b_sum = 0, pix_count = 0;

	//Boundaries in Image
	int xMax = min(x_start + block_size, int(m_edittedImg.getWidth()));
	int yMax = min(y_start + block_size, int(m_edittedImg.getHeight()));

	//Calculating Average-Color in Pixel Block
	for (int y = y_start; y < yMax; y++) {
		for (int x = x_start; x < xMax; x++) {
			ofColor currCol = m_edittedImg.getColor(x,y);

			//Add value to sums
			r_sum += currCol.r;
			g_sum += currCol.g;
			b_sum += currCol.b;

			pix_count++;

		}
	}

	if (pix_count == 0) {
		//Returns a black color to avoid dividing by zero and altering the math
		return ofColor(0,0,0);
	}

	//Sum Division to get Avg
	return ofColor (r_sum/pix_count, g_sum/pix_count, b_sum/pix_count);

}

//DRAWCIRCLE - Used for the halftone effect
//Converts the pixel to a circle


void ofApp::convertToCircle(int x_center, int y_center, float rad, ofColor col) {
	//To ensure the circle created doesn't go out of image boundaries
	
	//Ensures no negative indices are created.
	// If circle exceeds the image's top-left, it defaults to 0
	int y_start = max(0, y_center - (int)rad);
	
	//Ensures circle doesn't exit the height of image
	//Defaults to height of image-1 if circle gets out of bounds
	int y_end = min(int(m_edittedImg.getHeight())-1 , y_center + int(rad));

	//Same concept on x-axis
	//Ensures no negative indices are taken, defaults to 0 if that's true
	int x_start = max(0, x_center - (int)rad);
	//Ensures the circle doesn't exceed the width of the image on x-axis, obtains the width-1 if true
	int x_end = min(int(m_edittedImg.getWidth()) - 1, x_center + int(rad));

	//Loop through each pixel in the circle's bounds
	for (int y = y_start; y < y_end; y++) {
		for (int x = x_start; x < x_end; x++) {

			//Obtain distance from pixel to circle
			//This is the x_component of the vector
			int x_comp = x_center - x;
			int y_comp = y_center - y;
			float dist = sqrt(x_comp*x_comp + y_comp*y_comp);

			//if distance is within the radius, and the pixel isn't an outline --> color the circle
			//&& edge_mask.getColor(x, y) == ofColor(0, 0, 0)
			if (dist<rad) {
				m_edittedImg.setColor(x,y,col);
			}

		}
	}
}

//Fills Blocks of Pixels to a defined color
void ofApp::fillBlock(int x_blockstart, int y_blockstart, int block_size, ofColor col) {
	//**No need to add blocksize in y_start & x_start since it's already done in the for loop in halftone

	//Min takes the minimum of the "starting point" vs Width to ensure that the end doesn't exceed the width of image
	int x_end = min(x_blockstart + block_size, (int)m_edittedImg.getWidth());

	//Min takes the minimum of the "starting point" vs height to ensure that the end doesn't exceed the height of image
	int y_end = min(y_blockstart + block_size, (int)m_edittedImg.getHeight());

	//Converts the colors from start to end of the blocks
	for (int y = y_blockstart; y < y_end; y++) {
		for (int x = x_blockstart; x < x_end; x++) {
			m_edittedImg.setColor(x, y, col);
		}
	}
}

//Distance between colors
//Uses Euclidean Equation to calculate the distance between two colors

float ofApp::colDistance(ofColor origCol, ofColor targetCol) {
	//Formula for Reference: 
	//d = sqrt((r1 - r2)^2 + (g1 - g2)^2 + (b1 - b2)^2)
	//Where RGB1 is the original color, and RGB2 is target color

	return (sqrt(pow(origCol.r - targetCol.r, 2) + pow(origCol.g - targetCol.g, 2) + pow(origCol.b - targetCol.b, 2)));
}




