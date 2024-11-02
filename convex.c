// gcc convex.c -o convex.exe -mwindows 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ID_TIME 1

void InitPoints(POINT *p, int *stack, int num);
int convex( POINT *p,int *stack,int size);
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
	hwnd = CreateWindow(WndClassName,"OpenGL Window Demo",WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 830,480,NULL,NULL,hInstance, NULL);
   
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
	HDC hdc;                       // handle to device context 
	HFONT hOldFont;
	static HFONT hFont;
	HPEN hOldPen;
	static HPEN hPen;

	RECT rcClient;                 // client area rectangle 
	POINT ptClientUL;              // client upper left corner 
	POINT ptClientLR;              // client lower right corner 
	static POINTS ptsBegin;        // beginning point 
	static POINTS ptsEnd;          // new endpoint 
	static POINTS ptsPrevEnd;      // previous endpoint 
	static BOOL fPrevLine = FALSE; // previous line flag 

	int num=20;
	static POINT *p;
	static int *stack,top;
	static BOOL TimeON;
	switch (message){
		case WM_CREATE:
			// srand((unsigned)time(NULL));
			hFont=CreateFont(-10,-5,0,0,800,TRUE,FALSE,FALSE,CHINESEBIG5_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_MODERN,NULL);
			p=malloc(num*sizeof(POINT));
			stack=malloc(num*sizeof(int));
			InitPoints(p,stack,num);
			hPen = CreatePen(PS_SOLID, 2,RGB(255,0,0));
			top=-1;TimeON=FALSE; 
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps); 
			hOldFont = SelectObject(hdc, hFont);
			hOldPen = SelectObject(hdc, hPen);
			MoveToEx(hdc, p[stack[0]].x, p[stack[0]].y,NULL);
			for(int i=1;i<top+1;i++){
				LineTo(hdc, p[stack[i]].x, p[stack[i]].y);
			}
			SelectObject(hdc, hOldPen);
			SetTextColor(hdc,RGB(0,0,255));
			for(int i=0;i<num;i++){
				Ellipse(hdc,p[i].x-10,p[i].y-10,p[i].x+10,p[i].y+10);
				sprintf(buf,"%2d",i);
				TextOut(hdc,p[i].x-8,p[i].y-6,buf,strlen(buf));
			}
			SetTextColor(hdc,RGB(0,0,0));
			for(int i=0;i<num;i++){
				sprintf(buf,"%2d-( %3d , %3d )",i,p[i].x,p[i].y);
				TextOut(hdc, 420+i/25*300,20+i%25*15,buf,strlen(buf));
			}
			SetTextColor(hdc,RGB(255,0,255));
			for(int i=0;i<top+1;i++){
				sprintf(buf," %2d ",stack[i]);				
				TextOut(hdc, 525+i/25*20,20+i%25*15,buf,strlen(buf));
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
					top=-1;
				case VK_F3:
					top=convex( p,stack,num);
					InvalidateRect(hwnd, NULL,TRUE);
					break; 
				case VK_SPACE:
					TimeON = !TimeON;
					if(TimeON){
						SetTimer(hwnd, ID_TIME,250,NULL);
					}else{
						KillTimer(hwnd, ID_TIME);
					}
					break;	
			}
			return 0;
		case WM_TIMER:
			PostMessage(hwnd, WM_KEYDOWN,VK_RETURN,0);
			return 0;	
		case WM_LBUTTONDOWN: 
			SetCapture(hwnd); 
			GetClientRect(hwnd, &rcClient); 
			ptClientUL.x = rcClient.left; 
			ptClientUL.y = rcClient.top; 
			ptClientLR.x = rcClient.right + 1; 
			ptClientLR.y = rcClient.bottom + 1; 
			ClientToScreen(hwnd, &ptClientUL); 
			ClientToScreen(hwnd, &ptClientLR); 
			SetRect(&rcClient, ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y); 
			ClipCursor(&rcClient); 
			ptsBegin = MAKEPOINTS(lParam); 
			return 0; 
		case WM_MOUSEMOVE: 
			if (wParam & MK_LBUTTON) 
			{ 
				hdc = GetDC(hwnd); 
				SetROP2(hdc, R2_NOTXORPEN); 
				if (fPrevLine) 
				{ 
					MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL); 
					LineTo(hdc, ptsPrevEnd.x, ptsPrevEnd.y); 
				} 
				ptsEnd = MAKEPOINTS(lParam); 
				MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL); 
				LineTo(hdc, ptsEnd.x, ptsEnd.y); 
				fPrevLine = TRUE; 
				ptsPrevEnd = ptsEnd; 
				ReleaseDC(hwnd, hdc); 
			} 
			break; 
		case WM_LBUTTONUP: 
			fPrevLine = FALSE; 
			ClipCursor(NULL); 
			ReleaseCapture(); 
			return 0; 
		case WM_DESTROY: 
			free(p);
			free(stack);
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

int convex( POINT *p, int *stack, int size ) {
	int value=0;
	char buf[100];
	int *pindex=malloc(size*sizeof(int));
	for(int i=0;i<size;i++)pindex[i]=i;
	sort(p, pindex, size);
	for(int i=0;i<size;i++){
		stack[i]=pindex[i];
	}
	int top = 0;	
	for(int i = top+1; i<size; i++){
		while((top>0) && (cross(p[stack[top-1]],p[stack[top]],p[pindex[i]])<0)){ top--; }
		stack[++top]=pindex[i];
	}
	for(int i = size-1; i>=0; i--){
		while((top>0) && (cross(p[stack[top-1]],p[stack[top]],p[pindex[i]])<0)){ top--; }
		stack[++top]=pindex[i];
	}	
	free(pindex);
	return top;
}
void InitPoints(POINT *p, int *stack, int num){
	for(int i=0;i<num;i++){
		p[i].x=20+rand()%180*2;
		p[i].y=20+rand()%180*2;
		stack[i]=i;
	}	
}