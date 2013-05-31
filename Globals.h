#pragma once

#include "Probability.h"
#include "Point.h"

//Main constants
struct Globals
{
	static double lineThreshold() {
		return 0.1; }

	static double circleThreshhold() {
		return 0.2;
	}

	static double clockThreshold() {
		return 2.0;
	}

	static double eps() {
		return 1.0e-3;
	}

	static void log(std::string message) {
		message = "";
		///dev/null
	}

	static Probability probabilityToConsiderShape() {
		return 60;
	}

	static double horizontalOrVerticalMinimalRatio() {
		return 4.0; 
	}

	//transfer value into different interval. Doesn't really belong here, but where else?
	static double interpolate(double val, double minSrc, double maxSrc, double minDst, double maxDst);

	//default box for character images
	static Box defaultImageBox() {
		return m_defaultImageBox;
	}

	static double anchorTolerance() {
		return 0.05;
	}

	static void growImageBox() {
		m_defaultImageBox.scale(1.2,1.2);
	}

	static void shrinkImageBox() {
		m_defaultImageBox.scale(0.8,0.8);
	}

	static void resetImageBox() {
		m_defaultImageBox = m_defaultDefaultImageBox;
	}

	static int max_characters_in_result() {
		return 99;
	}

private:
	static Box m_defaultImageBox;
	static Box m_defaultDefaultImageBox;
};