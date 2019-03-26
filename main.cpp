#include <iostream>
#include"TcpClient.h"
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <stdio.h>
using namespace std;

void *thread_fun(void *data)
{
	sleep(5);
	TcpClient *my_TcpClient = (TcpClient *)data;
	my_TcpClient->close();
}
int main(int argc, char **argv) {
	std::string str("Hello world!");
	char buf[10];
	TcpClient client;
	client.connect("127.0.0.1","9406");
	std::cout<<"connect"<<std::endl;
	client.write(str.data(),str.length());
	std::cout<<"write"<<std::endl;

	pthread_t thread_id = 0;
	pthread_create(&thread_id, NULL, thread_fun, &client);

	std::cout<<"read:"<<std::endl;
	client.readn(buf,sizeof(buf));

	std::cout<< buf <<std::endl;

	pthread_join(thread_id,NULL);

	return 0;
}
