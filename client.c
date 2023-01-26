#if defined WIN32
    #include <winsock2.h>
    #include <winsock.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#define BUFF_SIZE 512

int main( int argc, char *argv[]) {

    //  Winsock initialization.
    #if defined WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
        if ( iResult != 0){
            printf("Error at WSAStartup()\n");
            return 0;
        }
    #endif

    // 0 Setting up the socket.
    SOCKET socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    // Checking if the socket is created succesfuly.
    if ( socket_desc == INVALID_SOCKET ){
        return (SOCKET)NULL;
    }

    // Setting up the variables.
    char client_message[BUFF_SIZE];
    char server_message[BUFF_SIZE];
    struct sockaddr_in server_address = {0};
    memset(server_message, '\0', sizeof(server_message));

    // 1 Client reads the hostname(e.g. "localhost") or the hostaddr(e.g. 127.0.0.1) to use from the argv[1]
    // 1 Client reads the port number to use from argv[2]


    // HERE YOU CAN USE THE GETHOSTBYADDR()
    // const char *host_addr;
    // host_addr = argv[1];
    // struct hostent *HostEntity;
    // struct in_addr IPv4_address = {0};
    // IPv4_address.s_addr = inet_addr(host_addr);
    // HostEntity = gethostbyaddr( (char*)&IPv4_address, 4, AF_INET);

    // HERE YOU CAN USE THE GETHOSTBYNAME()
    const char *host_name;
    host_name = argv[1];
    struct hostent *HostEntity;
    HostEntity = gethostbyname(host_name);
    struct in_addr *IPv4_address = (struct in_addr*)HostEntity->h_addr_list[0];
    printf("Name of the host: %s\n", HostEntity->h_name);
    printf("Result of gethostbyname(%s): %s\n", host_name, inet_ntoa(*IPv4_address));
    

    int sizeof_server_address = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));
    // Delete the "*" infront of the IPv4_address if you'd like to use the gethostbyaddr() function instead of the gethostbyname() function.
    //     Because they're declared a bit differently in the two methods.
    server_address.sin_addr.s_addr = inet_addr( inet_ntoa(*IPv4_address) );


    // 2 the client send message to the server.
    strcpy(client_message,"Hey man wassup?");
    if( sendto(socket_desc, client_message, strlen(client_message), 0, (struct sockaddr*)&server_address, sizeof_server_address) < 0){
        printf("Unable to send message\n");
    }

    // 6 The OK message from the server.
    if ( recvfrom(socket_desc, server_message, sizeof(server_message), 0,(SOCKADDR*)&server_address,&sizeof_server_address) < 0) {
        printf("Couldn't receive\n");
        return -1;
    }

    printf("Received message from IP: %s and port(sender): %i\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
    printf("Msg from server: %s\n", server_message);
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));


    // 7 START client reads from the stdin and counts the vowels, and sends them
    printf("Enter the message to send to the server:\n");
    gets(client_message);
    // We need to hold the client_message in an temporary array.
    char temp_arr[BUFF_SIZE];
    strcpy(temp_arr, client_message);

    char *ptr_msg;
    ptr_msg = client_message;

    int num_of_vowels = 0;
    int itr = 0;

    for ( itr = 0 ; itr < strlen(client_message) ; itr++ ) {
        if ( *(ptr_msg+itr) == 'a' || *(ptr_msg+itr) == 'e' || *(ptr_msg+itr) == 'i' || *(ptr_msg+itr) == 'o' || *(ptr_msg+itr) == 'u' ) {
            num_of_vowels++;
        }
    }
    memset(client_message, '\0', sizeof(client_message));
    
    // printf("\n\nNumber of vowels: %d\n", num_of_vowels);
    itoa(num_of_vowels, ptr_msg, 10);

    // Send the number of vowels.
    if ( sendto(socket_desc, client_message, strlen(client_message), 0, (struct sockaddr*)&server_address, sizeof_server_address) < 0 ) {
        printf("Unable to send msg. %d",WSAGetLastError());
    }
    
    strcpy(client_message, temp_arr);

    char load[2];
    char *ptr_load;
    ptr_load = load;

    for ( itr = 0 ; itr < strlen(client_message) ; itr++ ) {
        if ( *(ptr_msg+itr) == 'a' || *(ptr_msg+itr) == 'e' || *(ptr_msg+itr) == 'i' || *(ptr_msg+itr) == 'o' || *(ptr_msg+itr) == 'u' ) {            
            *ptr_load = *(ptr_msg+itr);
            // printf("%s\n", load);
            if (sendto(socket_desc, ptr_load, strlen(load), 0, (struct sockaddr*)&server_address, sizeof_server_address ) < 0) {
                    printf("Unable to send message\n");
            }
        }
    }
    // 7 END


    // 9 The client recives the server_message, it contains the uppercase letters.
    if( recvfrom(socket_desc, server_message, sizeof(server_message),0,(SOCKADDR*)&server_address, &sizeof_server_address) < 0 ){
        printf("Couldn't recieve %d\n",WSAGetLastError());
    }
    else{
        printf("\nThe uppercase letters:\n%s", server_message);
    }


    // 10 The client/customer terminates.
    closesocket(socket_desc);
    #if defined WIN32
        WSACleanup();
    #endif

    return 0;
}
