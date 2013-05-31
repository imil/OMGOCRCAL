#pragma once

#include "Point.h"

//Styles for image drawing
enum DrawStyle
{
	dsDefault,
	dsImage,
	dsBorder,
	dsCalculatorBorder,
	dsCalculatorCellBorder,
	dsCalculatorMarkedCell,
	dsCalculatorVariantCell,
	dsInputZoneCell,
	dsInputZoneCurrent,
	dsInputZoneNext,
	dsInputZoneInvalidCell,
	dsResultBorder,
};

//Base class for all image output areas
class Visible
{
public:
	//Draw inside the specified rectangle
	virtual void draw(HDC hdc, RECT rect) const = 0;

	virtual ~Visible() {}

protected:
	void drawRect(HDC hdc, RECT rect, DrawStyle drawStyle = dsDefault) const;

	//shifts given box to right; translates to new line if right end reached
	Box shiftBox(Box currentBox, RECT rect) const;
	//projection of box to the rectangle
	Box realBox(RECT rect, Box box) const;

	//styles to HPEN mapping.
	HGDIOBJ getDrawStyle(DrawStyle drawStyle) const;
	//FIXME: handles are not released
	HGDIOBJ createDrawStyle(DrawStyle drawStyle) const;
	int getPenStyle(DrawStyle drawStyle) const;
	int getPenWidth(DrawStyle drawStyle) const;
	COLORREF getColor(DrawStyle drawStyle) const;
private:
	mutable std::map<DrawStyle, HGDIOBJ> drawStyles;
};