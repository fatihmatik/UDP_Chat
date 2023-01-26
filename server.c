#if defined WIN32
    #include <winsock2.h>
    #include <winsock.h>
#endif

#include <stdio.h>
#define USE_PORT 8080
#define IP_ADDR "127.0.0.1"
#define BUFF_SIZE 512


int main(){

    // winsocket initialization
    #if defined WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
        if ( iResult != 0){
            printf("Error at WSAStartup()\n");
            return 0;
        }
    #endif

    // 0. Setting up the server socket.
    SOCKET socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    char server_message[BUFF_SIZE], client_message[BUFF_SIZE];
    memset(client_message, '\0', sizeof(client_message));

    SOCKADDR_IN server_address = {0};
    SOCKADDR_IN client_address = {0};
    int sizeof_client_address = sizeof(client_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons( USE_PORT );
    server_address.sin_addr.s_addr = inet_addr( IP_ADDR );

    // Checking if the socket is created succesfuly.
    if ( socket_desc == INVALID_SOCKET ){
        return (SOCKET)NULL;
    }

    // Binding the socket to the desired port in "#define PORT"
    if ( bind( socket_desc, (SOCKADDR *)&server_address, sizeof(server_address) ) == SOCKET_ERROR)  {
        printf("bind failed with error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("The server_socket successfuly binded to the port %d\n\n", USE_PORT);



    // Waiting for the messages from the client.
    if ( recvfrom(socket_desc, client_message, sizeof(client_message), 0,(SOCKADDR*)&client_address,&sizeof_client_address) < 0) {
        printf("Couldn't receive\n");
        return -1;
    }


    // 3 The server displays the incoming mesg and the name of the host by gethostbyaddr.
    struct hostent *HostEntity;
    HostEntity = (struct hostent*)malloc(sizeof(struct hostent));
    struct in_addr IPv4_address = {0};
    IPv4_address.s_addr = inet_addr(inet_ntoa(client_address.sin_addr));
        // IPv4_address.s_addr = inet_addr("127.0.0.1");
    HostEntity = gethostbyaddr((char*)&IPv4_address, 4, AF_INET);
    printf("MSG recieved from the client with hostname: %s\n", HostEntity->h_name);
    printf("Received message from IP: %s and port(sender): %i\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    printf("Msg from client: %s\n", client_message);
    memset(client_message, '\0', sizeof(client_message));


    // 5 OK msg, to send to the client.
    strcpy(server_message, "OK");
    if( sendto(socket_desc, server_message, strlen(server_message), 0, (struct sockaddr*)&client_address, sizeof_client_address) < 0){
        printf("Unable to send message\n");
    }
    memset(server_message, '\0', sizeof(server_message));



    if ( recvfrom(socket_desc, client_message, sizeof(client_message), 0, (SOCKADDR*)&client_address,&sizeof_client_address) < 0) {
        printf("Couldn't receive\n");
    }
    printf("IP: %s and port: %i\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    printf("Count of the vowels: %s\n", client_message);
    
    int looplimit, itr;
    looplimit = atoi(client_message);
    memset(client_message, '\0', sizeof(client_message));
    int loopcounter = 0;
    
    // 8 server recieves the vowels seperately, the number of recvfrom() is determined by the initial msg of how many vowels are there,
    //     stored as the looplimit.
    while( loopcounter < looplimit ) {
        if ( recvfrom(socket_desc, client_message, sizeof(client_message), 0, (SOCKADDR*)&client_address,&sizeof_client_address) < 0) {
            printf("Couldn't receive\n");
        }
        else {
            // Every letter is capitalized and added to the server_message array.
            int int_vowel = client_message[0];
            int_vowel -=32;
            char vowel;
            vowel = (char)int_vowel;
            server_message[loopcounter] = vowel;
            
            memset(client_message, '\0', sizeof(client_message));
            loopcounter++;
        }
    }

    // Sending all of the uppercase vowels inside the server_message as a whole.
    server_message[looplimit] = '\0';
    if( sendto( socket_desc, server_message, strlen(server_message), 0, (SOCKADDR*)&client_address, sizeof_client_address) < 0 ){
        printf("Couldn't send %d\n", WSAGetLastError());
    }
    

    // 11 The server waits for more packets...
    printf("\n\nWaiting for more packets...");
    while (1) {
        if ( recvfrom(socket_desc, client_message, sizeof(client_message), 0, (SOCKADDR*)&client_address,&sizeof_client_address) < 0) {
            printf("Couldn't receive\n");
        }
        else {
            printf("IP: %s and port: %i\n ", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            printf("Msg from client: %s\n", client_message);
        }
    }


    return 0;
}
