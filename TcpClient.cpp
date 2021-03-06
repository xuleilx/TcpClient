/*
 * TcpClient.cpp
 *
 *  Created on: Oct 25, 2018
 *      Author: xuleilx
 */

#include "TcpClient.h"
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>

TcpClient::TcpClient() :
		m_sock(-1) {
	// TODO Auto-generated constructor stub
}

TcpClient::~TcpClient() {
	// TODO Auto-generated destructor stub
	if ( is_valid() ){
		::shutdown ( m_sock,SHUT_WR );
		m_sock = -1;
	}
}

int TcpClient::connect(const char *host, const char *serv) {
	int sockfd, n, ret = 0;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		std::cout << "tcp_connect error for " << host << "," << serv << ":"
				<< gai_strerror(n) << std::endl;
		ret = -1;
		goto exit;
	}
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue; /* ignore this one */

		if (::connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break; /* success */

		if (::close(sockfd) == -1) {
			std::cout << "close error" << std::endl;
			ret = -1;
			goto exit;
		}
	} while ((res = res->ai_next) != NULL);

	if (res == NULL) /* errno set from final connect() */{
		std::cout << "tcp_connect error for " << host << "," << serv
				<< std::endl;
		ret = -1;
		goto exit;
	}

	freeaddrinfo(ressave);

	m_sock = sockfd;
	exit: return ret;
}
/*
 * When  the  message does not fit into the send buffer of the socket,
 * send() normally blocks, unless the socket has been placed in nonblocking I/O mode.
 * In nonblocking mode it would fail with the error EAGAIN or EWOULDBLOCK in this case.
 * The select(2) call may be used to determine when it is possible to send more data.
 * */
int TcpClient::write(const void *vptr, size_t n) {
	int ret = 0;
	if (::write(m_sock, vptr, n) != n) {
		ret = -1;
		std::cout << "write error" << std::endl;
	}
	return ret;
}

/*
 * Write "n" bytes to a descriptor.
 * nonblocking I/O mode Use this Function
 * */
ssize_t TcpClient::writen(const void *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = ::write(m_sock, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0; /* and call write() again */
			else{
				std::cout << "writen error: "<<errno<< std::endl;
				return (-1); /* error */
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
/* Read some bytes from a descriptor.
 * return: -1 error happened
 *          0 the peer has performed an orderly shutdown
 * */
ssize_t TcpClient::read(void *ptr, size_t nbytes) {
	ssize_t		n;
again:
	if ( (n = ::read(m_sock, ptr, nbytes)) == -1){
		if (errno == EINTR)
			goto again;		/* and call read() again */
		else{
			std::cout << "read error: "<<errno<< std::endl;
		}
	} else if (n == 0)
	    std::cout << "read warn: the peer has performed an orderly shutdown!"<< std::endl;              /* EOF */

	return(n);
}

/* Read "n" bytes from a descriptor. */
ssize_t TcpClient::readn(void *vptr, size_t n) {
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = ::read(m_sock, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else{
				std::cout << "readn error: "<<errno<< std::endl;
				return (-1); /* error */
			}
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}

void TcpClient::close() {
	if ( is_valid() ){
		::shutdown ( m_sock,SHUT_WR );
		m_sock = -1;
	}
}


unsigned int TcpClient::occ_buf_size(){
    int ret = 0;
    unsigned int byte_unsent = 0;
    ret = ioctl(m_sock, SIOCOUTQ, &byte_unsent);
    if (ret < 0) {
        std::cout <<"ioctl SIOCOUTQ error"<<std::endl;
    }
    return byte_unsent;
}

int TcpClient::set_opt_nodelay(){
    int nflag = 1;
    if (setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&nflag, sizeof(nflag)) != 0){
        std::cout << "set tcp nodelay error: "<<strerror (errno)<<std::endl;
        return -1;
    }

    int opt = -1;
    int optLen = sizeof(int);
    if(getsockopt(m_sock,IPPROTO_TCP, TCP_NODELAY, &opt, (socklen_t *)&optLen) != 0){
        std::cout <<"get tcp nodelay setting error: "<<strerror (errno)<<std::endl;
        return -1;
    }else{
        std::cout <<"get tcp nodelay setting : "<<opt<<std::endl;
    }

    return 0;
}

// 由于操作系统的TCP/IP协议栈限制，有的操作系统不能设置到0,有个默认最小值。
int  TcpClient::set_no_sndbuf(int sock){

    int nzero1 = 0;
    if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&nzero1, sizeof(nzero1)) != 0){
        std::cout <<"set sndbuf zero error: "<<strerror (errno)<<std::endl;
        return -1;
    }

    int opt = -1;
    int optLen = sizeof(int);
    if(getsockopt(sock,SOL_SOCKET, SO_SNDBUF, &opt, (socklen_t *)&optLen) != 0){
        std::cout << "get tcp SO_SNDBUF setting error: " << strerror (errno)<<std::endl;
        return -1;
    }else{
        std::cout << "get tcp so_sndbuf setting : " <<opt<<std::endl;
    }

//    int nzero = 0;
//    if(setsockopt(sock, SOL_SOCKET, SO_SNDBUFFORCE, (char*)&nzero, sizeof(nzero)) != 0){
//        printf ("set SO_RCVBUFFORCE zero error: %s.\n" , strerror (errno));
//        return -1;
//    }

    return 0;
}
