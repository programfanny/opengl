//gcc wingl.c -o wingl.exe -mwindows -lopengl32 -lglu32 -lglut32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#define PI 3.141592653589732
#define ID_EDIT 401
#define ID_LIST 402
typedef struct _Point3{
	char name[3];
	float x,y,z;
}POINT3;
int findPt(POINT3 *pt, char *pname){
	int u=-1;
	for(int i=0;i<60;i++){
		if(strcmp(pt[i].name,pname)==0){u=i;break;}
	}
	return u;
}

typedef struct {
	double x, y, z;
} Vector3;

typedef struct {
    int indices[5];
} Face5;

typedef struct {
    int indices[6];
} Face6;

// Function to calculate the cross product of two vectors
Vector3 cross(Vector3 v1, Vector3 v2) {
    Vector3 result = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
    return result;
}

// Function to normalize a vector
Vector3 normalize(Vector3 v) {
    double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return (Vector3){v.x / length, v.y / length, v.z / length};
}

// Function to add two vectors
Vector3 add(Vector3 v1, Vector3 v2) {
    return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

// Function to calculate the dot product of two vectors
double dot(Vector3 v1, Vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Function to calculate the magnitude of a vector
double magnitude(Vector3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Function to calculate the scalar multiplication of a vector
Vector3 scalarMultiply(double scalar, Vector3 v) {
    return (Vector3){scalar * v.x, scalar * v.y, scalar * v.z};
}

// Function to calculate the point P such that P3, P4, P5 form an equilateral triangle
Vector3 P32P(Vector3 P1, Vector3 P2, Vector3 P3) {
	
    Vector3 v1 = {P2.x - P1.x, P2.y - P1.y, P2.z - P1.z};
    Vector3 v2 = {P2.x - P3.x, P2.y - P3.y, P2.z - P3.z};
    Vector3 v3 = cross(v1, v2);
    v3 = normalize(v3);
    double C = 1 / (2 * cos(54 * PI / 180));
    double S = sqrt(1 - C * C);
    Vector3 v4 = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    v4 = normalize(v4);
    v4 = scalarMultiply(C, v4);
    Vector3 P = add(add(P2, v4), scalarMultiply(S, v3));
    return P;
}
/*
void body60(Vector3 *vpoint, Face5 *face5, Face6 *face6) {
    int i, j;
    double phi = PI / 5;
    double theta = 2 * PI / 6;
	for (i = 0; i < 12; i++) {
        double lon = i * theta - PI / 2;
        for (j = 0; j < 5; j++) {
            double lat = j * phi;
            int idx = i * 5 + j;
            vpoint[idx].x = cos(lon) * sin(lat);
            vpoint[idx].y = sin(lon) * sin(lat);
            vpoint[idx].z = cos(lat);
        }
    }

    // Calculate the remaining points using P32P function
    // This is a simplified example and actual calculations would be more complex
    for (i = 0; i < 12; i++) {
        vpoint[12 + i] = P32P(vpoint[12 * 5], vpoint[12 * 5 + 1], vpoint[12 * 5 + 2]);
    }

    // Assign face indices for pentagons and hexagons
    // This is a simplified example and actual calculations would be more complex
    for (i = 0; i < 12; i++) {
        face5[i].indices[0] = i * 5;
        face5[i].indices[1] = (i * 5 + 1) % (12 * 5);
        face5[i].indices[2] = (i * 5 + 2) % (12 * 5);
        face5[i].indices[3] = (i * 5 + 3) % (12 * 5);
        face5[i].indices[4] = (i * 5 + 4) % (12 * 5);
    }

    for (i = 0; i < 20; i++) {
        face6[i].indices[0] = 12 * 5 + i * 6;
        face6[i].indices[1] = (12 * 5 + i * 6 + 1) % (20 * 6);
        face6[i].indices[2] = (12 * 5 + i * 6 + 2) % (20 * 6);
        face6[i].indices[3] = (12 * 5 + i * 6 + 3) % (20 * 6);
        face6[i].indices[4] = (12 * 5 + i * 6 + 4) % (20 * 6);
        face6[i].indices[5] = (12 * 5 + i * 6 + 5) % (20 * 6);
    }
}*/
// Caculate the points and faces on football by truncating Regular Icosahedron.
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
float angle=0.0;

WNDPROC OriginalEditProc;
void RenderScene(float angle, Vector3* vpoint,Face5* face5,Face6* face6, float color[][3]);
void DrawAxis();
void DrawCubeOld();
void DrawCircleX(float radius);
void DrawCircleY(float radius);
void DrawCircleZ(float radius);
void DrawTetrahedron();
void DrawOctahedron();
void DrawDodecahedron();
void DrawIcosahedron(float color[][3]);
void DrawIcosahedron2(float color[][3]);
void DrawIcosahedron3(float color[][3]);
void DrawFootball(Vector3 *vp, Face5* face5,Face6* face6, float color[][3]);
void DrawFootball2();
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
			SetTimer(hwnd, 1, 250, NULL);
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			// GetWindowRect(hListBox, &rect);
			// sprintf(buf, "(%d %d)-(%d %d)",rect.left,rect.top,rect.right,rect.bottom);
			// TextOut(hdc,10,10,buf,strlen(buf));
			// sprintf(buf, "width=%d, height=%d",rect.right-rect.left,rect.bottom-rect.top);
			// TextOut(hdc,10,30,buf,strlen(buf));
			EndPaint(hwnd, &ps);
			return 0;
		case WM_COMMAND:
			if(wParam==3){
				SendMessage(hListBox,LB_ADDSTRING,0,lParam);
			}
			return 0;
		case WM_TIMER:
			angle += 1;
			PostMessage(glhwnd, WM_PAINT, (WPARAM)10, (LPARAM)angle);
			// hdc = GetDC(hwnd);
			// sprintf(buf,"angle = %5.2f", angle);
			// TextOut(hdc,10,100,buf,strlen(buf));
			// ReleaseDC(hwnd,hdc);
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
			KillTimer(hwnd,1);
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
	float angle;
	static float color[100][3];
	static Vector3 *vp;
	static Face5* face5;
	static Face6* face6;
	switch(message){
		case WM_CREATE:
			InitOpenGL(hwnd, &hRC);
			GetWindowRect(hwnd, &rect);
			InitView( rect.right-rect.left, rect.bottom-rect.top );
	
			for(int i=0;i<100;i++){
				for(int j=0;j<3;j++){
					color[i][j]=(double)rand()/RAND_MAX;
				}
			}
			vp = (Vector3*)malloc(60*sizeof(Vector3));
			face5=(Face5*)malloc(12*sizeof(Face5));
			face6=(Face6*)malloc(60*sizeof(Face6));
			body60( vp, face5, face6 );
			return 0;
		case WM_PAINT:
			//if(wParam==10){MessageBox(NULL,"kk","KK",MB_OK);}
			angle = (float)lParam;
			hdc = BeginPaint(hwnd, &ps);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderScene(angle, vp,face5,face6,color);
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
	gluPerspective(30.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluLookAt(2.0f, 2.0f, 5.0f ,0.0f, 0.0f, 0.0f, 0.0f,0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}
void RenderScene(float angle, Vector3* vpoint, Face5* face5,Face6* face6,float color[][3]){
	glRotatef(0.3, 0.0f, 1.0f, 0.0f);
	DrawAxis();
	//glScalef(0.5f,0.5f,0.5f);
	//DrawTetrahedron();
	//DrawOctahedron();
	//glBegin(GL_POLYGON);
	// glColor3f(0.5f,0.5f,0.0f);
	// glVertex3f(0.0f, 0.0f, 0.0f ); 
	// glVertex3f( 0.0f, 1.0f, 0.0f ); 
	// glVertex3f( 0.0f,  0.0f, 1.0f ); 

	// glColor3f( 0.5f,0.0f,0.5f);
	// glVertex3f( 0.0f, 0.0f, 0.0f ); 
	// glVertex3f( 1.0f, 0.0f, 0.0f ); 
	// glVertex3f( 0.0f, 0.0f, 1.0f ); 
	// glColor3f( 0.0f,0.5f,0.5f);
	// glVertex3f( 0.0f, -1.0f, 0.0f ); 
	// glVertex3f( 1.0f, 0.0f, 0.0f ); 
	// glVertex3f( 0.0f, 1.0f, 0.0f );
	// glVertex3f( -1.0f, 0.0f, 0.0f ); 
	// glEnd();
	// DrawIcosahedron2();
	// DrawIcosahedron3(color);
	// DrawIcosahedron(color);

	// glColor3f(0.0f,1.0f,0.0f);
	// glBegin(GL_LINES);
	// glVertex3f(0.0f,0.0f,0.0f);
	// glVertex3f(1.5f, 0.9f, 3.0f);
	// glVertex3f(0.0f, 0.0f,0.0f);
	// glVertex3f(0.0f, 6.0f, 1.2f);
	// glEnd();
	/*
	glPushMatrix();
	glRotatef(4*angle, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f,-2.5f, 0.0f);
	glScalef(0.8f,0.8f,0.8f);	

	//glutSolidTorus(0.5f,1.0f,20,24);
	// glutWireDodecahedron();
	//glutWireOctahedron();
	//DrawOctahedron();
	//DrawTetrahedron();
	//DrawDodecahedron();
	//DrawIcosahedron();
	DrawIcosahedron2();

	glPopMatrix();
	*/	
	//glPushMatrix();
	//glRotatef(4*angle, 0.0f, 0.0f, 1.0f);
	//glTranslatef(2.0f,0.0f,0.0f);
	//glScalef(0.5f,0.5f,0.5f);	

	//glutSolidTorus(0.5f,1.0f,20,24);
	// glutWireDodecahedron();
	// glutWireOctahedron();
	// DrawOctahedron();
	// DrawTetrahedron();
	// DrawDodecahedron();
	// DrawIcosahedron3(color);
	DrawFootball(vpoint,face5,face6,color);

	//glPopMatrix();

	// glPushMatrix();
	// glRotatef(angle, 0.0f, 1.0f, 0.0f);
	// glTranslatef(-0.3f,1.5f,-0.7f);
	// glScalef(0.5f,0.5f,0.5f);	
	// glBegin(GL_TRIANGLES);
	// glVertex3f(-0.5f, -0.5f, 0.0f );   glColor3f( 1.0f, 0.0f, 0.0f );
	// glVertex3f( 0.5f, -0.5f, 0.0f );   glColor3f( 0.0f, 1.0f, 0.0f );
	// glVertex3f( 0.0f,  0.5f, 0.0f );   glColor3f( 0.0f, 0.0f, 1.0f );
	// glEnd();
	// DrawCircleX(0.7); glColor3f( 1.0f, 0.0f, 0.0f );
	// DrawCircleY(0.7); glColor3f( 0.0f, 1.0f, 0.0f );
	// DrawCircleZ(0.7); glColor3f( 0.0f, 0.0f, 1.0f );
	// glColor3f( 1.0f, 1.0f, 1.0f );
	// glPointSize(3.0f);
	// glBegin(GL_POINTS);
	// // glVertex3f(0.0,0.0,0.0); 
	// glVertex3f(0.7*cos(angle*3.1415926535897/180),0.7*sin(angle*3.1415926535897/180),0.0);
	// // glVertex3f(0.0,0.0,0.0); 
	// glVertex3f(0.7*sin((angle+45)*3.1415926535897/180),0.0,0.7*cos((angle+45)*3.1415926535897/180));
	// // glVertex3f(0.0,0.0,0.0); 
	// glVertex3f(0.0,0.7*sin((angle+90)*3.1415926535897/180),0.7*cos((angle+90)*3.1415926535897/180));
	// glEnd();
	// glScalef(0.2f,0.2f,0.2f);
	// DrawCubeOld();
	// glPopMatrix();
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
void DrawCircleZ(float radius){
	int circle_points=24;
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<circle_points;i++){
		float angle=2*3.1415926535898*i/circle_points;
		glVertex3f(radius*cos(angle), radius*sin(angle), 0.0f);
	}
	glEnd();
}
void DrawCircleX(float radius){
	int circle_points=24;
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<circle_points;i++){
		float angle=2*3.1415926535898*i/circle_points;
		glVertex3f(0.0f, radius*cos(angle), radius*sin(angle));
	}
	glEnd();
}
void DrawCircleY(float radius){
	int circle_points=24;
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<circle_points;i++){
		float angle=2*3.1415926535898*i/circle_points;
		glVertex3f(radius*cos(angle), 0.0f, radius*sin(angle));
	}
	glEnd();
}
// 正方体
void DrawCubeOld(){
	GLfloat varray[][3]={
		-1.0,-1.0,-1.0,
		 1.0,-1.0,-1.0,
		 1.0, 1.0,-1.0,
		-1.0, 1.0,-1.0,
		-1.0,-1.0, 1.0,
		 1.0,-1.0, 1.0,
		 1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		 1.0, 0.0, 0.0,
		 0.0, 1.0, 0.0,
	};
	GLint face[][4]={

	};
	glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    glColor3fv(varray[8]); // 红色
    glVertex3fv(varray[4]);
    glVertex3fv(varray[5]);
    glVertex3fv(varray[6]);
    glVertex3fv(varray[7]);
     // 后面
    glColor3fv(varray[9]); // 绿色
    glVertex3fv(varray[0]);
    glVertex3fv(varray[3]);
    glVertex3fv(varray[2]);
    glVertex3fv(varray[1]);
    // 左面
    glColor3f(0.0f, 0.0f, 1.0f); // 蓝色
    glVertex3fv(varray[4]);
    glVertex3fv(varray[7]);
    glVertex3fv(varray[3]);
    glVertex3fv(varray[0]); 
    // 右面
    glColor3f(1.0f, 1.0f, 0.0f); // 黄色
    glVertex3fv(varray[1]);
    glVertex3fv(varray[2]);
    glVertex3fv(varray[6]);
    glVertex3fv(varray[5]);
    
    // 顶面
    glColor3f(1.0f, 0.0f, 1.0f); // 紫色
    glVertex3fv(varray[7]);
    glVertex3fv(varray[3]);
    glVertex3fv(varray[2]);
    glVertex3fv(varray[6]);
    
    // 底面
    glColor3f(0.0f, 1.0f, 1.0f); // 青色
    glVertex3fv(varray[0]);
    glVertex3fv(varray[1]);
    glVertex3fv(varray[5]);
    glVertex3fv(varray[4]);
    
    glEnd();	
}
// 正四面体
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
// 正八面体
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
		0.0f, 0.0f, 0.0f,
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
// 正十二面体
void DrawDodecahedron(){
	GLfloat phi = (1+sqrt(5))/2;
	GLfloat vp[][3]={
		//(0,±φ,±1/φ)
		0.0f,   phi,  1/phi,
		0.0f,  -phi,  1/phi,
		0.0f,  -phi, -1/phi,
		0.0f,   phi, -1/phi,
		//(±1/φ,0,±φ)
		1/phi,  0.0f,  phi,
	 -1/phi,  0.0f,  phi,
	 -1/phi,  0.0f, -phi,
		1/phi,  0.0f, -phi,
		//(±φ,±1/φ,0)
		phi,1,0,
	 -phi,1,0,
	 -phi,-1,0,
		phi,-1,0,
		//(±1,±1,±1)
		  1,  1,  1,
		 -1,  1,  1,
		  1, -1,  1,
		 -1, -1,  1,
		  1,  1, -1,
		 -1,  1, -1,
		  1, -1, -1,
		 -1, -1, -1,
	};
	int faces[][5]={
		5, 13, 9, 10, 15,
		5, 4, 12,  0, 13,
		0,12,8,16,3,
		16,7,6,17,3,
		6,19,10,9,17,
		17,9,13,0,3,
		5,4,14,1,15,
		14,4,12,8,11,
		11,8,16,7,18,
		18,7,6,19,2,
		2,19,10,5,1,
		1,14,11,18,2,
	};
	GLfloat vcolor[][3]={ 
		1.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.8f, 0.8f, 0.8f,
		1.0f, 0.5f, 0.5f,
		0.5f, 1.0f, 0.5f,
		0.5f, 0.5f, 1.0f,
		0.0f, 0.5f, 0.5f,
	};
	for(int i=0;i<12;i++){
		glBegin(GL_POLYGON);
		glColor3fv(vcolor[i]);
		for(int j=0;j<5;j++){
			glVertex3fv(vp[faces[i][j]]);
		}
		glEnd();
	}	
}
// 正二十面体
void DrawIcosahedron(float vcolor[][3]){
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
	/*	
	for(int i=0;i<20;i++){
		glBegin(GL_POLYGON);
		glColor3fv(vcolor[i]);
		for(int j=0;j<3;j++){
			glVertex3fv(vp[faces[i][j]]);
		}
		glEnd();
	}
	glColor3f(1.0f,1.0f,0.0f);
	for(int i=0;i<12;i++){
		glRasterPos3fv(vp[i]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 65+i);
	}*/
    int A[12][12]={0};
    for(int i=0;i<sizeof(faces)/sizeof(faces[0]);i++){
        int v1 = faces[i][0];
        int v2 = faces[i][1];
        int v3 = faces[i][2];

        A[v1][v2]=1;A[v2][v1]=1;
        A[v2][v3]=1;A[v3][v2]=1;
        A[v3][v1]=1;A[v1][v3]=1;
    }
    GLfloat footballPoint[60][3];
    glColor3f(1.0f,1.0f,1.0f);
    glPointSize(4.0);
    for(int i=0;i<12;i++){
    	int k=0;
    	glBegin(GL_POLYGON);
    	//glBegin(GL_POINTS);
    	//glBegin(GL_LINES);
    	glColor3fv(vcolor[i]);
        for(int j=0;j<12;j++){
            if(A[i][j]){
            	footballPoint[i*5+k][0]=(vp[i][0]*2+vp[j][0])/3;
				footballPoint[i*5+k][1]=(vp[i][1]*2+vp[j][1])/3;
				footballPoint[i*5+k][2]=(vp[i][2]*2+vp[j][2])/3;
				//glVertex3f(0.0f,0.0f,0.0f);
            	glVertex3fv(footballPoint[i*5+k]);
            	k++;
            }
        }
        glEnd();
    }
}
void DrawIcosahedron2(float vcolor[][3]){
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
	for(int i=0;i<3;i++){
		glColor3fv(vcolor[i+2]);
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
}
void DrawIcosahedron3(float vcolor[][3]){
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
	glColor3f(0.5f,0.1f,0.5f);	
	for(int i=0;i<20;i++){
		glBegin(GL_LINE_LOOP);
		// glColor3fv(vcolor[i]);
		for(int j=0;j<3;j++){
			glVertex3fv(vp[faces[i][j]]);
		}
		glEnd();
	}
	glColor3f(1.0f,1.0f,0.0f);
	for(int i=0;i<12;i++){
		glRasterPos3fv(vp[i]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 65+i);
	}
	int plane[][4]={
		0,1,2,3,  4,5,6,7,  8,9,10,11,
	};
	
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2,0X3333);
	// glLineWidth(2.0);
	glColor3fv(vcolor[2]);
	for(int i=0;i<3;i++){
		
		glBegin(GL_LINE_LOOP);
		//glBegin(GL_POLYGON);
		for(int j=0;j<4;j++){
			glVertex3fv(vp[plane[i][j]]);
		}
		glEnd();
	}
	glDisable( GL_LINE_STIPPLE );
	// glLineWidth(1.0);
	glColor3f(1.0f,1.0f,0.0f);
	for(int i=0;i<12;i++){
		glRasterPos3fv(vp[i]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 65+i);
	}	
	/*	
    int A[12][12]={0};
    for(int i=0;i<sizeof(faces)/sizeof(faces[0]);i++){
        int v1 = faces[i][0];
        int v2 = faces[i][1];
        int v3 = faces[i][2];

        A[v1][v2]=1;A[v2][v1]=1;
        A[v2][v3]=1;A[v3][v2]=1;
        A[v3][v1]=1;A[v1][v3]=1;
    }
    GLfloat footballPoint[60][3];
    glColor3f(1.0f,1.0f,1.0f);
    glPointSize(4.0);
    for(int i=0;i<12;i++){
    	int k=0;
    	glBegin(GL_POLYGON);
    	//glBegin(GL_POINTS);
    	//glBegin(GL_LINES);
    	glColor3fv(vcolor[i]);
        for(int j=0;j<12;j++){
            if(A[i][j]){
            	footballPoint[i*5+k][0]=(vp[i][0]*2+vp[j][0])/3;
				footballPoint[i*5+k][1]=(vp[i][1]*2+vp[j][1])/3;
				footballPoint[i*5+k][2]=(vp[i][2]*2+vp[j][2])/3;
				//glVertex3f(0.0f,0.0f,0.0f);
            	glVertex3fv(footballPoint[i*5+k]);
            	k++;
            }
        }
        glEnd();
    }*/
}

void DrawFootball(Vector3 *vp, Face5* face5,Face6* face6, float color[][3]){

	body60(vp,face5,face6);
	
	for(int i=0;i<12;i++){
		glColor3fv(color[i]);
		glBegin(GL_POLYGON);
		for(int j=0;j<5;j++){
			glVertex3f(vp[face5[i].indices[j]].x,vp[face5[i].indices[j]].y,vp[face5[i].indices[j]].z);
		}
		glEnd();
	}
	
	for(int i=0;i<60;i++){
		glColor3fv(color[i+12]);
		glBegin(GL_POLYGON);
		for(int j=0;j<6;j++){
			glVertex3f(vp[face6[i].indices[j]].x,vp[face6[i].indices[j]].y,vp[face6[i].indices[j]].z);
		}
		glEnd();
	}
}
/*
typedef struct tagPIXELFORMATDESCRIPTOR {
  WORD  nSize;
  WORD  nVersion;
  DWORD dwFlags;
  BYTE  iPixelType;
  BYTE  cColorBits;
  BYTE  cRedBits;
  BYTE  cRedShift;
  BYTE  cGreenBits;
  BYTE  cGreenShift;
  BYTE  cBlueBits;
  BYTE  cBlueShift;
  BYTE  cAlphaBits;
  BYTE  cAlphaShift;
  BYTE  cAccumBits;
  BYTE  cAccumRedBits;
  BYTE  cAccumGreenBits;
  BYTE  cAccumBlueBits;
  BYTE  cAccumAlphaBits;
  BYTE  cDepthBits;
  BYTE  cStencilBits;
  BYTE  cAuxBuffers;
  BYTE  iLayerType;
  BYTE  bReserved;
  DWORD dwLayerMask;
  DWORD dwVisibleMask;
  DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR, *PPIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;
nSize
Specifies the size of this data structure. This value should be set to sizeof(PIXELFORMATDESCRIPTOR).

nVersion
Specifies the version of this data structure. This value should be set to 1.

dwFlags
A set of bit flags that specify properties of the pixel buffer. The properties are generally not mutually exclusive; you can set any combination of bit flags, with the exceptions noted. The following bit flag constants are defined.
Value	Meaning
PFD_DRAW_TO_WINDOW = 0x00000004
The buffer can draw to a window or device surface.
PFD_DRAW_TO_BITMAP = 0x00000008
The buffer can draw to a memory bitmap.
PFD_SUPPORT_GDI = 0x00000010
The buffer supports GDI drawing. This flag and PFD_DOUBLEBUFFER are mutually exclusive in the current generic implementation.
PFD_SUPPORT_OPENGL = 0x00000020
The buffer supports OpenGL drawing.
PFD_GENERIC_ACCELERATED = 0x00001000
The pixel format is supported by a device driver that accelerates the generic implementation. If this flag is clear and the PFD_GENERIC_FORMAT flag is set, the pixel format is supported by the generic implementation only.
PFD_GENERIC_FORMAT = 0x00000040
The pixel format is supported by the GDI software implementation, which is also known as the generic implementation. If this bit is clear, the pixel format is supported by a device driver or hardware.
PFD_NEED_PALETTE = 0x00000080
The buffer uses RGBA pixels on a palette-managed device. A logical palette is required to achieve the best results for this pixel type. Colors in the palette should be specified according to the values of the cRedBits, cRedShift, cGreenBits, cGreenShift, cBluebits, and cBlueShift members. The palette should be created and realized in the device context before calling wglMakeCurrent.
PFD_NEED_SYSTEM_PALETTE = 0x00000100
Defined in the pixel format descriptors of hardware that supports one hardware palette in 256-color mode only. For such systems to use hardware acceleration, the hardware palette must be in a fixed order (for example, 3-3-2) when in RGBA mode or must match the logical palette when in color-index mode.When this flag is set, you must call SetSystemPaletteUse in your program to force a one-to-one mapping of the logical palette and the system palette. If your OpenGL hardware supports multiple hardware palettes and the device driver can allocate spare hardware palettes for OpenGL, this flag is typically clear.
This flag is not set in the generic pixel formats.

PFD_DOUBLEBUFFER = 0x00000001
The buffer is double-buffered. This flag and PFD_SUPPORT_GDI are mutually exclusive in the current generic implementation.
PFD_STEREO = 0x00000002
The buffer is stereoscopic. This flag is not supported in the current generic implementation.
PFD_SWAP_LAYER_BUFFERS = 0x00000800
Indicates whether a device can swap individual layer planes with pixel formats that include double-buffered overlay or underlay planes. Otherwise all layer planes are swapped together as a group. When this flag is set, wglSwapLayerBuffers is supported.
 
You can specify the following bit flags when calling ChoosePixelFormat.

Value	Meaning
PFD_DEPTH_DONTCARE = 0x20000000
The requested pixel format can either have or not have a depth buffer. To select a pixel format without a depth buffer, you must specify this flag. The requested pixel format can be with or without a depth buffer. Otherwise, only pixel formats with a depth buffer are considered.
PFD_DOUBLEBUFFER_DONTCARE = 0x40000000
The requested pixel format can be either single- or double-buffered.
PFD_STEREO_DONTCARE = 0x80000000
The requested pixel format can be either monoscopic or stereoscopic.
 
With the glAddSwapHintRectWIN extension function, two new flags are included for the PIXELFORMATDESCRIPTOR pixel format structure.

Value	Meaning
PFD_SWAP_COPY = 0x00000400
Specifies the content of the back buffer in the double-buffered main color plane following a buffer swap. Swapping the color buffers causes the content of the back buffer to be copied to the front buffer. The content of the back buffer is not affected by the swap. PFD_SWAP_COPY is a hint only and might not be provided by a driver.
PFD_SWAP_EXCHANGE = 0x00000200
Specifies the content of the back buffer in the double-buffered main color plane following a buffer swap. Swapping the color buffers causes the exchange of the back buffer's content with the front buffer's content. Following the swap, the back buffer's content contains the front buffer's content before the swap. PFD_SWAP_EXCHANGE is a hint only and might not be provided by a driver.
iPixelType 
Specifies the type of pixel data. The following types are defined.

Value	Meaning
PFD_TYPE_RGBA = 0
RGBA pixels. Each pixel has four components in this order: red, green, blue, and alpha.
PFD_TYPE_COLORINEX = 1
Color-index pixels. Each pixel uses a color-index value.

cColorBits
Specifies the number of color bitplanes in each color buffer. For RGBA pixel types, it is the size of the color buffer, excluding the alpha bitplanes. For color-index pixels, it is the size of the color-index buffer.

cRedBits
Specifies the number of red bitplanes in each RGBA color buffer.

cRedShift
Specifies the shift count for red bitplanes in each RGBA color buffer.

cGreenBits
Specifies the number of green bitplanes in each RGBA color buffer.

cGreenShift
Specifies the shift count for green bitplanes in each RGBA color buffer.

cBlueBits
Specifies the number of blue bitplanes in each RGBA color buffer.

cBlueShift
Specifies the shift count for blue bitplanes in each RGBA color buffer.

cAlphaBits
Specifies the number of alpha bitplanes in each RGBA color buffer. Alpha bitplanes are not supported.

cAlphaShift
Specifies the shift count for alpha bitplanes in each RGBA color buffer. Alpha bitplanes are not supported.

cAccumBits
Specifies the total number of bitplanes in the accumulation buffer.

cAccumRedBits
Specifies the number of red bitplanes in the accumulation buffer.

cAccumGreenBits
Specifies the number of green bitplanes in the accumulation buffer.

cAccumBlueBits
Specifies the number of blue bitplanes in the accumulation buffer.

cAccumAlphaBits
Specifies the number of alpha bitplanes in the accumulation buffer.

cDepthBits
Specifies the depth of the depth (z-axis) buffer.

cStencilBits
Specifies the depth of the stencil buffer.

cAuxBuffers
Specifies the number of auxiliary buffers. Auxiliary buffers are not supported.

iLayerType
Ignored. Earlier implementations of OpenGL used this member, but it is no longer used.

bReserved
Specifies the number of overlay and underlay planes. Bits 0 through 3 specify up to 15 overlay planes and bits 4 through 7 specify up to 15 underlay planes.

dwLayerMask
Ignored. Earlier implementations of OpenGL used this member, but it is no longer used.

dwVisibleMask
Specifies the transparent color or index of an underlay plane. When the pixel type is RGBA, dwVisibleMask is a transparent RGB color value. When the pixel type is color index, it is a transparent index value.

dwDamageMask
Ignored. Earlier implementations of OpenGL used this member, but it is no longer used.
*/
