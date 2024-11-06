// gcc convex.c -o convex.exe -mwindows 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ID_TIME 1

void InitPoints(POINT* p, int* stack, int num);
int convex( POINT *p,int *stack,int *pindex, int size);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
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
	hwnd = CreateWindow(WndClassName,"OpenGL Window Demo",WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 830,750,NULL,NULL,hInstance, NULL);
   
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg, NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	char buf[100];
	PAINTSTRUCT ps;
	HDC hdc;
	HFONT hOldFont;
	static HFONT hFont;
	HPEN hOldPen;
	static HPEN hPen;
	static RECT rect;
	static int cxClient, cyClient ;
	int num=60;
	static POINT *p;
	static int *stack,*pindex,top;
	static BOOL TimeON;
	switch (message){
		case WM_CREATE:
			srand((unsigned)time(NULL));
			hFont=CreateFont(-8,-4,0,0,500,FALSE,FALSE,FALSE,0,0,0,0,0,0);
			p=malloc(num*sizeof(POINT));
			stack=malloc((num+1)*sizeof(int));
			pindex=malloc(num*sizeof(int));
			memset(pindex, 0,num*sizeof(int));
			InitPoints(p,pindex,num);
			hPen = CreatePen(PS_SOLID, 2,RGB(255,0,0));
			top=-1;TimeON=FALSE; 
			return 0;
		case WM_SIZE:
			GetClientRect(hwnd, &rect);
			rect.bottom=420;
			return 0;	
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps); 
			GetClientRect(hwnd, &rect);
			rect.bottom=420;
			hOldFont = SelectObject(hdc, hFont);
			hOldPen = SelectObject(hdc, hPen);
			if(top>0){
				MoveToEx(hdc, p[stack[0]].x, p[stack[0]].y,NULL);
				for(int i=1;i<top+1;i++){
					LineTo(hdc, p[stack[i]].x, p[stack[i]].y);
				}
			}
			SelectObject(hdc, hOldPen);
			SetTextColor(hdc,RGB(0,0,255));
			for(int i=0;i<num;i++){
				MoveToEx(hdc,p[i].x-3,p[i].y-3,NULL);
				LineTo(hdc,p[i].x+3,p[i].y+3);
				MoveToEx(hdc,p[i].x-3,p[i].y+3,NULL);
				LineTo(hdc,p[i].x+3,p[i].y-3);
				sprintf(buf,"%2d",pindex[i]);
				TextOut(hdc,p[pindex[i]].x-3,p[pindex[i]].y+3,buf,strlen(buf));
			}
			SetTextColor(hdc,RGB(0,0,0));
			for(int i=0;i<num;i++){
				sprintf(buf,"%2d-( %3d , %3d )",i,p[i].x,p[i].y);
				TextOut(hdc, 400+i/25*100,20+i%25*15,buf,strlen(buf));
			}
			for(int i=0;i<num;i++){
				sprintf(buf,"%2d",pindex[i]);
				TextOut(hdc, 460+i/25*100,20+i%25*15,buf,strlen(buf));
			}

			SetTextColor(hdc,RGB(255,0,255));
			for(int i=0;i<top+1;i++){
				sprintf(buf," %2d ",stack[i]);				
				TextOut(hdc, 480+i/25*20,20+i%25*15,buf,strlen(buf));
			}
			SetTextColor(hdc, RGB(0,0,0));
			SelectObject(hdc, hOldFont);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					PostMessage(hwnd, WM_DESTROY,0,0);
					break;
				case VK_RETURN:
					InitPoints(p, stack, num);
				case VK_F3:
					top=convex( p,stack, pindex, num );
					InvalidateRect(hwnd, &rect,TRUE);
					break; 
				case VK_SPACE:
					TimeON = !TimeON;
					if(TimeON){
						SetTimer(hwnd, ID_TIME,2000,NULL);
					}else{
						KillTimer(hwnd, ID_TIME);
					}
					break;	
			}
			return 0;
		case WM_TIMER:
			PostMessage(hwnd, WM_KEYDOWN,VK_RETURN,0);
			return 0;	
		case WM_DESTROY: 
			free(p);
			free(stack);
			free(pindex);
			DeleteObject(hPen);
			DeleteObject(hFont);
			PostQuitMessage(0); 
			return 0; 
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
int cross(POINT o, POINT a, POINT b) {
	return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
int partition(int *stack, POINT *p, int low, int high) {
	int pivot = stack[high];
	int i = low - 1;
	for (int j = low; j < high; j++) {
		if (p[stack[j]].y < p[pivot].y || (p[stack[j]].y == p[pivot].y && p[stack[j]].x < p[pivot].x)) {
			i++;
			int temp = stack[i];
			stack[i] = stack[j];
			stack[j] = temp;
		}
	}
	int temp = stack[i + 1];
	stack[i + 1] = stack[high];
	stack[high] = temp;
	return i + 1;
}
void quickSort(int *stack, POINT *p, int low, int high) {
	if (low < high) {
		int pi = partition(stack, p, low, high);
		quickSort(stack, p, low, pi - 1);
		quickSort(stack, p, pi + 1, high);
	}
}
int sort(POINT *p, int *stack, int size) {
	quickSort(stack, p, 0, size - 1);
	return 0; 
}
int convex( POINT *p, int *stack,int *pindex, int size ) {
  	int i, t, top = 0;
	for( i=0;i<size;i++)pindex[i]=i;
	sort(p, pindex, size);
	for(i=0;i<size;i++){stack[i]=pindex[i];}
	/* lower  */
	for ( i = 0; i < size; ++i) {
	  while (top >= 2 && cross(p[stack[top-2]], p[stack[top-1]], p[pindex[i]]) <= 0) --top;
	  stack[top++] = pindex[i];
	}
	/* upper  */
	for (i = size-2, t = top+1; i >= 0; --i) {
	  while (top >= t && cross(p[stack[top-2]], p[stack[top-1]], p[pindex[i]]) <= 0) --top;
	  stack[top++] = pindex[i];
	}
	return top-1;
}

void InitPoints(POINT* p, int* pindex, int num){
	for(int i=0;i<num;i++){
		p[i].x=50+rand()%150*2;
		p[i].y=50+rand()%150*2;
		pindex[i]=i;
	}
}
