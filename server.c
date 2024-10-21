#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyx.h>
SOCKET create_listen_socket(unsigned short port){
	SOCKET sock = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET ==sock){
		printf("Create Socket failed!!1\n");
		return -1;
	}
	struct sockAddr_in local={0};
	local.sin_family = AF_INET;
	local.sin_port=htons(port);
	local.sin_addr.s_addr=inet_addr("0.0.0.0");
	if(-1==bind(sock,(struct sockaddr*)&local,sizeof(local))){
		printf("bind fail.\n");
		return -1;
	}
	if(-1==listen(sock,10)){
		printf("listen socket failed.\n");
		return 0;
	}
	return sock;	
}
int main(int argc, char* argv[]){
	WSADATA wsaData;
	WSAStartup (MAKEWORD(2,2),&wsaData);
	SOCKET listen_socket = create_listen_socket(8080);
	struct sockaddr_in client_addr;
	int socklen = sizeof(struct sockaddr_in);
	while(TRUE){
		SOCKET client_socket = accept(listen_socket,(struct sockaddr*)&client_addr,&socklen);
		if(INVALID_SOCKET==client_socket)continue;
		client_
		printf("New connect, ip: %s port: %d", inet_ntoa(client_addr.ain_addr),ntohs(client_addr.sin_port));
		while(true){
			//Get pictures.

			//output the pictures.
			

		}

	}


}
