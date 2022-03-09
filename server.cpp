/**
 * TCP������ͨ�Ż�������
 * zhangyl 2018.12.13
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

int main(int argc, char* argv[])
{
    //1.����һ������socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //  ��������family��type��protocol����һ����Э��أ�AF_INET����IPV4��AF_INET6����IPV6��AF_LOCAL����UnixЭ��
    //  type���׽ӿ�Э�飬SOCK_STREAM�ֽ�����SOCK_DGRAM���ݱ��� SOCK_RAWԭʼ�׽���
    //  protocol����ʹ�õľ��崫���Э��
    //  ���ص���һ���Ǹ��������׽�����������sockfd����ָ����Э��غ��׽������ͣ�û�о����Э���ַ��IP + port��
    if (listenfd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }

    //2.��ʼ����������ַ���󶨾���Ľ���
    // socketֻ�Ǵ�����һ��fd�����ǲ�û�з�������Э���ַ��bind��һ��̶���Э���ַ��sockfd����һ��
    // bind����������sockfd�� myaddr�� addrlen
    // sockfd����Ҫ�󶨵�sockfd
    // myaddr����Ҫ�󶨵�IP�Ͷ˿ںţ���Ҫʹ�������ֽ���-�����ģʽ
    // addrlen��myaddr�ĳ���
    struct sockaddr_in bindaddr;
    // sockaddr_in������һ��Э���ַ��IP + port��
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);
    // INADDR_ANY����������IP��ַ
    // �ֽ������� htons��host to network ��short  htonl��host to network ��long
    
    if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        // �׻�����������ʽ���ݣ�ʹ��ǿ������ת����ͳһ�˴�������Ľṹ
        std::cout << "bind listen socket error." << std::endl;
		close(listenfd);
        return -1;
    }

	//3.����������socket�������׽ӿ�Ĭ���������ģ���Ҫ����connect��listen���Խ���ת��Ϊ�����׽ӿ�
    // �ڶ����������ں�Ϊ���׽ӿ��Ŷӵ�������Ӹ���
    // �������Ӷ��У�
    // δ������Ӷ��У� �Ѿ�����SYN ACK���ģ��ȵ����������֣� ��������Ӷ��У��Ѿ��������������
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
		//4. ���ܿͻ�������
        int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
        // ������ܳɹ������ص���һ���µ��������׽ӿ�
        if (clientfd != -1)
        {         	
			char recvBuf[32] = {0};
            // ������
			//5. �ӿͻ��˽�������
			int ret = recv(clientfd, recvBuf, 32, 0);
			if (ret > 0) 
			{
				std::cout << "recv data from client, data: " << recvBuf << std::endl;
				//6. ���յ�������ԭ�ⲻ���ط����ͻ���
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
            // �׽��ֽ���ر�״̬��������ʹ�ø��׽��֣�����TCP�ᷢ�ͷ��ͻ��������Ѿ��ŶӴ���������
        }
    }
	
	//7.�ر�����socket
	close(listenfd);

    return 0;
}