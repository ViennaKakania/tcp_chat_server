#include<iostream>
#include<cstring>

#include<arpa/inet.h>
#include<unistd.h>

using std::cout;
using std::endl;

int main(){
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(9090);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (sockaddr*)&local_addr, sizeof(local_addr));

    listen(listen_fd, 3);

    cout << "服务端等待连接" << endl;

    int server_fd = accept(listen_fd, nullptr, nullptr);

    cout << "客户端已连接" << endl;

    char buf[1024];
    while(true){
        int n = recv(server_fd, buf, sizeof(buf)-1, 0);
        
	if(n == 0){
	    cout << "客户端断开连接" << endl;
            break;
	}

	if(n < 0) break;

	buf[n] = '\0';

	cout << "收到：" << buf << endl;
    }

    close(server_fd);
    close(listen_fd);

}	
