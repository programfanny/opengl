// gcc winconvex.c -o winconvex.exe -mwindows;
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ID_TIMER 1

VOID ShowPoints(HDC hdc,POINT* points,int num){
	char buf[100];
	for(int i=0;i<num;i++){
		sprintf(buf,"%d",i);
		TextOut(hdc,points[i].x,points[i].y,buf,strlen(buf));
	}
}
double distance(POINT* points, int* pindex, int a, int b,int p){
	POINT O=points[a],A=points[b],B=points[p];
	double cross = 1.0*(A.x-O.x)*(B.y-O.y)-1.0*(A.y-O.y)*(B.x-O.x);
	double OA=sqrt((A.x-O.x)*(A.x-O.x)+(A.y-O.y)*(A.y-O.y));
	double dist = fabs(cross)/OA;
	return dist;
}
int cross(POINT* points, int* pindex,int len, int k, int i){
	POINT O=points[pindex[k]],A=points[pindex[(k+1)%len]],B=points[i];
	return (A.x-O.x)*(B.y-O.y)-(A.y-O.y)*(B.x-O.x);
}
VOID LoadData(POINT* points, int *num){
	char buf[100];
	int num_points;
	int x,y;
	FILE *fp=fopen("winconvex.in","r");
	fscanf(fp,"%d",&num_points);
	*num = num_points;
	for(int i=0;i<num_points;i++){
		fscanf(fp,"%d,%d\n",&x,&y);
		points[i].x=x;points[i].y=y;
	}
	fclose(fp);
}
VOID GetHull( HDC hdc, POINT* points, int num, int **pavl, int* pindex, int *pleft, int *pright, HPEN *hpen, int *ti ){
	char buf[100];
	double maxdis,xcross,zdist;
	int left=*pleft;
	int right=*pright;
	int len=right-left;
	int zcount=0;
	for(int i=0;i<num;i++){
		if( (*pavl)[i]==0 )zcount++;
	}
	sprintf(buf,"count=%d ",num-zcount-len);
	TextOut(hdc,20,360+(*ti)*12,buf,strlen(buf));
	for(int i=0;i<len;i++){
		sprintf(buf,"%d ",pindex[i]);
		TextOut(hdc,80+i*20,360+(*ti)*12,buf,strlen(buf));
	}
	if(zcount+len>=num)return;

	int *puseful=malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		puseful[i]=len;
	}
	int p=right;
	for(int k=left;k<right;k++){
		maxdis=0.0;
		pindex[p++]=pindex[k];
		int q=-1;
		for(int i=0;i<num;i++){
			if(pindex[k]==i)continue;
			if(pindex[(k+1)%len]==i)continue;
			if((*pavl)[i]){
				xcross=cross(points, pindex, len, k,i);
				if(cross(points, pindex,len, k,i)>=0){
					puseful[i]--;
				}else{
					zdist=distance(points,pindex, pindex[k],pindex[(k+1)%len],i);
					if(maxdis<zdist){
						maxdis=zdist; q=i;
					}
				}
			}
		}
		if(q!=-1){
			pindex[p++]=q;
		}
	}

	for(int i=0;i<num;i++){
		if(puseful[i]==0){(*pavl)[i]=0;}
	}
	left=right;right=p;len=right-left;
	for(int i=0;i<len;i++){
		pindex[i]=pindex[left+i];
	}
	left=0; right=len;
	free(puseful);
	*pleft=left;*pright=right;(*ti)++;
	GetHull(hdc, points, num, pavl, pindex, pleft, pright,hpen,ti);
}
VOID GetConvex(HDC hdc, POINT* points, int num,HPEN *hpen){
	int xmin=400,xmax=0,ymin=400,ymax=0;
	int pxmin,pxmax,pymin,pymax,ti=0;
	for(int i=0;i<num;i++){
		if(xmin>points[i].x){xmin=points[i].x;pxmin=i;}
		if(ymin>points[i].y){ymin=points[i].y;pymin=i;}
		if(xmax<points[i].x){xmax=points[i].x;pxmax=i;}
		if(ymax<points[i].y){ymax=points[i].y;pymax=i;}
	}
	int *pindex=malloc(num*sizeof(int));
	pindex[0]=pxmin; pindex[1]=pymin; pindex[2]=pxmax; pindex[3]=pymax;
	//stack[0]=pxmin; stack[1]=pymin; stack[2]=pxmax; stack[3]=pymax;
	int top=4;
	int *pavail=malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		pavail[i]=1;
	}
	int left,right;
	left=0;right=4;	
	GetHull(hdc, points, num, &pavail, pindex, &left, &right, hpen, &ti);
	int len=right-left;
	char buf[100];
	MoveToEx(hdc,points[pindex[0]].x,points[pindex[0]].y,NULL);	
	for(int i=0;i<len;i++){
		LineTo(hdc,points[pindex[(i+1)%len]].x,points[pindex[(i+1)%len]].y);
	}
}
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
	hwnd = CreateWindow (szAppName,                 	// window class name
						TEXT ("The Hello Program"), 	// window caption
						WS_OVERLAPPEDWINDOW,        	// window style
						CW_USEDEFAULT,              	// initial x position
						CW_USEDEFAULT,              	// initial y position
						CW_USEDEFAULT,              	// initial x size
						CW_USEDEFAULT,              	// initial y size
						NULL,                       	// parent window handle
						NULL,                       	// window menu handle
						hInstance,                  	// program instance handle
						NULL) ;                     	// creation parameters
	 
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
	HINSTANCE 	hInstance;
	char buf[100];
	static RECT rect;
	static HDC mdc,mdcB;
	static HFONT hFont;
	static HPEN hpen[16],hOldPen;
	static BITMAP bitmap,bitmapB;
	static HBITMAP hBitmap, hBitmapA, hBitmapB;
	static BYTE *pBits, *pBitsB;
	static BYTE mouse_state;
	static POINT ptA, ptB;
	static int screenWidth, screenHeight, screenSize;
	static POINT *points;
	static int num;

	switch (message)
	{
	case WM_CREATE:
		srand((unsigned)time(NULL));
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		hFont=CreateFont(-10,-5,0,0,500,FALSE,FALSE,FALSE,0,0,0,0,0,0);
		GetClientRect(hwnd, &rect);
		hdc = GetDC(hwnd);
		mdc = CreateCompatibleDC(hdc);
		mdcB = CreateCompatibleDC(hdc);
		SelectObject(mdc,hFont);
		screenWidth =  rect.right-rect.left;
		screenHeight = rect.bottom-rect.top;
		hBitmap  = CreateCompatibleBitmap (hdc, screenWidth, screenHeight) ;
		hBitmapB = CreateCompatibleBitmap (hdc, screenWidth, screenHeight) ;
		GetObject( hBitmap,sizeof(BITMAP),&bitmap );
		GetObject(hBitmapB,sizeof(BITMAP),&bitmapB);
		pBits  = bitmap.bmBits  = (BYTE*)malloc( bitmap.bmWidthBytes * bitmap.bmHeight);
		pBitsB = bitmapB.bmBits = (BYTE*)malloc( bitmapB.bmWidthBytes * bitmapB.bmHeight);
		memset( bitmap.bmBits, 0xFF, bitmap.bmWidthBytes * bitmap.bmHeight);
		memset(bitmapB.bmBits, 0xFF, bitmap.bmWidthBytes * bitmapB.bmHeight);
		SetBitmapBits( hBitmap,  bitmap.bmWidthBytes *  bitmap.bmHeight, bitmap.bmBits);
		SetBitmapBits(hBitmapB, bitmapB.bmWidthBytes * bitmapB.bmHeight, bitmapB.bmBits);
		SelectObject(mdc, hBitmap);
		SelectObject(mdcB, hBitmapB);
		SelectObject(mdc,hFont);
		for(int i=0;i<16;i++){
			hpen[i]=CreatePen(PS_DOT,1,RGB(i*16,0,192));
		}
		num = 100;
		points = (POINT*)malloc(num*sizeof(POINT));
		SetTimer(hwnd, ID_TIMER,1500,NULL);
		return 0 ;
	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;
		BitBlt(hdc,0,0,1200,900,mdc,0,0,SRCCOPY);
		EndPaint (hwnd, &ps) ;
		return 0 ;
	case WM_TIMER:
		PostMessage(hwnd, WM_KEYDOWN, VK_HOME,0);
		return 0;	
	case WM_KEYDOWN:
		switch(wParam){
			case VK_ESCAPE:
				PostMessage( hwnd,WM_DESTROY,0,0 );
				break;
			case VK_END:
				LoadData(points,&num);
				hdc=GetDC(hwnd);
				SelectObject(hdc,hFont);
				ShowPoints(hdc,points,num);
				GetConvex(hdc,points,num,hpen);
				ReleaseDC(hwnd, hdc);
				break;
			case VK_HOME:
				for(int i=0;i<num;i++){
					points[i].x=20+rand()%150*2;
					points[i].y=20+rand()%150*2;
				}
 				memset( bitmap.bmBits, 0xFF, bitmap.bmWidthBytes * bitmap.bmHeight);
 				SetBitmapBits( hBitmap,  bitmap.bmWidthBytes *  bitmap.bmHeight, bitmap.bmBits);
 			case VK_F2:
 				ShowPoints(mdc,points,num);
				GetConvex(mdc,points,num, hpen);
				InvalidateRect(hwnd,&rect,FALSE);
				break;
		}		
		return 0;	
	  
	case WM_DESTROY:
		for(int i=0;i<16;i++){
			DeleteObject(hpen[i]);
		}
		free(points);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc (hwnd, message, wParam, lParam);
}
