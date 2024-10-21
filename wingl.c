#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define ID_EDIT 401
#define ID_LIST 402
WNDPROC OriginalEditProc;
void InitOpenGL(HWND hwnd, HGLRC *hRC);
void InitView(int width, int height);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SubClassEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass,glwndclass;
	char WndClassName[]="MainWndClass";
	char OpenGLWndClassName[]="OpenGLWndClass";
	wndclass.style=CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance =hInstance;
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = WndClassName;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,"The program requires WINDOWS NT!","Error",MB_ICONERROR);
		return 0;
	}
	glwndclass.style=CS_HREDRAW | CS_VREDRAW;
	glwndclass.lpfnWndProc = glWndProc;
	glwndclass.cbClsExtra = 0;
	glwndclass.cbWndExtra = 0;
	glwndclass.hInstance =hInstance;
	glwndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	glwndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	glwndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	glwndclass.lpszMenuName = NULL;
	glwndclass.lpszClassName = OpenGLWndClassName;
	if(!RegisterClass(&glwndclass)){
		MessageBox(NULL,"The program requires WINDOWS NT!","Error",MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(WndClassName,"OpenGL Window Demo",WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 830,480,NULL,NULL,hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg, NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HINSTANCE hInstance;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	char buf[100];
	static HWND glhwnd, hEditBox, hListBox;
	switch(message){
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
			glhwnd = CreateWindow("OpenGLWndClass",NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,400,5,400,400,hwnd, NULL, hInstance, NULL );
			hEditBox = CreateWindow("edit",NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,400,407,400,32,hwnd, (HMENU)ID_EDIT, hInstance, NULL);
			hListBox = CreateWindow("listbox",NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL ,198,5,200,434,hwnd, (HMENU)ID_LIST, hInstance, NULL);
			// LBS_STANDARD = LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER
			//for(int i=0;i<30;i++){
			//	sprintf(buf,"item %02d",i+1);
			//	SendMessage(hListBox,LB_ADDSTRING,0,(LPARAM)buf);
			//}
			OriginalEditProc = (WNDPROC)SetWindowLong(hEditBox, GWL_WNDPROC,(LONG)SubClassEditProc);
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			GetWindowRect(hListBox, &rect);
			sprintf(buf, "(%d %d)-(%d %d)",rect.left,rect.top,rect.right,rect.bottom);
			TextOut(hdc,10,10,buf,strlen(buf));
			sprintf(buf, "width=%d, height=%d",rect.right-rect.left,rect.bottom-rect.top);
			TextOut(hdc,10,30,buf,strlen(buf));

			EndPaint(hwnd, &ps);
			return 0;
		case WM_COMMAND:
			if(wParam==3){
				//char *text=(char*)lParam;
				SendMessage(hListBox,LB_ADDSTRING,0,lParam);
				//free(text);
			}
			return 0;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_HOME:
					SetFocus(hEditBox);
					break;
				case VK_ESCAPE:
					PostMessage(hwnd,WM_DESTROY,0,0);
					break;


			}
			return 0;		
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK SubClassEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	char buf[100];
	switch (message) {
		case WM_KEYDOWN:
			if (wParam == VK_RETURN) {
				GetWindowText(hwnd, buf, 100);
				PostMessage(GetParent(hwnd),WM_COMMAND,(WPARAM)3,(LPARAM)buf);
				SetWindowText(hwnd, "");
				return 0;
			}
			break;
		case WM_CHAR:	
		case WM_KEYUP:
			if (wParam == VK_RETURN) return 0;
			break;				
	}
	 return CallWindowProc(OriginalEditProc, hwnd, message, wParam, lParam);
}
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HGLRC hRC;
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	switch(message){
		case WM_CREATE:
			InitOpenGL(hwnd, &hRC);
			GetWindowRect(hwnd, &rect);
			InitView(rect.right-rect.left,rect.bottom-rect.top);
			return 0;
		case WM_PAINT:
			hdc=BeginPaint(hwnd, &ps);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SwapBuffers(hdc);
			EndPaint(hwnd, &ps);
			return 0;	
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;	
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
void InitOpenGL(HWND hwnd, HGLRC *hRC) {
	// 获取设备上下文
	HDC hDC;
	hDC = GetDC(hwnd);
	// 设置像素格式
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int format = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, format, &pfd);
	// 创建渲染上下文
	*hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, *hRC);
	ReleaseDC(hwnd, hDC);
}
void InitView(int width, int height){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluLookAt(0.0f, 0.0f, 10.0f ,0.0f, 0.0f, 0.0f, 0.0f,1.0f, 0.0f);
}	
