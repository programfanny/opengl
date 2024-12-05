// gcc convex.c -oconvex.exe -mwindows
// Algorithm:
//   Find the farthest point to the founded line and add to the convex list.
// 2024-12-05 by programfanny 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void DrawRect(HDC hdc, RECT *rect);
void InitPoints(POINT **points, int **pindex,int **stack, int *num);
void ShowPoints(HDC hdc,int xpos, int ypos, POINT *points, int *pindex, int num,HFONT hFont);
void ShowConvex(HDC hdc, int xpos, int ypos, POINT *points, int *stack, int top){
	if(top>0){
		MoveToEx(hdc, points[stack[top-1]].x,points[stack[top-1]].y,NULL);
		for(int i=0;i<top;i++){
			LineTo(hdc, points[stack[i]].x,points[stack[i]].y);
		}
	}
}
int Cross(POINT O, POINT A, POINT B);
double Dist(POINT O, POINT A, POINT B);
int GetConvex( POINT *points, int *pindex, int num, int *stack );


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("HelloWin") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;
	if (!RegisterClass (&wndclass))
	{
	     MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR) ;
	     return 0 ;
	}
	hwnd = CreateWindow (szAppName,                  // window class name
	                     TEXT ("The Hello Program"), // window caption
	                     WS_OVERLAPPEDWINDOW,        // window style
	                     CW_USEDEFAULT,              // initial x position
	                     CW_USEDEFAULT,              // initial y position
	                     CW_USEDEFAULT,              // initial x size
	                     CW_USEDEFAULT,              // initial y size
	                     NULL,                       // parent window handle
	                     NULL,                       // window menu handle
	                     hInstance,                  // program instance handle
	                     NULL) ;                     // creation parameters
	
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;
	while (GetMessage (&msg, NULL, 0, 0))
	{
	     TranslateMessage (&msg) ;
	     DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	HINSTANCE hInstance;
	static HDC mdc;
	static BITMAP bitmap;
	static HBITMAP hBitmap, hBitmapA;
	static BYTE *pBits;

	static HFONT hFont;
	static RECT rect;
	static HWND glhwnd, hlist, hedit;
	static POINT* points;
	static int *pindex, *stack, num, top;

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		hdc = GetDC(hwnd);
		mdc = CreateCompatibleDC(hdc);
		hBitmap  = CreateCompatibleBitmap (hdc, rect.right-rect.left, rect.bottom-rect.top) ;
		GetObject( hBitmap,sizeof(BITMAP),&bitmap );	
		pBits  = bitmap.bmBits  = (BYTE*)malloc( bitmap.bmWidthBytes * bitmap.bmHeight);
		memset( bitmap.bmBits, 0xFF, bitmap.bmWidthBytes * bitmap.bmHeight);
		SetBitmapBits( hBitmap,  bitmap.bmWidthBytes *  bitmap.bmHeight, bitmap.bmBits);
		SelectObject(mdc, hBitmap);
		hFont = CreateFont(-8,-4,0,0,0,0,0,0,0,0,0,0,0,0);
		SetRect( &rect,15,15,305,305 );
		InitPoints(&points, &pindex,&stack, &num);
	    return 0 ;
	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps);
		SelectObject(hdc, hFont);
		DrawRect(hdc, &rect);
		ShowPoints(hdc,0,0, points, pindex, num,hFont);
		ShowConvex(hdc, 0,0,points, stack, top);
		EndPaint (hwnd, &ps) ;
		return 0 ;
	case WM_KEYDOWN:
		switch(wParam){
			case VK_ESCAPE:
				PostMessage(hwnd, WM_DESTROY,0,0);
				break;
			case VK_SPACE:
				InitPoints(&points, &pindex,&stack,&num);
				top=GetConvex( points, pindex, num, stack );
				break;	
		}
		InvalidateRect(hwnd, &rect,TRUE);
		return 0;	
	case WM_DESTROY:
		if(points)free(points);
		if(pindex)free(pindex);
		if(stack)free(stack);
	    PostQuitMessage(0);
	    return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
void InitPoints(POINT **points, int **pindex,int **stack, int *num){
	if(*points)free(*points);
	if(*pindex)free(*pindex);
	if(*stack)free(*stack);
	*num=20+rand()%180;
	*points=malloc(*num*sizeof(POINT));
	*pindex=malloc(*num*sizeof(int));
	*stack=malloc(*num*sizeof(int));	
	for(int i=0;i<*num;i++){
		(*pindex)[i]=i;
		(*points)[i].x=20+rand()%280;
		(*points)[i].y=20+rand()%280;
	}
}
void DrawRect(HDC hdc, RECT *rect){
	MoveToEx(hdc, rect->left,rect->top,NULL);
	LineTo(hdc, rect->right, rect->top);
	LineTo(hdc, rect->right, rect->bottom);
	LineTo(hdc, rect->left, rect->bottom);
	LineTo(hdc, rect->left, rect->top);
}
void ShowPoints(HDC hdc,int xpos, int ypos,POINT *points, int *pindex, int num, HFONT hFont){
	char buf[100];
	HFONT hOldFont = SelectObject(hdc, hFont);
	for(int i=0;i<num;i++){
		MoveToEx(hdc,xpos+points[pindex[i]].x-2,ypos+points[pindex[i]].y-2,NULL);
		LineTo(hdc, xpos+points[pindex[i]].x+2,ypos+points[pindex[i]].y+2);
		MoveToEx(hdc,xpos+points[pindex[i]].x-2,ypos+points[pindex[i]].y+2,NULL);
		LineTo(hdc, xpos+points[pindex[i]].x+2,ypos+points[pindex[i]].y-2);
	}
	SelectObject(hdc, hOldFont);
}
int Cross(POINT O, POINT A, POINT B){
	return (A.x-O.x)*(B.y-O.y)-(A.y-O.y)*(B.x-O.x);
}
double Dist(POINT O, POINT A, POINT B){//B--OA
	double xcrs = Cross(O,A,B)*1.0;
	double len = sqrt( (A.x-O.x)*(A.x-O.x) + (A.y-O.y) * (A.y-O.y) );
	return fabs(xcrs/len);
}
void GetHull(POINT *points,int *pindex,int num,int *stack,int *top, int *puse){
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
				if(Cross(points[stack[k]],points[stack[(k+1)%len]],points[pindex[i]])>=0){
					uuse[i]--; if(uuse[i]==0){puse[i]=1;}
				}else{
					dist = Dist(points[stack[k]],points[stack[(k+1)%len]],points[pindex[i]]);
					if(maxdist<dist){
						maxdist=dist; q=i;
					}
				}
			}
		}
		if(q!=-1){
			stack[p++]=pindex[q];puse[q]=1;
		}
	} 
	free(uuse);	
	for(int i=0;i<p-len;i++){stack[i]=stack[len+i];}
	*top=p-len;	
	GetHull(points,pindex,num,stack,top,puse);
}
int GetConvex( POINT *points, int *pindex, int num, int *stack ){
	int top=0,s=1000,t=-1,us,ut;
	int *puse = malloc(num*sizeof(int));
	memset(puse,0,num*sizeof(int));
	for(int i=0;i<num;i++){
		if(s>points[i].x){s=points[i].x;us=i;}
		if(t<points[i].x){t=points[i].x;ut=i;}		
	}
	stack[0]=us;stack[1]=ut;top=2;puse[us]=1;puse[ut]=1;
	int ti=0;
	GetHull(points,pindex,num,stack,&top,puse);
	free(puse);
	return top;
}
