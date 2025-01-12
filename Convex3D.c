//gcc cg.c -ocg.exe -mwindows -lopengl32 -lglu32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define eps 1e-8
#define ID_TIMER2 2


typedef struct _point_{
	int x,y,z;
}POINT3D;
typedef struct _face_{
	int a,b,c;
}FACE;
typedef struct _dothull_{
	POINT3D *points;
	POINT *xypoints,*yzpoints,*xzpoints;
	int *pindex, num_points;
	int *hullxy, num_hullxy;
	int *hullyz, num_hullyz;
	int *hullxz, num_hullxz;
	int *hull3d, num_hull3d;
	int **faces, num_faces;
	int **colors, num_colors;
	int xyz,u,p;
}DOTHULL;
typedef int (*cmp_func)(const void *a, const void *b);

void DrawAxis();
void FreeDotHull( DOTHULL *dothull );
void Get2DConvexA( POINT *points, int *pindex, int size, int *stack, int *stacktop );
void Get2DConvexB( POINT *points, int *pindex, int size, int *stack, int *stacktop );

void Get3DConvexA( HWND hwnd, HWND glhwnd, DOTHULL *dothull );
void Get3DConvexB( HWND hwnd, HWND glhwnd, DOTHULL *dothull );
void Get3DConvexC( HWND hwnd, HWND glhwnd, DOTHULL *dothull );

void GetxyConvex( DOTHULL *dothull );
void GetxzConvex( DOTHULL *dothull );
void GetyzConvex( DOTHULL *dothull );

