//gcc gldemo.c -o gldemo.exe -mwindows -lopengl32 -lglu32 -lglut32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "gldemores.h"

#define ID_Edit    401
#define ID_List    402

// 参数结构体
typedef struct {
	float x1, y1, z1; // 线段起点坐标
	float x2, y2, z2; // 线段终点坐标
} LineParams;
typedef struct {
	float centerX, centerY, centerZ; // 长方体中心坐标
	float length, width, height; // 长方体的长宽高
	float faceDirectionX, faceDirectionY, faceDirectionZ; // 面的方向
} CuboidParams;

typedef void (*DrawFunction)(void*);
typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

// 窗口宽度和高度
static const int width = 410;
static const int height = 400;
HGLRC hRC;
HWND hCmdList, hEditBox, hGLCanvas, hGLResult;
UINT timerID;
UINT timerState;

float xRot = 0.0f;
float yRot = 0.0f;
float zRot = 0.0f;
float Rot = 0.0;
LineParams lineParams;
CuboidParams cuboidParams;
WNDPROC OriginalEditProc = NULL;

static const char szWindowClass[] = "WindowClass";
static const char szglWindowClass[] = "OpenGLWindowClass";
static const char szglrsWindowClass[] = "ResultWindowClass";
static const char szTitle[] = "OpenGL 3D Graphics";

void gl_Draw();
void RenderScene(HWND hwnd, int functioonIndex, void* functionParams);
int InitOpenGL(HWND hwnd);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK rsWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;

HWND CreateCommandPanel(HINSTANCE hInstance, HWND hwndParent, int left, int top, int width, int height);
HWND CreateCommandEdit(HINSTANCE hInstance, HWND hwndParent, int left, int top, int width, int height);
HWND CreateOpenGLCanvas(HINSTANCE hInstance, HWND hwndParent, int left, int top, int width, int height);
HWND CreateOpenGLResult(HINSTANCE hInstance, HWND hwndParent, int left, int top, int width, int height);

