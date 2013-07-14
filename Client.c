#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 20000
#define LENGTH 512 

int main(int argc, char *argv[]) {
    int sockfd;
    int nsockfd;
    char revbuf[LENGTH];
    struct sockaddr_in remote_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERRO: Problema ao criar o socket! (errno = %d)\n", errno);
        exit(1);
    }

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr);
    bzero(&(remote_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr)) == -1) {
        fprintf(stderr, "ERRO: Problema ao conectar no host! (errno = %d)\n", errno);
        exit(1);
    } else {
        printf("[Cliente] Conectado ao servidor na porta %d.\n", PORT);
    }

    /* AQUI É ONDE DEFINIMOS QUAL ARQUIVO SERÁ ENVIADO */
    char* fs_name = "/home/tuta/NetBeansProjects/TrabalhoDeRedes/Teste_Cliente.txt";
    /***************************************************/

    char sdbuf[LENGTH];
    printf("[Cliente] Enviando %s para o servidor... \n", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if (fs == NULL) {
        printf("ERRO: Arquivo %s não encontrado.\n", fs_name);
        exit(1);
    }

    bzero(sdbuf, LENGTH);
    int fs_block_sz;
    while ((fs_block_sz = fread(sdbuf, sizeof (char), LENGTH, fs)) > 0) {
        if (send(sockfd, sdbuf, fs_block_sz, 0) < 0) {
            fprintf(stderr, "ERRO: Erro ao enviar o arquivo %s. (errno = %d)\n", fs_name, errno);
            break;
        }
        bzero(sdbuf, LENGTH);
    }

    printf("[Cliente] Arquivo %s enviado ao servidor!\n", fs_name);

    close(sockfd);
    printf("[Cliente] Conexão encerrada.\n");
    return (0);
}
