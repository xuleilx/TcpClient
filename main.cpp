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
	sleep(3);
	TcpClient *my_timers = (TcpClient *)data;
	my_timers->close();
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
//    client.read(buf,sizeof(buf));

	std::cout<< client.read(buf,sizeof(buf))<<std::endl;

    pthread_join(thread_id,NULL);

	return 0;
}
