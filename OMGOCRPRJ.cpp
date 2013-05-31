// Main window. Rather roughly coded, I'm not a Win32 expert, not even remotely
//

#include "stdafx.h"

#include "OMGOCRPRJ.h"

#include "Globals.h"

#include "ShapeParser.h"
#include "ShapeReader.h"

#include "Keypad.h"
#include "CharDisplay.h"
#include "InputZone.h"

#include "Evaluator.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND currentWindow;

Box Globals::m_defaultDefaultImageBox = Box(Point(0, 0), Point(120, 160));
Box Globals::m_defaultImageBox = Box(Point(0, 0), Point(120, 160));

Keypad keypad;
InputZone inputZone;
CharDisplay charDisp(false); //input buffer display
CharDisplay resultDisp(true); //result display
Evaluator evaluator;


//Complete reset
void updateData()
{
	ShapeParser parser;
	std::string fileName = "definition.txt";
	parser.parse("definition.txt");

	keypad = Keypad();

	charDisp.clear();

	resultDisp.clear();

	inputZone.clear();
}

void invalidate()
{
	InvalidateRect(currentWindow, NULL, TRUE);
}

//Input zone changed: updating buffer & result 
void updateCharDisp()
{
	charDisp.clear();
	std::vector<Character> input = inputZone.characters();
	charDisp.assign(input);

	resultDisp.clear();
	std::vector<Character> result = evaluator.evaluate(input);
	resultDisp.assign(result);

	invalidate();
}

//Window part boundaries

Point keypadTopLeft(10,20);

int keypadWidth() {
	return Globals::defaultImageBox().width() * 2;
}

int keypadHeight() {
	return Globals::defaultImageBox().height() * 2;
}

RECT keypadRect()
{
	RECT imageRect;
	imageRect.left = keypadTopLeft.x;
	imageRect.top = keypadTopLeft.y;
	imageRect.bottom = imageRect.top + keypadHeight();
	imageRect.right = imageRect.left + keypadWidth();
	return imageRect;
}

Box chardispBox;

RECT chardispRect()
{
	return chardispBox;
}

Box inputAreaBox;

RECT inputAreaRect()
{
	return inputAreaBox;
}

Box resultdispBox;

RECT resultdispRect()
{
	return resultdispBox;
}

//recalculate window part sizes and positions
void updateWindowSize(int width, int height)
{
	keypadTopLeft.x = width - keypadWidth() - 10;
	keypadTopLeft.y = height - keypadHeight() - 10;

	chardispBox.topLeft = Point(10, keypadTopLeft.y);
	chardispBox.bottomRight = Point(keypadTopLeft.x - 10, keypadTopLeft.y + keypadHeight() / 2);

	resultdispBox.topLeft = Point(chardispBox.left(), chardispBox.bottom());
	resultdispBox.bottomRight = Point(chardispBox.right(), keypadTopLeft.y + keypadHeight());

	inputAreaBox.topLeft = Point(10,10);
	inputAreaBox.bottomRight = Point(width - 10, keypadTopLeft.y - 10);

	inputZone.updateCharBox();
	charDisp.updateCharBox();
	resultDisp.updateCharBox();
}

void updateWindowSize()
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo(currentWindow,&wi))
	{
		Box clientBox = wi.rcClient;
		updateWindowSize(clientBox.width(), clientBox.height());
	}
}

//draw a green box on the keypad
void drawCurrentRecognizedChars(HDC hdc)
{
	const std::vector<Character> & chars = inputZone.currentChars();
	keypad.drawMarked(hdc, keypadRect(), chars);
}

//main drawing function
void paint(HDC hdc)
{
	keypad.draw(hdc,keypadRect());

	drawCurrentRecognizedChars(hdc);

	inputZone.draw(hdc, inputAreaRect());

	charDisp.draw(hdc, chardispRect());

	resultDisp.draw(hdc, resultdispRect());
}

//Mouse events processing

bool mouseIsDown = false;
Point clickPoint;
bool mouseHasMoved = false;
std::vector<Point> draggedPoints;

bool mouseMoved(Point point)
{
	return mouseHasMoved;
}

void mouseDown(Point point)
{
	clickPoint = point;
	mouseIsDown = true;
	draggedPoints.clear();
}

void endDrag(Point point)
{
	inputZone.drag(draggedPoints, inputAreaRect());
	mouseHasMoved = false;
	draggedPoints.clear();
	updateCharDisp();
	invalidate();
}

void mouseUp(Point point)
{
	mouseIsDown = false;
	if ( mouseMoved(point) )
		endDrag(point);
	else
		mouseClick(point);
}

void mouseMove(Point point)
{
	if (mouseIsDown && !mouseHasMoved)
	{
		if (point.dist(clickPoint) > 5)
			mouseHasMoved = true;
	}

	if(mouseIsDown)
	{
		draggedPoints.push_back(point);

		//partial redraw during drag
		HDC hdc = GetDC(currentWindow);
		drawDraggedPoints(hdc);
		ReleaseDC(currentWindow, hdc);
	}
}

//keypad click processing
void mouseClick(Point point)
{
	if (Box(keypadRect()).contains(point))
	{
		//find an image on button...
		if (const CompositeImage* image = keypad.locate(point, Box(keypadRect())))
		{
			CompositeImage charImg = *image;
			//...and add it to the input zone
			inputZone.add(charImg);
			updateCharDisp();
		}

		invalidate();
	}
}

//right mouse click processing
void rmouseUp(Point point)
{
	if (Box(keypadRect()).contains(point))
	{
		if (const CompositeImage* image = keypad.locate(point, Box(keypadRect())))
		{
			CompositeImage charImg = *image;

			//there aren't any characters on keypad, remember that?
			Character ch = CharacterRules::recognizeSingle(charImg);

			if (ch.name() == "Plus")
			{
				Globals::growImageBox();
			}
			if (ch.name() == "Minus")
			{
				Globals::shrinkImageBox();
			}
			if (ch.name() == "Equals")
			{
				Globals::resetImageBox();
			}
			if (ch.name() == "Zero")
			{
				PostMessage(currentWindow, WM_COMMAND, ID_FILE_REFRESH, 0);
			}
			if (ch.name() == "Four")
			{
				PostMessage(currentWindow, WM_CLOSE, 0, 0);
			}

			invalidate();

			//may be needed if image size has changed
			updateWindowSize();
		}
	}
}

void drawDraggedPoints(HDC hdc)
{
	Image image(inputAreaBox, draggedPoints);
	image.draw(hdc, inputAreaBox);
}

//I don't really know what's going on later... let's trust Visual Studio


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	updateData();

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OMGOCRPRJ, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OMGOCRPRJ));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OMGOCRPRJ));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_OMGOCRPRJ);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	currentWindow = hWnd;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_REFRESH:
			updateData();
			invalidate();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		paint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			updateWindowSize(x,y);
		}
		break;
	case WM_LBUTTONDOWN:
		mouseDown(getPoint(lParam));
		break;
	case WM_LBUTTONUP:
		mouseUp(getPoint(lParam));
		break;
	case WM_RBUTTONUP:
		rmouseUp(getPoint(lParam));
		break;
	case WM_MOUSEMOVE:
		mouseMove(getPoint(lParam));
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

Point getPoint(LPARAM lParam)
{
	return Point(LOWORD(lParam),HIWORD(lParam));
}

