#include "stdafx.h"

#include "Visible.h"

void Visible::drawRect(HDC hdc, RECT rect, DrawStyle drawStyle) const
{
	HGDIOBJ oldPen = SelectObject(hdc, getDrawStyle(drawStyle));

	POINT oldPoint;
	MoveToEx(hdc, rect.left, rect.top, &oldPoint);

	LineTo(hdc, rect.right, rect.top);
	LineTo(hdc, rect.right, rect.bottom);
	LineTo(hdc, rect.left,  rect.bottom);
	LineTo(hdc, rect.left,  rect.top);

	MoveToEx(hdc, oldPoint.x, oldPoint.y, NULL);

	SelectObject(hdc, oldPen);
}

HGDIOBJ Visible::getDrawStyle(DrawStyle drawStyle) const
{
	if (drawStyles[drawStyle] == NULL)
	{
		drawStyles[drawStyle] = createDrawStyle(drawStyle);
	}
	return drawStyles[drawStyle];
}

HGDIOBJ Visible::createDrawStyle(DrawStyle drawStyle) const
{
	return CreatePen(getPenStyle(drawStyle), getPenWidth(drawStyle), getColor(drawStyle));
}

int Visible::getPenStyle(DrawStyle drawStyle) const
{
	switch(drawStyle)
	{
	case dsDefault:
	default:
		return PS_SOLID;
	}
}

int Visible::getPenWidth(DrawStyle drawStyle) const
{
	switch(drawStyle)
	{
	case dsCalculatorMarkedCell:
		return 3;
	case dsCalculatorVariantCell:
		return 2;
	default:
		return 1;
	}
}

COLORREF Visible::getColor(DrawStyle drawStyle) const
{
	switch(drawStyle)
	{
	case dsBorder:
	case dsInputZoneCell:
		return RGB(100,100,100);
	case dsCalculatorBorder:
		return RGB(0,20,255);
	case dsCalculatorCellBorder:
		return RGB(0,200,255);
	case dsCalculatorMarkedCell:
	case dsInputZoneCurrent:
	case dsResultBorder:
		return RGB(0,255,0);
	case dsCalculatorVariantCell:
	case dsInputZoneNext:
		return RGB(255,255,0);
	case dsInputZoneInvalidCell:
		return RGB(255, 100, 0);
	default:
		return RGB(0,0,0);
	}
}

Box Visible::shiftBox(Box currentBox, RECT rect) const
{
	currentBox.transfer(currentBox.width(),0);
	if (currentBox.bottomRight.x > (rect.right - rect.left))
	{
		currentBox.transfer( - currentBox.topLeft.x, currentBox.height());
	}
	return currentBox;
}

Box Visible::realBox(RECT rect, Box box) const
{
	box.transfer(rect.left, rect.top);
	return box;
}
