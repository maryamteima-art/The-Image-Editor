#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		//CUSTOM VARIABLES

		ofImage m_origImg;
		ofImage m_edittedImg;
		ofImage m_prehalftoneImg;
		ofImage m_presplashImg;
		ofImage m_preposterImg;
		ofImage m_presobelImg;
		//Brightness,Contrast,Saturation Controls
		float m_contrast;
		float m_brightness;
		float m_saturation;

		//Colorbalance will have multiple sliders - tone to apply the balanace to (midtones, higlights, shadows), r_intensity, g_intensity, b_intensity, and strength of grading
		//Uppertone specifies the maximum range it applies to while lowertone specifies minimum range to apply colograding to
		float m_lowertone;
		float m_uppertone;
		float m_rIntensity;
		float m_gIntensity;
		float m_bIntensity;
		float m_strength;

		//Yellow-Blue Tint
		float m_YBtint;

		//Sobel/Outline Strength, represents the sensitivity of the sobel filter
		//0-127 Low sensitivity all lines get detected. Fine lines & prominent lines are detected
		//128-199 mid sensitivity detects prominent edges and some fine details
		//200-255 high sensitivity, only prominent lines are detected
		float m_sobstr;
		ofImage edge_mask;
		float m_detail;

		//Tritone
		ImVec4 m_triShadCol;
		ImVec4 m_triMidCol;
		ImVec4 m_triHighCol;

		//Halftone
		ImVec4 m_halfShadCol;
		ImVec4 m_halfMidCol;
		ImVec4 m_halfHighCol;

		//Color-Splash
		ImVec4 m_splashCol;
		float m_accuracy;

		//Eyedropper
		bool eyedropper = false;
		bool click = false;
		ofColor eyeCol;
		

		ofxImGui::Gui m_gui;

		//OF Functions
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		//CUSTOM FUNCTIONS

		//LOADING & SAVING
		void loadImg();
		void saveImg();

		//FILTERS
		void Brightness(float intensity);
		void Contrast(float intensity);
		void Saturation(float intensity);
		void YellowBlue(float intensity);

		//ColorBalance will have red, green, blue, and brightness intensities
		//Brightness indicates if it applies to shadows, midtones or highlights
		void ColBalance(float r_intensity, float g_intensity, float b_intensity, float lower_tone, float upper_tone, float grade_strength);
		
		
		//Intensity is the threshold of the outline
		void sobel(float intensity);


		//Halftone Filter will consist of creating circular shading (halftone) attop the image
		//Detail is simply how many pixels are combined to a singular circle
		//It determines how many "circles" there are in the halftone effect
		
		//1st version of Halftone, didn't work
		void halftone(float detail);

		//2nd version Improved
		//Users can specify the color of halftone applied to shadows, midtones, and highlights & can specify detail
		//High detail = increased number of pixels per circle-block = larger circles
		void halftone2(float detail, ImVec4 Shadvect, ImVec4 Midvect, ImVec4 Highvect);

		void tritone(ImVec4 Shadvect, ImVec4 Midvect, ImVec4 Highvect);

		//Color-Splash
		//Converts everything to grayscale except the color chosen by the user
		//User gets to choose the color and how accurate the filter is
		//High Accuracy = Exact shade chosen, low accuracy = slightly tinted & exact chosen
		void colorsplash(ImVec4 colvect, float accuracy);

		//HELPER FUNCTIONS
		ofColor AvgBlockCol(int x, int y, int block_size);

		void convertToCircle(int x_center, int y_center, float rad, ofColor col);

		void fillBlock(int x_start, int y_start, int block_size, ofColor col);

		float colDistance(ofColor origCol , ofColor targetCol);
};
