#include<iostream>
#include<cstring>

#include<arpa/inet.h>
#include<unistd.h>

using std::cout;
using std::endl;
using std::string;
using std::cin;

int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    inet_pton(AF_INET, "192.168.145.129", &server.sin_addr);

    int n = connect(client_fd, (sockaddr*)&server, sizeof(server));

    if(n < 0){
        perror("connect");
	return 1;
    }

    cout << "客户端连接成功" << endl;

    string msg;

    while(getline(cin, msg)){
	if(msg == "exit"){
	    close(client_fd);
            break;
	}
	
        send(client_fd, msg.c_str(), msg.size(), 0);

	cout << "发送成功" << endl;
    }
    
    close(client_fd);
}