void drawHollowCircle(float cx, float cy, float radius) {
    int num_segments = 100;
    float angle;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        angle = 2.0 * 3.1415926 * i / num_segments;
        glVertex2f(cos(angle) * radius + cx, sin(angle) * radius + cy);
    }
    glEnd();
}
void drawWireCube(float width, float height, float depth) {
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth = depth * 0.5f;

    glBegin(GL_LINES);
    // 前面
    glVertex3f(-halfWidth, -halfHeight, halfDepth);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glVertex3f(halfWidth, halfHeight, halfDepth);
    glVertex3f(halfWidth, halfHeight, halfDepth);
    glVertex3f(-halfWidth, halfHeight, halfDepth);
    glVertex3f(-halfWidth, halfHeight, halfDepth);
    glVertex3f(-halfWidth, -halfHeight, halfDepth);
    // 后面
    glVertex3f(-halfWidth, -halfHeight, -halfDepth);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glVertex3f(halfWidth, halfHeight, -halfDepth);
    glVertex3f(halfWidth, halfHeight, -halfDepth);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);
    glVertex3f(-halfWidth, -halfHeight, -halfDepth);
    // 连接前后面
    glVertex3f(-halfWidth, -halfHeight, halfDepth);
    glVertex3f(-halfWidth, -halfHeight, -halfDepth);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glVertex3f(-halfWidth, halfHeight, halfDepth);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);
    glVertex3f(halfWidth, halfHeight, halfDepth);
    glVertex3f(halfWidth, halfHeight, -halfDepth);
    glEnd();
}
void DrawFunction0(void* drawParams){
	drawHollowCircle(0.0, 0.0, 2.0);
}
void DrawFunction1(void* drawParams){
	drawWireCube(1.2, 2.4, 4.5);
}
void DrawFunction2(void* params) {
	// 实现绘制长方体的函数
	glColor3f(1.0f,0.5f,0.0f);
	glBegin(GL_QUADS);
    glVertex3f(-1.0f, -1.0f, 0.0f); // 顶点 A
    glVertex3f(1.0f, -1.0f, 0.0f);  // 顶点 B
    glVertex3f(1.0f, 1.0f, 0.0f);   // 顶点 C
    glVertex3f(-1.0f, 1.0f, 0.0f);  // 顶点 D
	glEnd();
}
void DrawFunction3(void* params) {
	// 实现绘制其他图形的函数
}
void DrawLine(void* lineParams) {
	LineParams* params = lineParams;
	glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_LINES);
    
    glVertex3f(params->x1, params->y1, params->z1);
    glVertex3f(params->x2, params->y2, params->z2);

    glVertex3f(-1.0f, -1.0f, 0.0f); // 顶点 A
    glVertex3f(1.0f, -1.0f, 0.0f);  // 顶点 B
    
    glEnd();
}
void DrawCuboid(void* params) {
	CuboidParams *cuboidParams = params;
    // 根据 params 中的参数绘制长方体
    // ...
}
DrawFunction drawFunctions[] = {
	DrawFunction0,
	DrawFunction1,
	DrawFunction2,
	DrawFunction3,
	DrawLine,
	DrawCuboid
};
int InitOpenGL(HWND hwnd) {
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
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluLookAt(0.0f, 0.0f, 10.0f ,0.0f, 0.0f, 0.0f, 4.0f,4.0f, 4.0f);

	ReleaseDC(hwnd, hDC);
	return TRUE;
}
void RenderScene(HWND hwnd, int functionIndex, void* functionParams) {
	HDC hDC = GetDC(hwnd);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glRotatef(0.93,0.577,0.577,0.577);
	drawFunctions[functionIndex](functionParams);
	SwapBuffers(hDC);
	ReleaseDC(hwnd, hDC);
}
void ReadEditBox(HWND hEditBox, LineParams* outParams) {
    char buffer[100];
    GetWindowText(hEditBox, buffer, sizeof(buffer));
    sscanf(buffer, "%f %f %f %f %f %f", &outParams->x1, &outParams->y1, &outParams->z1, &outParams->x2, &outParams->y2, &outParams->z2);
}
void ReadEditBoxCuboid(HWND hEditBox, CuboidParams* outParams) {
    char buffer[100];
    GetWindowText(hEditBox, buffer, sizeof(buffer));
    sscanf(buffer, "%f %f %f %f %f %f %f %f %f",
           &outParams->centerX, &outParams->centerY, &outParams->centerZ,
           &outParams->length, &outParams->width, &outParams->height,
           &outParams->faceDirectionX, &outParams->faceDirectionY, &outParams->faceDirectionZ);
}
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	char buf[100];
    switch (message) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN) { // 如果按下的是回车键
                // 这里处理回车键按下的事件
                // 获取文本框中的内容
                GetWindowText(hwnd, buf, 100);
                // 分析内容并设置绘图参数
                // 绘图命令
                
                // line(x1, y1, x2, y2) 
                // line(x1, y1, z1, x2, y2, z2) 
                // ball(x,y,z,r)
                // cube(length, width, height) 中心在原点，棱与坐标轴平行。
                // 清空文本框 
                SetWindowText(hwnd, "");
                // 向gl窗口发送消息执行绘图操作
                return 0; // 返回0表示已经处理了这个键
            }
            break;
    }

    // 调用原始的窗口过程
    return CallWindowProc(OriginalEditProc, hwnd, message, wParam, lParam);
}
void SubclassEditBox( HWND hwndParent ) {
    HWND hwndEdit = GetDlgItem(hwndParent, ID_Edit);
    OriginalEditProc = (WNDPROC)SetWindowLongPtr(hEditBox, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc, glwc, glrs;
	HWND hwnd;
	HDC hdc;
	PAINTSTRUCT ps;
	MSG msg;

	// 注册窗口类
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = "gldemomenu";
	wc.lpszClassName = szWindowClass;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	// 创建窗口
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1080, 470,
		NULL, NULL, hInstance, NULL);

	if (!hwnd) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	glwc.cbSize = sizeof(WNDCLASSEX);
	glwc.style = CS_HREDRAW | CS_VREDRAW ;
	glwc.lpfnWndProc = glWndProc;
	glwc.cbClsExtra = 0;
	glwc.cbWndExtra = 0;
	glwc.hInstance = hInstance;
	glwc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	glwc.hCursor = LoadCursor(NULL, IDC_ARROW);
	glwc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	glwc.lpszMenuName = NULL;
	glwc.lpszClassName = szglWindowClass;
	glwc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&glwc)) {
		MessageBox(NULL, "OpenGL Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	/*	
	glrs.cbSize = sizeof(WNDCLASSEX);
	glrs.style = CS_HREDRAW | CS_VREDRAW ;
	glrs.lpfnWndProc = rsWndProc;
	glrs.cbClsExtra = 0;
	glrs.cbWndExtra = 0;
	glrs.hInstance = hInstance;
	glrs.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	glrs.hCursor = LoadCursor(NULL, IDC_ARROW);
	glrs.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	glrs.lpszMenuName = NULL;
	glrs.lpszClassName = szglrsWindowClass;
	glrs.hIconSm = LoadIcon(NULL, IDI_APPLICATION);	
	if (!RegisterClassEx(&glrs)) {
		MessageBox(NULL, "OpenGL2 Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}	
	*/

	// 创建UI组件
	hGLCanvas = CreateOpenGLCanvas(hInstance, hwnd,10,30,410,370);
	//hGLResult = CreateOpenGLCanvas(hInstance, hwnd,640,30,410,370);
	
	// 显示窗口
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	// 初始化OpenGL

	//if (!InitOpenGL(hGLResult)) {
	//	MessageBox(NULL, "OpenGL Initialization Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
	//	return 0;
	//}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HINSTANCE hInstance;
	PAINTSTRUCT ps;
	HDC hdc;
	char buf[100];
	static int functionIndex=2;
	static int functionCount = sizeof(drawFunctions) / sizeof(drawFunctions[0]);

	switch (message) {
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

			
			hCmdList = CreateCommandPanel(hInstance, hwnd,426,30,205,340);
			hEditBox = CreateCommandEdit(hInstance, hwnd,426,375,205,25);
			SubclassEditBox(hwnd);
			if (!InitOpenGL(hGLCanvas)) {
				MessageBox(NULL, "OpenGL Initialization Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
				return 0;
			}

			lineParams.x1 = 0.0;
			lineParams.y1 = 0.0;
			lineParams.z1 = 0.0;
			lineParams.x2 = 0.0;
			lineParams.y2 = 1.0;
			lineParams.z2 = 0.0;

			timerState = 0;
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			sprintf(buf,"hGLCanvas = %X",hGLCanvas);
			SetWindowText(hwnd, buf);
			sprintf(buf,"functionIndex = %d",functionIndex);
			TextOut(hdc,50,10,buf,strlen(buf));
			sprintf(buf,"message size: %d  wParam size: %d  lParam size: %d",sizeof(message), sizeof(wParam),sizeof(lParam));
			TextOut(hdc,350,10,buf,strlen(buf));
			EndPaint(hwnd, &ps);
			return 0;
		case WM_COMMAND:
			// 处理命令列表和编辑框的消息
			return 0;
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					PostMessage(hwnd, WM_DESTROY, 0, 0);
					break;
				case VK_SPACE:
					PostMessage(hGLCanvas, WM_KEYDOWN, VK_SPACE, 0);
					break;	
				case VK_UP:
					functionIndex = (functionIndex+1)%functionCount;

					PostMessage(hGLCanvas,WM_KEYDOWN,VK_UP,functionIndex);
					InvalidateRect(hwnd,NULL,FALSE);
					break;
				case VK_DOWN:
					functionIndex = (functionIndex+functionCount-1)%functionCount;
					PostMessage(hGLCanvas,WM_KEYDOWN,VK_DOWN,functionIndex);
					InvalidateRect(hwnd,NULL,FALSE);
					break;
			}
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hDC;
	char buf[100];
	static void *params = NULL;

	static int functionIndex=1;

	switch (message) {
		case WM_CREATE:
			InitOpenGL(hwnd);
			timerState=0;
			return 0;
		case WM_PAINT:
			hDC = BeginPaint(hwnd, &ps);
			RenderScene(hwnd, functionIndex, params);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_SPACE:					
					if(timerState==0){
						timerState=1;
						timerID = SetTimer(hwnd, 1, 100, NULL); // 设置定时器
					}else{
						timerState=0;
						KillTimer(hwnd, 1);
					}
					return 0;	

				case VK_UP:
				case VK_DOWN:
					functionIndex=lParam;

					switch(functionIndex){
						case 0:
							params = NULL;
							break;
						case 1:
							params = NULL;
							break;
						case 2:
							params = NULL;
							break;
						case 3:
							params = NULL;
							break;
						case 4:						
							params = (void*)&lineParams;
							break;
						case 5:
							params = NULL; 
							break;	
					}
					RenderScene(hwnd, functionIndex, params);
					break;	
			}
			return 0;
		case WM_TIMER:
			RenderScene(hwnd, functionIndex, params);
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case ID_Edit:
					ReadEditBox(hEditBox, &lineParams);
					RenderScene(hwnd, functionIndex, params);
					break;
				case ID_List:
					break;
			}
			return 0;
		case WM_DESTROY:
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hRC);
			KillTimer(hwnd, 1); // 删除定时器
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK rsWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	switch(message){
		case WM_CREATE:
			//InitOpenGL(hwnd);
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
HWND CreateCommandPanel(HINSTANCE hInstance, HWND hwndParent,int left, int top, int width,int height) {
	return CreateWindowEx(0, "listbox","Command List",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER,
		left, top, width, height,
		hwndParent, (HMENU)ID_List, hInstance, NULL);
}
HWND CreateCommandEdit(HINSTANCE hInstance, HWND hwndParent,int left, int top, int width,int height) {
	return CreateWindowEx(0, "EDIT", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		left, top, width, height,
		hwndParent, (HMENU)ID_Edit, hInstance, NULL);
}
HWND CreateOpenGLCanvas(HINSTANCE hInstance, HWND hwndParent,int left, int top, int width,int height) {
	return CreateWindowEx(0, szglWindowClass, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER,
		left, top, width, height,
		hwndParent, NULL, hInstance, NULL);
}
HWND CreateOpenGLResult(HINSTANCE hInstance, HWND hwndParent,int left, int top, int width,int height) {
	return CreateWindowEx(0, szglrsWindowClass, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER,
		left, top, width, height,
		hwndParent, NULL, hInstance, NULL);
}
