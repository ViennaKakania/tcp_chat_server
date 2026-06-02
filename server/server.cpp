#include<iostream>
#include<cstring>

#include<arpa/inet.h>
#include<unistd.h>

#include <thread>

using std::cout;
using std::endl;
using std::thread;


void relay(int from, int to){      // 转发函数
    char buf[1024];

    while(true){
        int n = recv(from, buf, sizeof(buf), 0);

	if(n == 0){
            cout << "一个客户端离线" << endl;
            const char* msg = "[系统] 对方已离线";
            send(to, msg, strlen(msg), 0);
            break;	    
	}

	if(n < 0){
	    perror("recv");      
       	    break;
        }

	send(to, buf, n, 0);
    }
}

int main(){
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(9090);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (sockaddr*)&local_addr, sizeof(local_addr));

    listen(listen_fd, 3);

    cout << "服务端等待客户端1连接..." << endl;

    int server_fd1 = accept(listen_fd, nullptr, nullptr);

    cout << "客户端1已连接" << endl;



    cout << "服务端等待客户端2连接..." << endl;

    int server_fd2 = accept(listen_fd, nullptr, nullptr);

    cout << "客户端2已连接" << endl;

    thread t1(relay, server_fd1, server_fd2);
    thread t2(relay, server_fd2, server_fd1);

    t1.join();
    t2.join();


    close(server_fd1);
    close(server_fd2);
    close(listen_fd);

}	
