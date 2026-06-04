#include<iostream>
#include<cstring>

#include<arpa/inet.h>
#include<unistd.h>

#include<thread>

using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::thread;


void recvThread(int sock){
    char buf[1024];

    while(true){
        int n = recv(sock, buf, sizeof(buf)-1, 0);

	if(n == 0){
	    cout << "\n服务器断开连接" << endl;
	    
	    close(sock);
	    break;
        }

	if(n < 0){
            perror("recv");
            break;
        }

	buf[n] = '\0';

	cout << "\n收到消息：\n" << buf << "\n"  << endl;
    }
}

int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    inet_pton(AF_INET, "192.168.145.129", &server.sin_addr);

    string username;
    cout << "请输入昵称: ";
    getline(cin, username);

    int n = connect(client_fd, (sockaddr*)&server, sizeof(server));

    send(client_fd, username.c_str(), username.size(), 0);

    cout << "连接成功" << endl;

    thread t(recvThread, client_fd);

    string msg;

    while(getline(cin, msg)){
	if(msg == "exit"){
	    shutdown(client_fd, SHUT_RDWR);
            break;
	}
	
        send(client_fd, msg.c_str(), msg.size(), 0);

	cout << "发送成功" << endl;
    }
    
    t.join();

    close(client_fd);
}
