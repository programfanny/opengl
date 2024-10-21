//client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	WSADATA wsaData;
	WSAStartup (MAKEWORD(2,2),&wsaData);

	HWND hDeskWnd = GetDesktopWindow();
	HDC hDeskDC = GetWindowDC(hDeskWnd);
	int hDeskWidth = GetDeviceCaps(hDeskDC, HORZRES);
	int hDeskHeight = GetDeviceCaps(hDeskDC, VERTRES);
	//初始化网络
	SOCKET client_socket = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET ==client_socket){
		printf("Create Socket failed!!1\n");
		return -1;
	}
	struct sockaddr_in target={0};
	target.sin_family = AF_INET;
	target.sin_port=htons(port);
	target.sin_addr.s_addr=inet_addr("127.0.0.1");
	法（-1==connect(client_socket,(struct sockaddr*)&target,sizeof(struct  sock))）{
		printf("Connect Server failed!!1\n");
		return -1;		
	}
	while(true){
		//将图片拷进存贮区
		
		//将存贮区存为文件
		//
	}
	return 0;
}