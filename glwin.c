#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define ID_TIMER2 2
typedef struct _point3d_{
	union {
		struct {int x, y,z;};
		int cord[3];
	};
}POINT3D;
typedef struct _dothull_{
	POINT3D *points;
	POINT *xypoints,*yzpoints,*xzpoints;
	int *pindex, num_points;
	int *hullxy, num_hullxy;
	int *hullyz, num_hullyz;
	int *hullxz, num_hullxz;
	int *hull3d, num_hull3d;
	int **faces, num_faces;
	int **sides, num_sides;
	int **colors, num_colors;
	int xyz,u,p;
}DOTHULL;

void DrawAxis();

void FreeDotHull(DOTHULL *dothull);

void glShow3DConvex(DOTHULL *dothull);
void glShowPoints(DOTHULL *dothull);

void GetxyConvex(DOTHULL *dothull);
void GetyzConvex(DOTHULL *dothull);
void GetxzConvex(DOTHULL *dothull);
void Get3DConvexA(DOTHULL *dothull);
void Get2DConvexA( POINT *points, int *pindex, int size, int *stack, int *stacktop );
void Get2DConvexB( POINT *points, int *pindex, int size, int *stack, int *stacktop );
void InitView(int width, int height);
void InitOpenGL(HWND hwnd, HGLRC *hRC);
void InitDotHull(DOTHULL *dothull,int num);

void RenderScene(void* param);

void Show2DConvex(HDC hdc, POINT *points, int num, int *stack, int top);
void ShowPlane( HDC hdc, DOTHULL *dothull);
void ShowPoints(HDC hdc, POINT *points, int num);
void ShowPointsData( HDC hdc, DOTHULL *dothull);
void ShowRect(HDC hdc,int left, int top, int right, int bottom);

