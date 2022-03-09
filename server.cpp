/**
 * TCP服务器通信基本流程
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

int main(int argc, char* argv[])
{
    //1.创建一个侦听socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //  三个参数family，type和protocol。第一个是协议簇，AF_INET代表IPV4，AF_INET6代表IPV6，AF_LOCAL代表Unix协议
    //  type是套接口协议，SOCK_STREAM字节流，SOCK_DGRAM数据报， SOCK_RAW原始套接字
    //  protocol代表使用的具体传输层协议
    //  返回的是一个非负整数，套接字描述符（sockfd），指定了协议簇和套接字类型，没有具体的协议地址（IP + port）
    if (listenfd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }

    //2.初始化服务器地址，绑定具体的进程
    // socket只是创建了一个fd，但是并没有分配具体的协议地址，bind将一组固定的协议地址与sockfd绑定在一起
    // bind三个参数：sockfd， myaddr， addrlen
    // sockfd：想要绑定的sockfd
    // myaddr：想要绑定的IP和端口号，均要使用网络字节序-即大端模式
    // addrlen：myaddr的长度
    struct sockaddr_in bindaddr;
    // sockaddr_in：定义一个协议地址（IP + port）
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);
    // INADDR_ANY：本机所有IP地址
    // 字节排序函数 htons：host to network 的short  htonl：host to network 的long
    
    if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        // 套机字以引用形式传递，使用强制类型转换，统一了传入参数的结构
        std::cout << "bind listen socket error." << std::endl;
		close(listenfd);
        return -1;
    }

	//3.启动侦听，socket创建的套接口默认是主动的，即要调用connect，listen可以将其转换为被动套接口
    // 第二个参数是内核为此套接口排队的最大连接个数
    // 两个连接队列：
    // 未完成连接队列： 已经发送SYN ACK报文，等到第三次握手； 已完成连接队列：已经完成了三次握手
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
		close(listenfd);
        return -1;
    }

    while (true)
    {
        struct sockaddr_in clientaddr;
        socklen_t clientaddrlen = sizeof(clientaddr);
		//4. 接受客户端连接
        int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
        // 如果接受成功，返回的是一个新的已连接套接口
        if (clientfd != -1)
        {         	
			char recvBuf[32] = {0};
            // 缓冲区
			//5. 从客户端接受数据
			int ret = recv(clientfd, recvBuf, 32, 0);
			if (ret > 0) 
			{
				std::cout << "recv data from client, data: " << recvBuf << std::endl;
				//6. 将收到的数据原封不动地发给客户端
				ret = send(clientfd, recvBuf, strlen(recvBuf), 0);
				if (ret != strlen(recvBuf))
					std::cout << "send data error." << std::endl;
				else
					std::cout << "send data to client successfully, data: " << recvBuf << std::endl;
			} 
			else 
			{
				std::cout << "recv data error." << std::endl;
			}
			
			close(clientfd);
            // 套接字进入关闭状态，不能再使用该套接字，但是TCP会发送发送缓冲区内已经排队待发的数据
        }
    }
	
	//7.关闭侦听socket
	close(listenfd);

    return 0;
}