void glShow3DConvex( DOTHULL *dothull );
void glShowPoints( DOTHULL *dothull );
void InitDotHull( DOTHULL *dothull, int num );
void InitOpenGL( HWND hwnd, HGLRC *hRC );
void InitView( int width, int height );
void readin(POINT3D **points,int *num);
void RenderScene( void* param );
void Show2DConvex( HDC hdc, POINT *points, int num, int *stack, int top );
void ShowDothullInfo( HDC hdc, DOTHULL *dothull );
void ShowPlane( HDC hdc, DOTHULL *dothull );
void ShowPoints( HDC hdc, POINT *points, int num );
void ShowPointsData( HDC hdc, DOTHULL *dothull );
void ShowRect( HDC hdc,int left, int top, int right, int bottom );
void SolveConvex(HWND hwnd, HWND glhwnd, DOTHULL *dothull);
void UpdateView(int deltaX, int deltaY);

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK glWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
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
	static int lastX=0, lastY=0, isDragging=0;
	switch(message){
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
			glhwnd = CreateWindow( "glWindow", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 320, 15, 300, 300, hwnd, NULL, hInstance, NULL );
			srand((unsigned)time(NULL));
			InitDotHull(&dothull,30);
			SolveConvex(hwnd, glhwnd, &dothull);
			break;
		case WM_PAINT:
			PostMessage(glhwnd,WM_PAINT,(WPARAM)&dothull,0);
			//GetWindowRect(hwnd, &rect);
			hdc = BeginPaint( hwnd, &ps );
			// ShowPointsData( hdc, &dothull );
			ShowPlane( hdc, &dothull );
			ShowRect(hdc,15,15,315,315);
			ShowDothullInfo(hdc,&dothull);
			EndPaint( hwnd, &ps );
			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					PostMessage(hwnd, WM_DESTROY,0,0);
					break;
				case VK_RETURN:
					InitDotHull(&dothull,30);
					SolveConvex(hwnd, glhwnd,&dothull);
					InvalidateRect(hwnd, NULL, TRUE);
					break;	
				//case VK_F3:
				//	Get3DConvexB(hwnd, glhwnd,&dothull);
				//	InvalidateRect(hwnd, NULL, FALSE);	
				//	break;
				case VK_LEFT:
					dothull.xyz *= 2;
					if(dothull.xyz>8)dothull.xyz=1;
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			return 0;
		case WM_LBUTTONDOWN:
		    // 记录鼠标按下位置
		    lastX = LOWORD(lParam);
		    lastY = HIWORD(lParam);
		    isDragging = TRUE;
		    return 0;
		case WM_MOUSEMOVE:
		    if (isDragging) {
		        int x = LOWORD(lParam);
		        int y = HIWORD(lParam);
		        int deltaX = x - lastX;
		        int deltaY = y - lastY;
		        UpdateView(deltaX, deltaY);
		        lastX = x;
		        lastY = y;
		        // 重绘窗口
		        InvalidateRect(hwnd, NULL, FALSE);
		    }
		    return 0;
		case WM_LBUTTONUP:
		    isDragging = FALSE;
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
void InitLighting() {
    glEnable(GL_DEPTH_TEST); // 启用深度测试
    glEnable(GL_LIGHTING);   // 启用光照
    glEnable(GL_LIGHT0);    // 启用光源0

    // 设置光源0的位置和颜色
    GLfloat light0_position[] = {2.0, 2.0, 2.0, 0.0}; // 位置（x, y, z, w）
    GLfloat light0_diffuse[] = {0.9, 0.3, 0.8, 1.0};  // 漫反射光颜色
    GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0}; // 镜面反射光颜色

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // 设置材料属性
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0}; // 镜面反射颜色
    GLfloat mat_shininess[] = {50.0};              // 光泽度

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
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
	// InitLighting();
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
void UpdateView(int deltaX, int deltaY) {
    static float angleX = 0.0f;
    static float angleY = 0.0f;
    angleX += deltaX * 0.01f;
    angleY += deltaY * 0.01f;
    if (angleY > 1.5f) angleY = 1.5f;
    if (angleY < -1.5f) angleY = -1.5f;
    glRotatef(angleX, 0.0f, 1.0f, 0.0f);
    glRotatef(angleY, 1.0f, 0.0f, 0.0f);
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
void InitDotHull(DOTHULL *dothull, int num){
	if(dothull->points)FreeDotHull(dothull);
	dothull->num_points=num;
	dothull->points=malloc((num+1)*sizeof(POINT3D));
	dothull->pindex=malloc((num+1)*sizeof(int));
	dothull->xypoints=malloc(num*sizeof(POINT));
	dothull->yzpoints=malloc(num*sizeof(POINT));
	dothull->xzpoints=malloc(num*sizeof(POINT));
	dothull->hullxy=malloc(num*sizeof(int));
	dothull->hullyz=malloc(num*sizeof(int));
	dothull->hullxz=malloc(num*sizeof(int));
	dothull->hull3d=malloc(num*sizeof(int));
	readin(&dothull->points,&num);
	for(int i=0;i<num;i++){
		dothull->pindex[i]=i;
		dothull->xypoints[i].x = dothull->points[i].x;
		dothull->xypoints[i].y = dothull->points[i].y;
		dothull->yzpoints[i].x = dothull->points[i].y;
		dothull->yzpoints[i].y = dothull->points[i].z;
		dothull->xzpoints[i].x = dothull->points[i].x;
		dothull->xzpoints[i].y = dothull->points[i].z;
	}
	dothull->num_colors=num;
	dothull->colors=malloc(num*sizeof(int*));
	for(int i=0;i<num;i++){
		dothull->colors[i]=malloc(3*sizeof(int));
		for(int j=0;j<3;j++){
			dothull->colors[i][j]=rand()%256;
		}
	}
	dothull->num_faces=0;
	dothull->faces=NULL;
	dothull->xyz=8;
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
	if(dothull->colors){
		for(int i=0;i<dothull->num_points;i++){
			free(dothull->colors[i]);
		}
		free(dothull->colors);
	}
	ZeroMemory(dothull,0);
}
void SavePoints(POINT3D *points,int num){
    FILE *fp=fopen("tubao.in","a");
    fprintf(fp,"%d\n",num);
    for(int i=0;i<num;i++){
        fprintf(fp,"%d,%d,%d\n",points[i].x,points[i].y,points[i].z);
    }
    fclose(fp);
}
void readin(POINT3D **points,int *num){
    POINT3D *p=*points;
    int count;
    FILE *fp=fopen("tubao.in","r");
    if(fp){
        int x,y,z;
        fscanf(fp,"%d\n",&count);
        free(p);
        p=malloc((count+1)*sizeof(POINT3D));
        for (int i = 0; i < count; i++) {
            fscanf(fp,"%d,%d,%d", &x,&y,&z);
            p[i].x=x; p[i].y=y; p[i].z=z;
        }
        *points=p;
        fclose(fp);
    }else{
        count=*num;
        for(int i=0;i<count;i++){
            p[i].x=20+rand()%280;
            p[i].y=20+rand()%280;
            p[i].z=20+rand()%280;
        }
        SavePoints(p,count);    
    }
    *num=count;
    p[count].x=0;p[count].y=0;p[count].z=0;
}
void SolveConvex(HWND hwnd, HWND glhwnd, DOTHULL *dothull){
	GetxyConvex(dothull);
	GetyzConvex(dothull);
	GetxzConvex(dothull);
	Get3DConvexA(hwnd, glhwnd, dothull);
	//Get3DConvexB(hwnd, glhwnd, dothull);
	//Get3DConvexC(hwnd, glhwnd, dothull);
}
int Cross(POINT O, POINT A, POINT B){
	return (A.x-O.x)*(B.y-O.y)-(A.y-O.y)*(B.x-O.x);
}
POINT3D cross_product(POINT3D a, POINT3D b) {
    return (POINT3D){
        (a.y * b.z - a.z * b.y),
        (a.z * b.x - a.x * b.z),
        (a.x * b.y - a.y * b.x)
    };
}
double len(POINT3D p) {
    return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}
int sgn(double x) {
    return x < -eps ? -1 : x > eps ? 1 : 0;
}
int point_equal(POINT3D P, POINT3D Q){
	if((P.x==Q.x) && (P.y==Q.y) && (P.z==Q.z))return 1;
	return 0;
}
void swap(POINT3D *pa, POINT3D *pb){
    int t;
    t=pa->x,pa->x=pb->x,pb->x=t;
    t=pa->y,pa->y=pb->y,pb->y=t;
    t=pa->z,pa->z=pb->z,pb->z=t;
}
int ptonln(POINT3D a, POINT3D b, POINT3D c) {
    POINT3D ab={b.x-a.x,b.y-a.y,b.z-a.z};
    POINT3D bc={c.x-b.x,c.y-b.y,c.z-b.z};
    return sgn(len(cross_product(ab, bc))) <= 0;
}
double Dist(POINT O, POINT A, POINT B){//B--OA
	double xcrs = Cross(O,A,B)*1.0;
	double len = sqrt( (A.x-O.x)*(A.x-O.x) + (A.y-O.y) * (A.y-O.y) );
	return fabs(xcrs/len);
}
// ----------------- sort the points ------------------ 
int cmp(const void *a, const void *b) {
    POINT3D *pointA = (POINT3D *)a;
    POINT3D *pointB = (POINT3D *)b;
    if (pointA->x != pointB->x) {
        return (pointA->x > pointB->x) - (pointA->x < pointB->x);
    }
    if (pointA->y != pointB->y) {
        return (pointA->y > pointB->y) - (pointA->y < pointB->y);
    }
    return (pointA->z > pointB->z) - (pointA->z < pointB->z);
}
int partition(int *pindex, int low, int high, cmp_func cmp, void *data, int item_size) {
    void *pivot = (char *)data + (pindex[high] * item_size);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (cmp((char *)data + (pindex[j] * item_size), pivot) < 0) {
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
void quicksort(int *pindex, int low, int high, cmp_func cmp, void *data, int item_size) {
    if (low < high) {
        int pi = partition(pindex, low, high, cmp, data, item_size);
        if (pi - 1 > low) {
            quicksort(pindex, low, pi - 1, cmp, data, item_size);
        }
        if (pi + 1 < high) {
            quicksort(pindex, pi + 1, high, cmp, data, item_size);
        }
    }
}
void sort(void *points, int *pindex, int size, int item_size, cmp_func cmp) {
    quicksort(pindex, 0, size - 1, cmp, points, item_size);
}
//-----------------------------------------------------
void Get2DHull(POINT *points,int *pindex, int num,int *stack,int *top, int *puse){
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
	Get2DHull(points,pindex,num,stack,top,puse);
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
	Get2DHull(points,pindex,size,stack,&top,puse);
	free(puse);
	*stacktop = top;
}
void ShowPoints(HDC hdc, POINT *points, int num){
	for(int i=0; i<num; i++){
		MoveToEx( hdc, points[i].x-2, points[i].y-2, NULL ); LineTo( hdc, points[i].x+2, points[i].y+2 );
		MoveToEx( hdc, points[i].x-2, points[i].y+2, NULL ); LineTo( hdc, points[i].x+2, points[i].y-2 );
	}
	char buf[100];
	HFONT hOldFont, hfont;
	hfont = CreateFont(-9,-4,0,0,0,0,0,0,0,0,0,0,0,0);
	hOldFont = SelectObject(hdc, hfont);
	for(int i=0;i<num;i++){
		sprintf(buf,"P%d[%d, %d] ",i,points[i].x,points[i].y);
		TextOut(hdc,400+i/15*120,350+i%15*10,buf,strlen(buf));
	}
	SelectObject(hdc, hOldFont);
	DeleteObject(hfont);	
}
void Show2DConvex(HDC hdc, POINT *points, int num, int *stack, int top){
	if(top<1)return;
	HPEN holdpen, hpen=CreatePen(PS_SOLID,2,RGB(255,0,0));
	holdpen = SelectObject(hdc, hpen);
	MoveToEx(hdc, points[stack[top-1]].x,points[stack[top-1]].y,NULL);
	for(int i=0;i<top;i++){
		LineTo(hdc,points[stack[i]].x,points[stack[i]].y);
	}
	SelectObject(hdc, holdpen);
	DeleteObject(hpen);
}
void ShowDothullInfo(HDC hdc, DOTHULL *dothull){
	HFONT hOldFont, hfont;
	hfont = CreateFont(-9,-4,0,0,0,0,0,0,0,0,0,0,0,0);
	hOldFont = SelectObject(hdc, hfont);
	char buf[100];
	sprintf(buf,"dothull = %p",dothull); TextOut(hdc,650,0,buf,strlen(buf));
	sprintf(buf,"num_points = %d",dothull->num_points); TextOut(hdc,650,10,buf,strlen(buf));
	sprintf(buf,"num_colors = %d",dothull->num_colors); TextOut(hdc,650,20,buf,strlen(buf));
	for(int i=0;i<dothull->num_points+1;i++){
		sprintf(buf,"%d-[ %3d %3d %3d ] ", i, dothull->points[i].x, dothull->points[i].y, dothull->points[i].z );
		TextOut(hdc, 650+i/15*85,30+i%15*10,buf,strlen(buf));
	}
	sprintf(buf,"num_faces = %d",dothull->num_faces); TextOut(hdc,650,180,buf,strlen(buf));
	for(int i=0;i<dothull->num_faces;i++){
		sprintf(buf,"%d-[ %3d %3d %3d ] ", i, dothull->faces[i][0], dothull->faces[i][1], dothull->faces[i][2] );
		TextOut(hdc, 650+i/15*85,190+i%15*10,buf,strlen(buf));
	}
	SelectObject(hdc, hOldFont);
	DeleteObject(hfont);
}
void Get2DConvexB( POINT *p, int *pindex, int size, int *stack, int *stacktop ) {
	// Andrew's Monotone Chain Algorithm
  	int i, t, top = 0;
	for( i=0;i<size;i++)pindex[i]=i;
	sort(p, pindex, size, sizeof(POINT3D), cmp);
	for(i=0;i<size;i++){stack[i]=pindex[i];}
	for ( i = 0; i < size; ++i) {
	  while (top >= 2 && Cross(p[stack[top-2]], p[stack[top-1]], p[pindex[i]]) <= 0) --top;
	  stack[top++] = pindex[i];
	}
	for (i = size-2, t = top+1; i >= 0; --i) {
	  while (top >= t && Cross(p[stack[top-2]], p[stack[top-1]], p[pindex[i]]) <= 0) --top;
	  stack[top++] = pindex[i];
	}
	*stacktop = top-1;
}
void ShowRect(HDC hdc,int left, int top, int right, int bottom){
	MoveToEx(hdc, left, top, NULL);
	LineTo(hdc, right, top);
	LineTo(hdc, right, bottom);
	LineTo(hdc, left, bottom);
	LineTo(hdc, left, top);
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
void glShowTriangles(DOTHULL *dothull){
	POINT3D *points=dothull->points;
	int **faces=dothull->faces;
	int **colors=dothull->colors;
	int ncolor=dothull->num_colors;
	glBegin(GL_TRIANGLES);
	for(int i=0;i<dothull->num_faces;i++){
		int k=rand()%ncolor;
		// int k=i%ncolor;
		glColor3f(colors[k][0]/256.0,colors[k][1]/256.0,colors[k][2]/256.0);
		glVertex3f(points[faces[i][0]].x/250.0,points[faces[i][0]].y/250.0,points[faces[i][0]].z/250.0);
		glVertex3f(points[faces[i][1]].x/250.0,points[faces[i][1]].y/250.0,points[faces[i][1]].z/250.0);
		glVertex3f(points[faces[i][2]].x/250.0,points[faces[i][2]].y/250.0,points[faces[i][2]].z/250.0);
	}
	glEnd();
}
void glShow3DConvex(DOTHULL *dothull){
	//glShowLines(dothull);
	glShowTriangles(dothull);
	//glShowConvex(dothull);
}
int which_side(POINT3D *points, int num, int p, int a, int b, int c) {
    POINT3D A=points[a], B=points[b], C=points[c], P=points[p];
    POINT3D AB = { B.x - A.x, B.y - A.y, B.z - A.z };
    POINT3D AC = { C.x - A.x, C.y - A.y, C.z - A.z };
    POINT3D AP = { P.x - A.x, P.y - A.y, P.z - A.z };
    int cross_x = AB.y * AC.z - AB.z * AC.y;
    int cross_y = AB.z * AC.x - AB.x * AC.z;
    int cross_z = AB.x * AC.y - AB.y * AC.x;
    int dot = cross_x * AP.x + cross_y * AP.y + cross_z * AP.z;
    return dot; // dot>=0;
}
void GetConvexA3D(POINT3D *points,int *pindex, int num, int ***out, int *out_size) {
	sort(points, pindex, num, sizeof(POINT3D), cmp);
    int unique_count = 1;
    for (int i = 1; i < num; i++) {
        if (!point_equal(points[pindex[i]], points[pindex[0]])) {
            points[unique_count++] = points[pindex[i]];
        }
    }
    if (unique_count < 3) {
        *out = NULL;
        *out_size = 0;
        return;
    }
    int p_size = unique_count;
    POINT3D *pp = (POINT3D *)malloc(p_size * sizeof(POINT3D));
    memcpy(pp, points, p_size * sizeof(POINT3D));
    int face_count = 0;
    int **faces = (int **)malloc(p_size * p_size * sizeof(int*));
    for(int i=0;i<p_size * p_size;i++){
    	faces[i]=malloc(3*sizeof(int));
    }
    int found=0;
    //find the first face.
    for (int i = 2; i < p_size; i++) {
        if (ptonln(pp[0], pp[1], pp[i])) continue;
        swap(&pp[1], &pp[2]);
        for (int j = i + 1; j < p_size; j++) {
            int pabc = which_side(pp, p_size, j, 0, 1, 2 );
            if ( pabc <= 0 ) {
            	swap(&pp[j], &pp[3]);
            	faces[face_count][0]=0;faces[face_count][1]=1;faces[face_count][2]=2; face_count++;
            	faces[face_count][0]=0;faces[face_count][1]=2;faces[face_count][2]=1; face_count++;
            	found=1;
            	break;
            }
        }
        if(found)break;
    }
    int **mark = (int **)malloc(p_size*sizeof(int*));
    for(int i=0;i<p_size;i++){
        mark[i]=(int*)malloc(p_size*sizeof(int));
        memset(mark[i],0,p_size*sizeof(int));
    }
    for (int v = 3; v < p_size; v++) {
        int **faces_temp = malloc(face_count * sizeof(int *));
        for(int i=0;i<face_count;i++){
        	faces_temp[i]=malloc(3*sizeof(int));
        }
        int face_count_tmp = 0;
        //--------------------------------------------------------------------------------
        for (int i = 0; i < face_count; i++) {
            int a = faces[i][0], b = faces[i][1], c = faces[i][2];
            int pabc = which_side( pp,p_size, v, a, b, c );
            if ( pabc <= 0 ){
                mark[a][b] = mark[b][a] = v;
                mark[b][c] = mark[c][b] = v;
                mark[c][a] = mark[a][c] = v;
            } else {
                faces_temp[face_count_tmp++] = faces[i];
            }
        }
        //--------------------------------------------------------------------------------
        face_count = face_count_tmp;
        for(int i=0;i<face_count;i++){
        	for(int j=0;j<3;j++){
        		faces[i][j]=faces_temp[i][j];
        	}
        }
        free(faces_temp);
        //--------------------------------------------------------------------------------
        int top=face_count;
        for (int i = 0; i < face_count; i++) {
            int a = faces[i][0], b = faces[i][1], c = faces[i][2];
            if (mark[a][b] == v) {
                faces[top][0]=b;faces[top][1]=a;faces[top][2]=v;top++;
            }
            if (mark[b][c] == v) {
                faces[top][0]=c;faces[top][1]=b;faces[top][2]=v;top++;
            }
            if (mark[c][a] == v) {
                faces[top][0]=a;faces[top][1]=c;faces[top][2]=v;top++;
            }
        }
        face_count=top;
    }
    *out_size = face_count;
    *out = (int **)malloc(face_count * sizeof(int**));
    for (int i = 0; i < face_count; i++) {
    	(*out)[i]=malloc(3*sizeof(int));
        (*out)[i][0] = faces[i][0];
        (*out)[i][1] = faces[i][1];
        (*out)[i][2] = faces[i][2];
    }  
    for(int i=0;i<p_size;i++){free(mark[i]);}
    free(mark);
    free(pp);
    free(faces);
}
void Get3DConvexA( HWND hwnd, HWND glhwnd, DOTHULL *dothull ){
	POINT3D *points=dothull->points;
	int *pindex=dothull->pindex;
	int num=dothull->num_points; 
	int **out; 
	int out_size = 0;
    GetConvexA3D(points, pindex, num, &out, &out_size);
    dothull->num_faces=out_size;
    dothull->faces=out;
}
void GetConvexB3D(POINT3D *points,int *pindex, int num, int ***out, int *out_size){

}
void Get3DConvexB( HWND hwnd, HWND glhwnd, DOTHULL *dothull ){
	POINT3D *points=dothull->points;
	int *pindex=dothull->pindex;
	int num=dothull->num_points; 
	int **out; 
	int out_size = 0;
	GetConvexB3D(points, pindex, num, &out, &out_size);
    dothull->num_faces=out_size;
    dothull->faces=out;		
}
void GetConvexC3D(POINT3D *points,int *pindex, int num, int ***out, int *out_size){

}
void Get3DConvexC( HWND hwnd, HWND glhwnd, DOTHULL *dothull ){
	POINT3D *points=dothull->points;
	int *pindex=dothull->pindex;
	int num=dothull->num_points; 
	int **out; 
	int out_size = 0;
	GetConvexC3D(points, pindex, num, &out, &out_size);
    dothull->num_faces=out_size;
    dothull->faces=out;	
}