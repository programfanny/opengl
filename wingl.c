// gcc wingl.c -o wingl.exe -mwindows -lopengl32 -lglu32 -lglut32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.141592653589732
#define ID_TIME 1
#define ID_EDIT 401
#define ID_LIST 402

typedef struct _Point3{
	char name[3];
	float x,y,z;
}POINT3;
typedef struct {
	float x, y, z;
} Vector3;
typedef struct {
	int indices[5];
} Face5;
typedef struct {
	int indices[6];
} Face6;
typedef void (*DrawFunction)();

int findPt(POINT3 *pt, char *pname);
void body60(Vector3 *vpoint,Face5 *face5, Face6 *face6);
void DrawFootball();
void InitOpenGL(HWND hwnd, HGLRC *hRC);
void InitView(int width, int height);
void RenderScene(int functionIndex);
void DrawAxis();
void DrawCube();
void DrawTetrahedron();
void DrawOctahedron();
void DrawDodecahedron();
void DrawIcosahedron();
WNDPROC OriginalEditProc;
DrawFunction drawFunctions[] = {
	DrawCube,
	DrawTetrahedron,
	DrawOctahedron,
	DrawDodecahedron,
	DrawIcosahedron,
	DrawFootball,
};
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
	static int iIndex;
	static BOOL isrot;
	//OBJECT object;
	char *function[]={
		"Cube",
		"Tetrahedron",
		"Octahedron",
		"Dodecahedron",
		"Icosahedron",
		"Football",
	};
	switch(message){
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
			glhwnd = CreateWindow("OpenGLWndClass",NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,400,5,400,400,hwnd, NULL, hInstance, NULL );
			hEditBox = CreateWindow("edit",NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,400,407,400,32,hwnd, (HMENU)ID_EDIT, hInstance, NULL);
			hListBox = CreateWindow("listbox",NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL ,198,5,200,434,hwnd, (HMENU)ID_LIST, hInstance, NULL);
			for(int i=0;i<sizeof(function)/sizeof(function[0]);i++){
				sprintf(buf,"%02d-%s",i+1,function[i]);
				SendMessage(hListBox,LB_ADDSTRING,0,(LPARAM)buf);
			}
			OriginalEditProc = (WNDPROC)SetWindowLong(hEditBox, GWL_WNDPROC,(LONG)SubClassEditProc);
			SetTimer(hwnd, 1, 250, NULL);
			iIndex = 0;
			isrot=1;
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			PostMessage(glhwnd, WM_PAINT,(WPARAM)iIndex,0);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case ID_LIST:
					switch(HIWORD(wParam)){
						case LBN_SELCHANGE:
							iIndex  = SendMessage (hListBox, LB_GETCURSEL, 0, 0) ;
							//FormObject(iIndex,&object);
							PostMessage(glhwnd, WM_PAINT,(WPARAM)iIndex,0);
							SetFocus(hwnd);
							break;
					}
					break;
			}
			return 0;
		case WM_TIMER:
			PostMessage(glhwnd, WM_PAINT, (WPARAM)iIndex, 0);
			return 0;	
		case WM_KEYDOWN:
			switch(wParam){
				case VK_HOME:
					SetFocus(hEditBox);
					break;
				case VK_ESCAPE:
					PostMessage(hwnd,WM_DESTROY,0,0);
					break;
				case VK_SPACE:
					isrot=!isrot;
					if(isrot){
						SetTimer(hwnd, ID_TIME, 250, NULL);
					}else{
						KillTimer(hwnd, ID_TIME);
					}
					break;	
			}
			return 0;		
		case WM_DESTROY:
			KillTimer(hwnd,1);
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HGLRC hRC;
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	char buf[100];
	float angle;
	
	static Vector3 *vp;
	static Face5* face5;
	static Face6* face6;
	static int functionIndex;
	void *param;
	switch(message){
		case WM_CREATE:
			InitOpenGL(hwnd, &hRC);
			GetWindowRect(hwnd, &rect);
			InitView( rect.right-rect.left, rect.bottom-rect.top );
			functionIndex = 0;
			return 0;
		case WM_PAINT:
			functionIndex = wParam;
			hdc = BeginPaint(hwnd, &ps);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
			RenderScene(functionIndex);
			SwapBuffers(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_DESTROY:
			free(face6);
			free(face5);
			free(vp);
			PostQuitMessage(0);
			return 0;	
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK SubClassEditProc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam){
	int iIndex;
	char buf[100];
	static HWND hParent, hListBox;
	switch (message) {
		case WM_KEYDOWN:
			if (wParam == VK_RETURN) {
				GetWindowText(hwnd, buf, 100);
				SetWindowText(hwnd, "");
				iIndex = atoi(buf);
				if(iIndex<0)iIndex=-1;
				if(iIndex>=0){
					SendMessage(hListBox,LB_SETCURSEL,(WPARAM)iIndex,0);
				}
				SendMessage(hParent, WM_COMMAND,MAKEWPARAM(ID_LIST,LBN_SELCHANGE),(LPARAM)hListBox);
				PostMessage(GetParent(hwnd), WM_PAINT,(WPARAM)iIndex,0);
				SetFocus(hParent);
				return 0;
			}
			break;
		case WM_CHAR:	
		case WM_KEYUP:
			if (wParam == VK_RETURN) return 0;
			break;
		case WM_SETFOCUS:
			hParent = GetParent(hwnd);
			hListBox = FindWindowEx(hParent, NULL,"ListBox",NULL);
			break;	
	}
	 return CallWindowProc(OriginalEditProc, hwnd, message, wParam, lParam);
}
void InitOpenGL(HWND hwnd, HGLRC *hRC) {
	HDC hDC;
	hDC = GetDC(hwnd);
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
	*hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, *hRC);
	ReleaseDC(hwnd, hDC);
}
void InitView(int width, int height){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	gluPerspective(20.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluLookAt(6.0f, 6.0f, 8.0f ,0.0f, 0.0f, 0.0f, 0.0f,0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}
void RenderScene(int functionIndex){
	DrawAxis();
	if(functionIndex>=0 && functionIndex<sizeof(drawFunctions)/sizeof(drawFunctions[0])){
		glRotatef(1.5,-1.0,1.0,-1.0);
		drawFunctions[functionIndex]();
	}
}
void DrawAxis(){
	GLfloat varray[][3]={
		2.0f,0.0f,0.0f,
		0.0f,2.0f,0.0f,
		0.0f,0.0f,2.0f,
		0.0f,0.0f,0.0f,
		1.0f,1.0f,1.0f,	
	};
	glColor3fv(varray[4]);
	glLineWidth(0.5f);
	glBegin(GL_LINES);
	glVertex3fv(varray[3]);	glVertex3fv(varray[0]);
	glVertex3fv(varray[3]);	glVertex3fv(varray[1]);
	glVertex3fv(varray[3]);	glVertex3fv(varray[2]);
	glEnd();
	glRasterPos3fv(varray[0]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'x');
	glRasterPos3fv(varray[1]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'y');
	glRasterPos3fv(varray[2]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'z');		
}
void DrawCube(){
	GLfloat vpoint[][3]={
		-0.5,-0.5,-0.5,
		 0.5,-0.5,-0.5,
		 0.5, 0.5,-0.5,
		-0.5, 0.5,-0.5,
		-0.5,-0.5, 0.5,
		 0.5,-0.5, 0.5,
		 0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5,
	};
	GLint faces[][4]={
		0,1,2,3,
		4,5,6,7,
		0,1,5,4,
		3,2,6,7,
		0,3,7,4,
		1,2,6,5,
	};
	float color[][3]={
		1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
	};
	for(int i=0;i<6;i++){
		glBegin(GL_POLYGON);
		glColor3fv(color[i]);
		for(int j=0;j<4;j++){
			glVertex3fv(vpoint[faces[i][j]]);
		}
		glEnd();
	}			
}
void DrawTetrahedron(){
	GLfloat vp[][3]={
		-2*sqrt(2)/3,              0,       -1.0/3,
		   sqrt(2)/3,      sqrt(6)/3,       -1.0/3,
		   sqrt(2)/3,     -sqrt(6)/3,       -1.0/3,
				0.0f,           0.0f,         1.0f,
	};
	int faces[][3]={
		0,1,2,
		0,1,3,
		0,2,3,
		1,2,3,
	};
	GLfloat vcolor[][3]={ 
		1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
	};
	for(int i=0;i<4;i++){
		glBegin(GL_POLYGON);
		glColor3fv(vcolor[i]);
		for(int j=0;j<3;j++){
			glVertex3fv(vp[faces[i][j]]);
		}
		glEnd();
	}
}
void DrawOctahedron(){
	GLfloat vp[][3]={
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f, -1.0f,
		 1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
	};
	int faces[][3]={
		0,5,2, 0,2,4, 0,4,3, 0,3,5,
		1,5,2, 1,2,4, 1,4,3, 1,3,5,
	};
	GLfloat vcolor[][3]={ 
		1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.2f, 0.2f, 0.5f,
	};
	for(int i=0;i<8;i++){
		glBegin(GL_POLYGON);
		glColor3fv(vcolor[i]);
		for(int j=0;j<3;j++){
			glVertex3fv(vp[faces[i][j]]);
		}
		glEnd();
	}
}
void DrawDodecahedron(){
	GLfloat phi = (1+sqrt(5))/2;
	GLfloat vp[][3]={
		 0.00,  0.43,  1.11,
		-0.69,  0.69,  0.69,
		-1.11,  0.00,  0.43,
		-0.69, -0.69,  0.69,
		 0.00, -0.43,  1.11,
		 0.69, -0.69,  0.69,
		 1.11,  0.00,  0.43,
		 1.11,  0.00, -0.43,
		 0.69, -0.69, -0.69,
		 0.43, -1.11,  0.00,
		 0.69,  0.69, -0.69,
		 0.43,  1.11,  0.00,
		-0.43,  1.11,  0.00,
		-0.69,  0.69, -0.69,
		 0.00,  0.43, -1.11,
		 0.69,  0.69,  0.69,
		-0.69, -0.69, -0.69,
		 0.00, -0.43, -1.11,
		-1.11,  0.00, -0.43,
		-0.43, -1.11,  0.00,
	};
	int faces[][5]={
		 9,  8, 17, 16, 19,
		16, 17, 14, 13, 18,
		 7, 10, 14, 17,  8,
		10, 14, 13, 12, 11,
		11, 15,  6,  7, 10,
		11, 15,  0,  1, 12,
		 2,  1, 12, 13, 18,
		 2, 18, 16, 19,  3,
		 2,  3,  4,  0,  1,
		 3, 19,  9,  5,  4,
		 5,  4,  0, 15,  6,	
		 9,  8,  7,  6,  5,	
	};
	float *color = (float*)malloc(32*3*sizeof(float));
	for(int i=0;i<32*3;i++){
		color[i]=(float)rand()/RAND_MAX;
	}
	for(int i=0;i<12;i++){
		//glBegin(GL_LINE_LOOP);
		glColor3f(color[i*3],color[i*3+1],color[i*3+2]);
		glBegin(GL_POLYGON);
		for(int j=0;j<5;j++){
			glVertex3fv(vp[faces[i][j]]);
		}
		glEnd();
	}
	glColor3f(1.0f,1.0f,1.0f);
	for(int i=0;i<20;i++){		
		glRasterPos3fv(vp[i]);
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 65+i);
	}
	free(color);	
}
void DrawIcosahedron(){
	GLfloat m=sqrt(50-10*sqrt(5))/10;
	GLfloat n=sqrt(50+10*sqrt(5))/10;
	GLfloat vp[][3]={
		// xz 
		   -m,   0.0f,      n,
			m,   0.0f,      n,
			m,   0.0f,     -n,
		   -m,   0.0f,     -n,
		// yz    
		 0.0f,      n,      m,
		 0.0f,      n,     -m,
		 0.0f,     -n,     -m, 
		 0.0f,     -n,      m,
		 // xy
			n,      m,   0.0f,
			n,     -m,   0.0f,    
		   -n,     -m,   0.0f,  
		   -n,      m,   0.0f,
	};

	int faces[][3]={
		4,0,1, 7,0,1, 6,2,3, 5,2,3, 8,4,5, 11,4,5, 9,6,7, 10,6,7, 1,8,9, 2,8,9, 0,10,11, 3,10,11,  0,11,4, 0,10,7, 1,8,4, 1,9,7, 2,6,9, 2,5,8, 3,6,10, 3,11,5, 
	};	

	int plane[][4]={
		0,1,2,3, 4,5,6,7,  8,9,10,11,
	};
	float *color = (float*)malloc(32*3*sizeof(float));
	for(int i=0;i<32*3;i++){
		color[i]=(float)rand()/RAND_MAX;
	}	
	for(int i=0;i<3;i++){
		glColor3f(color[i*3],color[i*3+1],color[i*3+2]);
		glBegin(GL_POLYGON);
		for(int j=0;j<4;j++){
			glVertex3fv(vp[plane[i][j]]);
		}
		glEnd();
	}
	glColor3f(1.0f,1.0f,0.0f);
	for(int i=0;i<12;i++){
		glRasterPos3fv(vp[i]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 65+i);
	}
	free(color);
}
int findPt(POINT3 *pt, char *pname){
	int u=-1;
	for(int i=0;i<60;i++){
		if(strcmp(pt[i].name,pname)==0){u=i;break;}
	}
	return u;
}
void body60(Vector3 *vpoint,Face5 *face5, Face6 *face6){
	char* adj="BELKHAHJIEDGJIFCFLKGBIFLACIELDCDKHJABJGKBEFCJHBICGAHGDLAKDFE";//60条线段 
	POINT3 pt[60];
	float m=sqrt(50-10*sqrt(5))/10;
	float n=sqrt(50+10*sqrt(5))/10;	
	float vp[][3]={
		   -m,   0.0f,      n,  // xz 
			m,   0.0f,      n,
			m,   0.0f,     -n,
		   -m,   0.0f,     -n,
		 0.0f,      n,      m,  // yz    
		 0.0f,      n,     -m,
		 0.0f,     -n,     -m, 
		 0.0f,     -n,      m,
			n,      m,   0.0f, // xy
			n,     -m,   0.0f,    
		   -n,     -m,   0.0f,  
		   -n,      m,   0.0f,
	};
	int face[][3]={
		 0, 1, 4,    0, 4, 11,   0, 11, 10,   0, 10,  7,    0,  7,  1,  
		 9, 7, 1,    9,  8, 1,   9,  8,  2,   9,  2,  6,    9,  6,  7, 
		 5, 2, 8,    5, 8,  4,   5,  4, 11,   5, 11,  3,    5,  3,  2, 
		 1, 8, 4,    3, 6, 10,   2,  3,  6,   3, 11, 10,    6,  7, 10,
	};	
	for(int i=0;i<12;i++){
		for(int j=0;j<5;j++){
			vpoint[i*5+j].x=pt[i*5+j].x=(vp[i][0]*2+vp[adj[i*5+j]-65][0])/3;
			vpoint[i*5+j].y=pt[i*5+j].y=(vp[i][1]*2+vp[adj[i*5+j]-65][1])/3;
			vpoint[i*5+j].z=pt[i*5+j].z=(vp[i][2]*2+vp[adj[i*5+j]-65][2])/3;
			pt[i*5+j].name[0]=i+65;
			pt[i*5+j].name[1]=adj[i*5+j];
			pt[i*5+j].name[2]='\0';
			face5[i].indices[j]=i*5+j;
		}
	}
	for(int i=0;i<20;i++){
		int u;
		char pname[3]={0};
		pname[0]=65+face[i][0],    pname[1]=65+face[i][1];     u=findPt( pt, pname ); face6[i].indices[0]=u;
		pname[0]=65+face[i][1],    pname[1]=65+face[i][0];     u=findPt( pt, pname ); face6[i].indices[1]=u;
		pname[0]=65+face[i][1],    pname[1]=65+face[i][2];     u=findPt( pt, pname ); face6[i].indices[2]=u;
		pname[0]=65+face[i][2],    pname[1]=65+face[i][1];     u=findPt( pt, pname ); face6[i].indices[3]=u;
		pname[0]=65+face[i][2],    pname[1]=65+face[i][0];     u=findPt( pt, pname ); face6[i].indices[4]=u;
		pname[0]=65+face[i][0],    pname[1]=65+face[i][2];     u=findPt( pt, pname ); face6[i].indices[5]=u;
	}
}
void DrawFootball(){
	Vector3 *vp;
	Face5 *face5;
	Face6 *face6;
	float *color;
	vp = (Vector3*)malloc(60*sizeof(Vector3));
	face5=(Face5*)malloc(12*sizeof(Face5));
	face6=(Face6*)malloc(60*sizeof(Face6));
	body60( vp, face5, face6 );	
	color = (float*)malloc(32*3*sizeof(float));
	for(int i=0;i<32*3;i++){
		color[i]=(float)rand()/RAND_MAX;
	}
	for(int i=0;i<12;i++){
		glColor3f(color[i*3],color[i*3+1],color[i*3+2]);
		glBegin(GL_POLYGON);
		for(int j=0;j<5;j++){
			glVertex3f(vp[face5[i].indices[j]].x,vp[face5[i].indices[j]].y,vp[face5[i].indices[j]].z);
		}
		glEnd();
	}
	for(int i=0;i<20;i++){
		glColor3f(color[i*3+36],color[i*3+37],color[i*3+38]);
		glBegin(GL_POLYGON);
		for(int j=0;j<6;j++){
			glVertex3f(vp[face6[i].indices[j]].x,vp[face6[i].indices[j]].y,vp[face6[i].indices[j]].z);
		}
		glEnd();
	}
	free(vp);
	free(face5);
	free(face6);
}
