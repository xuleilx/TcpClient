/*
 * TcpClient.h
 *
 *  Created on: Oct 25, 2018
 *      Author: xuleilx
 */

#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_
#include<sys/socket.h>
#include<netinet/in.h>

class TcpClient {
public:
	TcpClient();
	virtual ~TcpClient();
	//server
//	int listen(const char *, const char *, socklen_t *);
//	int accept();
	//client
	int connect(const char *host, const char *serv);
	int write(const void *, size_t);//blocking
	ssize_t writen(const void *, size_t);// nonblocking
	ssize_t	read(void *, size_t);
	ssize_t	readn(void *, size_t);
	void close();
private:
    bool is_valid() const
    {
        return m_sock != -1;
    }
private:
    int m_sock;
};

#endif /* TCPCLIENT_H_ */
