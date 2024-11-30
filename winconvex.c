// gcc winconvex.c -o winconvex.exe -mwindows;
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
VOID ShowStack( HDC hdc, POINT* points, int num, int *stack, int top ){
	if(top<=0)return;
	if(num<top)return;
	stack[top]=stack[0];
	MoveToEx( hdc, points[stack[0]].x, points[stack[0]].y, NULL );
	for(int i=1;i<top+1;i++){
		LineTo(hdc,points[stack[i]].x, points[stack[i]].y);
	}
}
VOID ShowPoints(HDC hdc, POINT* points, int num){
	char buf[100];
	for(int i=0;i<num;i++){
		//Ellipse(hdc, points[i].x-10,points[i].y-10, points[i].x+10,points[i].y+10);
		sprintf(buf,"%d",i);
		TextOut(hdc,points[i].x,points[i].y,buf,strlen(buf));
		// sprintf(buf,"%d - [%3d %3d ]",i,points[i].x,points[i].y);
		// TextOut(hdc,600+i/15*80,20+i%15*14,buf,strlen(buf));
	}
}
VOID LoadData(POINT* points, int *num){
	char buf[100];
	int num_points;
	int x,y;
	FILE *fp=fopen("winconvex.in","r");
	if(!fp){
		MessageBox(NULL,"Missing File winconvex.in, Continue...","LoadData",MB_OK);
		return;
	}
	fscanf(fp,"%d",&num_points);
	*num = num_points;
	for(int i=0;i<num_points;i++){
		fscanf(fp,"%d,%d\n",&x,&y);
		points[i].x=x;points[i].y=y;
	}
	fclose(fp);
}
void InitPoints(POINT* points, int num){
	for(int i=0;i<num;i++){
		points[i].x=20+rand()%150*2;
		points[i].y=20+rand()%150*2;
	}
}
int cross(POINT* points, int* pindex,int len, int k, int i){
	POINT O=points[pindex[k]],A=points[pindex[(k+1)%len]],B=points[i];
	return (A.x-O.x)*(B.y-O.y)-(A.y-O.y)*(B.x-O.x);
}
double distance(POINT* points, int* pindex, int a, int b,int p){
	POINT O=points[a],A=points[b],B=points[p];
	double cross = 1.0*(A.x-O.x)*(B.y-O.y)-1.0*(A.y-O.y)*(B.x-O.x);
	double OA=sqrt((A.x-O.x)*(A.x-O.x)+(A.y-O.y)*(A.y-O.y));
	double dist = fabs(cross)/OA;
	return dist;
}
VOID GetHull( POINT* points, int num, int *pavl, int *count, int* stack, int *top ){
	double maxdis,xcross,zdist;
	int left=0;
	int right=*top;
	int len=right-left;

	if(*count<=0)return;

	int *puseful=malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		puseful[i]=len;
	}
	int p=right;
	for(int k=left;k<right;k++){
		maxdis=0.0;
		stack[p++]=stack[k];
		int q=-1;
		for(int i=0;i<num;i++){
			if(pavl[i]){
				xcross=cross(points, stack, len, k,i);
				if(cross(points, stack,len, k,i)>=0){
					puseful[i]--;
				}else{
					zdist=distance(points,stack, stack[k],stack[(k+1)%len],i);
					if(maxdis<zdist){
						maxdis=zdist; q=i;
					}
				}
			}
		}
		if(q!=-1){
			stack[p++]=q; pavl[q]=0; (*count)--;
		}
	}
	for(int i=0;i<num;i++){
		if(puseful[i]==0){
			pavl[i]=0,(*count)--;
		}
	}
	left=right;right=p;len=right-left;
	for(int i=0;i<len;i++){
		stack[i]=stack[left+i];
	}
	*top=len;
	free(puseful);
	GetHull( points, num, pavl, count, stack, top );
}

int GetConvex( POINT* points, int num, int *stack ){
	int xmin=400,xmax=0,ymin=400,ymax=0;
	int pxmin,pxmax,pymin,pymax,ti=0,top,count;
	for(int i=0;i<num;i++){
		if(xmin>points[i].x){xmin=points[i].x;pxmin=i;}
		if(ymin>points[i].y){ymin=points[i].y;pymin=i;}
		if(xmax<points[i].x){xmax=points[i].x;pxmax=i;}
		if(ymax<points[i].y){ymax=points[i].y;pymax=i;}
	}
	int *pavail=malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		pavail[i]=1;
	}
	stack[0]=pxmin;stack[1]=pymin,stack[2]=pxmax;stack[3]=pymax;
	top=4;count=num-top;
	for(int i=0;i<top;i++)pavail[stack[i]]=0;
	GetHull( points, num, pavail, &count, stack, &top );
	return top;
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
	static int *stack;
	static int num, top;

	switch (message)
	{
	case WM_CREATE:
		//PlaySound (TEXT ("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC) ;
		srand((unsigned)time(NULL));
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		//hFont = CreateFont(-30,-15,  0, 0, 400 , FALSE, FALSE, FALSE, DEFAULT_CHARSET,  OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,  FF_DONTCARE,  TEXT("å¾®è½¯é›…é»‘") );
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
		stack = (int*)malloc((num+1)*sizeof(int));
		InitPoints(points, num);
		top=0;

		return 0 ;
	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;
		SelectObject(hdc,hFont);
		ShowPoints( hdc, points, num );
		ShowStack( hdc, points, num, stack, top );

		EndPaint (hwnd, &ps) ;
		return 0 ;
	case WM_TIMER:
		hdc = GetDC(hwnd);


		ReleaseDC(hwnd,hdc);
		return 0;	
	case WM_KEYDOWN:
		switch(wParam){
			case VK_ESCAPE:
				PostMessage( hwnd,WM_DESTROY,0,0 );
				break;
			case VK_END:
				LoadData(points,&num);
				ShowPoints(mdc,points,num);
				//top = GetConvex( points, num, stack );
				InvalidateRect( hwnd,&rect,TRUE);
				break;
			case VK_HOME:
				InitPoints(points, num);
 				memset( bitmap.bmBits, 0xFF, bitmap.bmWidthBytes * bitmap.bmHeight);
 				SetBitmapBits( hBitmap,  bitmap.bmWidthBytes *  bitmap.bmHeight, bitmap.bmBits);
 			case VK_F2:
 				ShowPoints(mdc,points,num);
				top = GetConvex(points,num, stack );
				InvalidateRect(hwnd,&rect,TRUE);
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
