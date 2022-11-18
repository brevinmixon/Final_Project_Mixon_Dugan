#pragma once
#include <string>

namespace DrawingUtilNG {

	//converts a color model from HSV (degrees, 0.-1., 0.-1.) to RGB (3 values 0.-1.)
	bool hsv2rgb(double H, double S, double V,
		double& red, double& green, double& blue);

	// takes over the screen and allows graphical text input
	bool buildStringFromFsInkey(int key, std::string& currString);

	// draws common shapes, filled in or not
	// if outerSizes are given (not negative) use sizes to create a hole instead of shape
	//    and outerSize to draw the filled-in part.
	void drawCircle(double centerX, double centerY, double sizeX, double sizeY, bool filled, 
		double outerSizeX = -1, double outerSizeY = -1);
	void drawRectangle(double x, double y, int sizeX, int sizeY, bool filled, 
		double outerSizeX = -1, double outerSizeY = -1);

	// many-pointed star, given outer and inner size, startAngle assures one
	// point is at that angle (used to animate rotation)
	void drawStar(double centerX, double centerY,
		double outerSize, double innerSize, int numbPoints, double startAngle, bool filled);

	// draws self-connecting star gram. (e.g., pentagram, star-of-david, etc)
	void drawStarGram(double centerX, double centerY,
		double size, int numbPoints, int numbSkip, double startAngle, bool filled);
};
