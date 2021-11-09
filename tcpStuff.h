
#ifndef GROUP_A_CS4323_TCPSTUFF_H
#define GROUP_A_CS4323_TCPSTUFF_H

#define DATA_SERVER_PORT_NUM 8080
#define CLIENT_BACKLOG 25
#define MSG_BUFFER_SIZE 10240


void createSocket(int *sock);

void bindSocket(struct sockaddr_in *sAddress, const int *sock);

void listenOnSocket(const int* sock, int backLogCount);

bool acceptConnection(int* clientSock, const int* hSock, struct sockaddr_in* client);

void readSocket(const int* sock, char* buffer);

void writeSocket(const int* sock, char* buffer);

#endif //GROUP_A_CS4323_TCPSTUFF_H
