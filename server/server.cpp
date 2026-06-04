#include<iostream>
#include<cstring>

#include<arpa/inet.h>
#include<unistd.h>

#include <thread>

#include <vector>
#include <mutex>
#include <algorithm>

#include <unordered_map>

using std::cout;
using std::endl;
using std::string;
using std::thread;

using std::vector;
using std::mutex;
using std::lock_guard;

using std::unordered_map;

vector<int> clients;
unordered_map<int,string> users;
mutex clients_mutex;


void broadcast(const char* buf, int len, int send_sock){
    lock_guard<mutex> lock(clients_mutex);

    for(auto fd: clients){
        if(fd != send_sock){
	    send(fd, buf, len, 0);
	}
    }
}

void client_handler(int fd){      
    char buf[1024];

    while(true){
        int n = recv(fd, buf, sizeof(buf), 0);

	if(n == 0){
            cout << users[fd]  << "离开聊天室\n" << endl;
	    string offline_msg = "[系统] " + users[fd] + " 离开聊天室\n";
	    broadcast(offline_msg.c_str(), offline_msg.size(), -1);
            
	    lock_guard<mutex> lock(clients_mutex);
            clients.erase(remove(clients.begin(), clients.end(), fd),clients.end());
            users.erase(fd);

	    close(fd);
	    break;	    
	}

	if(n < 0){
	    perror("recv");      
            
	    lock_guard<mutex> lock(clients_mutex);
            clients.erase(remove(clients.begin(), clients.end(), fd),clients.end());
            
            close(fd);
	    break;
        }

	string msg = "[" + users[fd] + "] " + string(buf, n);
	broadcast(msg.c_str(), msg.size(), fd);
    }
}

void shutdown_all_clients(){
    lock_guard<mutex> lock(clients_mutex);

    const char* msg = "[Server] 服务器即将关闭\n";

    for(auto fd : clients){
        send(fd, msg, strlen(msg), 0);

        shutdown(fd, SHUT_RDWR);

        close(fd);
    }

    clients.clear();

    cout << "所有客户端已断开" << endl;
}

void server_command(){
    string cmd;

    while(true){
        getline(std::cin, cmd);

        if(cmd == "exit"){
            shutdown_all_clients();

            cout << "服务器关闭" << endl;

            exit(0);
        }
    }
}

int main(){
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(9090);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (sockaddr*)&local_addr, sizeof(local_addr));

    listen(listen_fd, 10);

    cout << "聊天室服务器启动成功\n（输入exit可关闭服务器）" << endl;

    thread cmd_thread(server_command);
    cmd_thread.detach();

    while(true){
        int server_fd = accept(listen_fd, nullptr, nullptr);
        
	char name_buf[128];
	int m = recv(server_fd, name_buf, sizeof(name_buf), 0);
	string username(name_buf, m);   //转为字符串
	users[server_fd] = username;

        if(server_fd < 0) {
            perror("accept");
            continue;
        }

       	lock_guard<std::mutex> lock(clients_mutex);
	clients.push_back(server_fd);
	cout << users[server_fd] << "上线" << endl;

	thread t(client_handler, server_fd);

        t.detach();
    }

    close(listen_fd);

}	
