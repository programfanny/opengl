//windows 图像
//windows 网络
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>

void CaptureScreen(const char* filename);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow){
	static TCHAR szAppName[]=TEXT("szAPPName");
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
	wndclass.hbrBackground =(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,"This program requires Windows NT!",szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName, "The Monitor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg, NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	switch(message){
		case WM_CREATE:
			return 0;
		case WM_PAINT:
			hdc=BeginPaint(hwnd, &ps);
			TextOut(hdc, 10,10,"I iiii",6);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_F12:
					//MessageBox(NULL,"F12","KKK",MB_OK);
					CaptureScreen("pic01.bmp");
					break;
				case VK_ESCAPE:
					PostMessage(hwnd, WM_DESTROY,0,0);
					break;
			}
			return 0;	
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd,message, wParam, lParam);
}

void CaptureScreen(const char *filename){
	BITMAPINFOHEADER bi;
	BITMAPFILEHEADER bf;

	HDC hScreenDC = GetDC(NULL);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
	SelectObject(hMemoryDC, hOldBitmap);

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// 初始化BITMAPFILEHEADER结构体
	bf.bfType = 0x4D42; // 'BM'
	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * height * 3);
	bf.bfReserved1 = 0;
	bf.bfReserved2 = 0;
	bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// 保存截图
	FILE* file = fopen(filename, "wb");
	if (file) {
		fwrite(&bf, 1, sizeof(BITMAPFILEHEADER), file);
		fwrite(&bi, 1, sizeof(BITMAPINFOHEADER), file);

		// 获取位图信息
		unsigned char* bitmapData = malloc(width * height * 3);
		GetDIBits(hMemoryDC, hBitmap, 0, height, bitmapData, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

		unsigned char* TempData = malloc(width * height * 3);
		// 由于DIB是自上而下存储的，我们需要反转行
		// for (int y = 0; y < height; y++) {
		// 	memcpy(&TempData[(height - y - 1) * width * 3], &bitmapData[y * width * 3], width * 3);
		// }
		for (int y = 0; y < height ; y++) {
			memcpy(&TempData[ y * width * 3 ], &bitmapData[y * width * 3], width * 3);
		}
		fwrite(TempData, 1, width * height * 3, file);
		free(TempData);
		free(bitmapData);

		fclose(file);
	}

	// 清理资源
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);
}