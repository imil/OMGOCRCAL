#pragma once

//Main window

#include "resource.h"

#include "Point.h"

void updateWindowSize(int width, int height);
void mouseDown(Point point);
void mouseUp(Point point);
void mouseMove(Point point);
void mouseClick(Point point);
void rmouseUp(Point point);
void paint(HDC hdc);
Point getPoint(LPARAM lParam);
void drawDraggedPoints(HDC hdc);