//int GetConvex( POINT *points, int num, int *stack );

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow){
	static TCHAR szAppName[]=TEXT("HelloWin");
	static TCHAR glszAppName[]=TEXT("glWindow");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName=szAppName;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,TEXT("This program requires Windows NT!"),szAppName,MB_ICONERROR);
		return 0;
	}
	wndclass.lpfnWndProc = glWndProc;
	wndclass.lpszClassName=glszAppName;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,TEXT("This program requires Windows NT!"),szAppName,MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName,TEXT("The Hello Program"),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	// static RECT rect;
	HINSTANCE hInstance;
	static HWND glhwnd;
	static DOTHULL dothull;

	switch(message){
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
			glhwnd = CreateWindow( "glWindow", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 320, 15, 300, 300, hwnd, NULL, hInstance, NULL );
			srand((unsigned)time(NULL));
			InitDotHull(&dothull,30);
			GetxyConvex(&dothull);
			GetyzConvex(&dothull);
			GetxzConvex(&dothull);
			Get3DConvexA(&dothull);
			break;
		case WM_PAINT:
			PostMessage(glhwnd,WM_PAINT,(WPARAM)&dothull,0);
			//GetWindowRect(hwnd, &rect);
			hdc = BeginPaint( hwnd, &ps );
			// ShowPointsData( hdc, &dothull );
			ShowPlane( hdc, &dothull );
			ShowRect(hdc,15,15,315,315);
			EndPaint( hwnd, &ps );

			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					PostMessage(hwnd, WM_DESTROY,0,0);
					break;
				case VK_RETURN:
					InitDotHull(&dothull,30);
					GetxyConvex(&dothull);
					GetyzConvex(&dothull);
					GetxzConvex(&dothull);
					Get3DConvexA(&dothull);
					InvalidateRect(hwnd, NULL, FALSE);
					break;	
				case VK_LEFT:
					dothull.xyz *= 2;
					if(dothull.xyz>8)dothull.xyz=1;
					InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;

		case WM_DESTROY:
			FreeDotHull(&dothull);
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
} 
LRESULT CALLBACK glWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HGLRC hRC;
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;
	static DOTHULL *dothull;
	switch(message){
		case WM_CREATE:
			InitOpenGL(hwnd, &hRC);
			GetWindowRect(hwnd, &rect);
			InitView( rect.right-rect.left, rect.bottom-rect.top );
			dothull=NULL;
			SetTimer(hwnd, ID_TIMER2,250,NULL);

			return 0;
		case WM_PAINT:
			dothull=(DOTHULL*)wParam;
			hdc=BeginPaint(hwnd, &ps);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderScene(dothull);
			SwapBuffers(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_TIMER:
			PostMessage(hwnd, WM_PAINT, (WPARAM)dothull, 0);
			return 0;	
		case WM_DESTROY:
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
	gluPerspective(20.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluLookAt(6.0f, 6.0f, 8.0f ,0.0f, 0.0f, 0.0f, 0.0f,0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}
void glShowConvex3d(DOTHULL *dothull){
	glShowPoints(dothull);
	glShow3DConvex(dothull);
	//glShowConvex3D(dothull);
}
void RenderScene(void* param){
	glRotatef(0.5,1.0,0.4,0.7);
	DrawAxis();
	if(param!=NULL){
		glShowConvex3d(param);
		//glShowHull3D(param);
	}
}
void DrawAxis(){
	GLfloat varray[][3]={
		2.0f,0.0f,0.0f,
		0.0f,2.0f,0.0f,
		0.0f,0.0f,2.0f,
		0.0f,0.0f,0.0f,
		0.5f,0.5f,0.5f,	
	};
	glColor3fv(varray[4]);
	glLineWidth(0.5f);
	glBegin(GL_LINES);
	glVertex3fv(varray[3]);	glVertex3fv(varray[0]);
	glVertex3fv(varray[3]);	glVertex3fv(varray[1]);
	glVertex3fv(varray[3]);	glVertex3fv(varray[2]);
	glEnd();
	// glRasterPos3fv(varray[0]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'x');
	// glRasterPos3fv(varray[1]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'y');
	// glRasterPos3fv(varray[2]);glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'z');		
}
void glShowPoints(DOTHULL *dothull){
	POINT3D *points=dothull->points;
	glBegin(GL_POINTS);
	for(int i=0;i<dothull->num_points;i++){
		glVertex3f(points[i].x/250.0,points[i].y/250.0,points[i].z/250.0);
	}
	glEnd();
}
void FreeDotHull(DOTHULL *dothull){
	if(dothull->points)free(dothull->points);
	if(dothull->pindex)free(dothull->pindex);
	if(dothull->xypoints)free(dothull->xypoints);
	if(dothull->yzpoints)free(dothull->yzpoints);
	if(dothull->xzpoints)free(dothull->xzpoints);
	if(dothull->hullxy)free(dothull->hullxy);
	if(dothull->hullyz)free(dothull->hullyz);
	if(dothull->hullxz)free(dothull->hullxz);
	if(dothull->hull3d)free(dothull->hull3d);
	if(dothull->faces){
		for(int i=0;i<dothull->num_faces;i++){
			free(dothull->faces[i]);
		}
		free(dothull->faces);
	}
	if(dothull->sides){
		for(int i=0;i<dothull->num_sides;i++){
			free(dothull->sides[i]);
		}
		free(dothull->sides);
	}
	if(dothull->colors){
		for(int i=0;i<dothull->num_colors;i++){
			free(dothull->colors[i]);
		}
		free(dothull->colors);
	}
	ZeroMemory(dothull,0);
}
void InitDotHull(DOTHULL *dothull, int num){
	if(dothull->points)FreeDotHull(dothull);
	dothull->num_points=num;
	dothull->points=malloc(num*sizeof(POINT3D));
	dothull->pindex=malloc(num*sizeof(int));
	dothull->xypoints=malloc(num*sizeof(POINT));
	dothull->yzpoints=malloc(num*sizeof(POINT));
	dothull->xzpoints=malloc(num*sizeof(POINT));
	dothull->hullxy=malloc(num*sizeof(int));
	dothull->hullyz=malloc(num*sizeof(int));
	dothull->hullxz=malloc(num*sizeof(int));
	dothull->hull3d=malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		dothull->pindex[i]=i;
		for(int j=0;j<3;j++){
			dothull->points[i].cord[j]=20+rand()%280;
		}
		dothull->xypoints[i].x = dothull->points[i].x;
		dothull->xypoints[i].y = dothull->points[i].y;

		dothull->yzpoints[i].x = dothull->points[i].y;
		dothull->yzpoints[i].y = dothull->points[i].z;

		dothull->xzpoints[i].x = dothull->points[i].x;
		dothull->xzpoints[i].y = dothull->points[i].z;
	}
	dothull->colors=malloc(num*sizeof(int*));
	for(int i=0;i<num;i++){
		dothull->colors[i]=malloc(3*sizeof(int));
		for(int j=0;j<3;j++){
			dothull->colors[i][j]=rand()%256;
		}		
	}
	dothull->xyz=8;
}
void ShowPlane( HDC hdc, DOTHULL *dothull){
	POINT *points;
	int num, top, *stack;
	num=dothull->num_points;
	char buf[100];
	sprintf(buf,"num_faces = %d ",dothull->num_faces);
	TextOut(hdc, 0, 0, buf,strlen(buf));

	switch(dothull->xyz){
		case 1: points=dothull->xypoints; stack=dothull->hullxy; top=dothull->num_hullxy; break;
		case 2: points=dothull->yzpoints; stack=dothull->hullyz; top=dothull->num_hullyz; break;
		case 4: points=dothull->xzpoints; stack=dothull->hullxz; top=dothull->num_hullxz; break;
		case 8: num=0; top=0; break;
	}
	ShowPoints(hdc, points, num);
	Show2DConvex(hdc, points, num, stack, top );
}
void ShowPointsData(HDC hdc,DOTHULL *dothull){
	char buf[100];
	int num=dothull->num_points;
	sprintf(buf,"Num_Points = %d",num);
	TextOut(hdc, 20,0,buf,strlen(buf));
}
void ShowRect(HDC hdc,int left, int top, int right, int bottom){
	MoveToEx(hdc, left, top, NULL);
	LineTo(hdc, right, top);
	LineTo(hdc, right, bottom);
	LineTo(hdc, left, bottom);
	LineTo(hdc, left, top);
}
void GetxyConvex(DOTHULL *dothull){
	Get2DConvexA( dothull->xypoints, dothull->pindex, dothull->num_points, dothull->hullxy, &dothull->num_hullxy );
	//Get2DConvexB( dothull->xypoints, dothull->pindex, dothull->num_points, dothull->hullxy, &dothull->num_hullxy );
}
void GetyzConvex(DOTHULL *dothull){
	Get2DConvexA( dothull->yzpoints, dothull->pindex, dothull->num_points, dothull->hullyz, &dothull->num_hullyz );
	//Get2DConvexB( dothull->yzpoints, dothull->pindex, dothull->num_points, dothull->hullyz, &dothull->num_hullyz );
}
void GetxzConvex(DOTHULL *dothull){
	Get2DConvexA( dothull->xzpoints, dothull->pindex, dothull->num_points, dothull->hullxz, &dothull->num_hullxz);
	//Get2DConvexB( dothull->xzpoints, dothull->pindex, dothull->num_points, dothull->hullxz, &dothull->num_hullxz);
}
int Cross(POINT O, POINT A, POINT B){
	return (A.x-O.x)*(B.y-O.y)-(A.y-O.y)*(B.x-O.x);
}
double Dist(POINT O, POINT A, POINT B){//B--OA
	double xcrs = Cross(O,A,B)*1.0;
	double len = sqrt( (A.x-O.x)*(A.x-O.x) + (A.y-O.y) * (A.y-O.y) );
	return fabs(xcrs/len);
}
void GetHull(POINT *points,int *pindex, int num,int *stack,int *top, int *puse){
	int count=0,len=*top;
	for(int i=0;i<num;i++){count+=puse[i];}
	if(count==num)return;

	int *uuse=malloc(num*sizeof(int));
	for(int i=0;i<num;i++)uuse[i]=len;
	double maxdist,dist;
	int p=len,q;
	for(int k=0;k<len;k++){
		stack[p++]=stack[k];
		maxdist=0.0;q=-1;
		for(int i=0;i<num;i++){
			if(!puse[i]){
				if(Cross(points[stack[k]],points[stack[(k+1)%len]],points[i])>=0){
					uuse[i]--; if(uuse[i]==0){puse[i]=1;}
				}else{
					dist = Dist(points[stack[k]],points[stack[(k+1)%len]],points[i]);
					if(maxdist<dist){
						maxdist=dist; q=i;
					}
				}
			}
		}
		if(q!=-1){
			stack[p++]=q;puse[q]=1;
		}
	} 
	free(uuse);	
	for(int i=0;i<p-len;i++){stack[i]=stack[len+i];}
	*top=p-len;	
	GetHull(points,pindex,num,stack,top,puse);
}
void Get2DConvexA( POINT *points, int *pindex, int size, int *stack, int *stacktop ){
	int top=0,s=1000,t=-1,us,ut;
	int *puse = malloc(size*sizeof(int));
	memset(puse,0,size*sizeof(int));
	for(int i=0;i<size;i++){
		if(s>points[pindex[i]].x){s=points[pindex[i]].x;us=pindex[i];}
		if(t<points[pindex[i]].x){t=points[pindex[i]].x;ut=pindex[i];}		
	}
	stack[0]=us;stack[1]=ut;top=2;puse[us]=1;puse[ut]=1;
	int ti=0;
	GetHull(points,pindex,size,stack,&top,puse);
	free(puse);
	*stacktop = top;
}
void ShowPoints(HDC hdc, POINT *points, int num){
	for(int i=0; i<num; i++){
		MoveToEx( hdc, points[i].x-2, points[i].y-2, NULL ); LineTo( hdc, points[i].x+2, points[i].y+2 );
		MoveToEx( hdc, points[i].x-2, points[i].y+2, NULL ); LineTo( hdc, points[i].x+2, points[i].y-2 );
	}
	char buf[100];
	for(int i=0;i<num;i++){
		sprintf(buf,"P%d[%d, %d] ",i,points[i].x,points[i].y);
		TextOut(hdc,650+i/20*120,i%20*20,buf,strlen(buf));
	}
}
void Show2DConvex(HDC hdc, POINT *points, int num, int *stack, int top){
	if(top<1)return;
	MoveToEx(hdc, points[stack[top-1]].x,points[stack[top-1]].y,NULL);
	for(int i=0;i<top;i++){
		LineTo(hdc,points[stack[i]].x,points[stack[i]].y);
	}
}
int partition(int *pindex, POINT *points, int low, int high) {
	int pivot = pindex[high];
	int i = low - 1;
	for (int j = low; j < high; j++) {
		if(points[pindex[j]].y*points[pivot].x-points[pivot].y*points[pindex[j]].x<0){
			i++;
			int temp = pindex[i];
			pindex[i] = pindex[j];
			pindex[j] = temp;
		}
	}
	int temp = pindex[i + 1];
	pindex[i + 1] = pindex[high];
	pindex[high] = temp;
	return i + 1;
}
void quickSort(int *pindex, POINT *points, int low, int high) {
	if (low < high) {
		int pi = partition(pindex, points, low, high);
		quickSort(pindex, points, low, pi - 1);
		quickSort(pindex, points, pi + 1, high);
	}
}
int sort(POINT *points, int *pindex, int size) {
	quickSort(pindex, points, 0, size - 1);
	return 0; 
}
// Graham Scan and Andrew Monotone algorithms
void Get2DConvexB( POINT *p, int *pindex, int size, int *stack, int *stacktop ) {
	// Andrew's Monotone Chain Algorithm
  	int i, t, top = 0;
	for( i=0;i<size;i++)pindex[i]=i;
	sort(p, pindex, size);
	for(i=0;i<size;i++){stack[i]=pindex[i];}
	/* lower  */
	for ( i = 0; i < size; ++i) {
	  while (top >= 2 && Cross(p[stack[top-2]], p[stack[top-1]], p[pindex[i]]) <= 0) --top;
	  stack[top++] = pindex[i];
	}
	/* upper  */
	for (i = size-2, t = top+1; i >= 0; --i) {
	  while (top >= t && Cross(p[stack[top-2]], p[stack[top-1]], p[pindex[i]]) <= 0) --top;
	  stack[top++] = pindex[i];
	}
	*stacktop = top-1;
}
void glShowLines(DOTHULL *dothull){
	POINT3D *points=dothull->points;
	int **colors=dothull->colors;
	int num = dothull->num_points;
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<num/2;i++){
		glVertex3f(points[i*2+0].x/250.0,points[i*2+0].y/250.0,points[i*2+0].z/250.0);
		glVertex3f(points[i*2+1].x/250.0,points[i*2+1].y/250.0,points[i*2+1].z/250.0);
	}
	glEnd();
}
void glShowTriangles(DOTHULL *dothull){
	POINT3D *points=dothull->points;
	int **colors=dothull->colors;
	glBegin(GL_TRIANGLES);
	for(int i=0;i<dothull->num_points/3;i++){	
		glColor3f(colors[i][0]/256.0,colors[i][1]/256.0,colors[i][2]/256.0);
		glVertex3f(points[i*3+0].x/250.0,points[i*3+0].y/250.0,points[i*3+0].z/250.0);
		glVertex3f(points[i*3+1].x/250.0,points[i*3+1].y/250.0,points[i*3+1].z/250.0);
		glVertex3f(points[i*3+2].x/250.0,points[i*3+2].y/250.0,points[i*3+2].z/250.0);
	}
	glEnd();
}
void glShowConvex(DOTHULL *dothull){
	POINT3D *points=dothull->points;
	int top=dothull->num_faces;
	int **faces=dothull->faces;
	int **colors=dothull->colors;
	glBegin(GL_TRIANGLES);
	for(int i=0;i<top;i++){
		glColor3f(colors[i][0]/256.0,colors[i][1]/256.0,colors[i][2]/256.0);
		for(int j=0;j<3;j++){
			glVertex3f(points[faces[i][j]].x/250.0,points[faces[i][j]].y/250.0,points[faces[i][j]].z/250.0);
		}
	}
	glEnd();
}
void glShow3DConvex(DOTHULL *dothull){
	//glShowLines(dothull);
	//glShowTriangles(dothull);
	glShowConvex(dothull);
}
void Get3DConvexA(DOTHULL *dothull){
	POINT3D *points=dothull->points;
	int num=dothull->num_points;
	int *pindex=dothull->pindex;
	int **colors=dothull->colors;
	int **faces=malloc(num*sizeof(int*));
	for(int i=0;i<num;i++){
		faces[i]=malloc(3*sizeof(int));
		for(int j=0;j<3;j++){
			faces[i][j]=rand()%num;
		}
	}
	//int *stack,top;
	dothull->num_faces=num;
	dothull->faces=faces;
}