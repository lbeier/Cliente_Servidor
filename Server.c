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
#define BACKLOG 5
#define LENGTH 512 

int main() {
    int sockfd;
    int nsockfd;
    int num;
    int sin_size;
    struct sockaddr_in addr_local; /* client addr */
    struct sockaddr_in addr_remote; /* server addr */
    char revbuf[LENGTH]; // Receiver buffer

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERRO: Problema ao criar o socket! (errno = %d)\n", errno);
        exit(1);
    } else
        printf("[Servidor] Socket obtido com sucesso.\n");

    addr_local.sin_family = AF_INET; // Protocol Family
    addr_local.sin_port = htons(PORT); // Port number
    addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
    bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct

    /* Bind a special Port */
    if (bind(sockfd, (struct sockaddr*) &addr_local, sizeof (struct sockaddr)) == -1) {
        fprintf(stderr, "ERRO: Problema ao bindar a porta. (errno = %d)\n", errno);
        exit(1);
    } else {
        printf("[Servidor] Porta %d bindada em 127.0.0.1 com sucesso.\n", PORT);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        fprintf(stderr, "ERRO: Problema para ouvir a porta. (errno = %d)\n", errno);
        exit(1);
    } else {
        printf("[Servidor] Ouvindo a porta %d com sucesso.\n", PORT);
    }

    int success = 0;
    while (success == 0) {
        sin_size = sizeof (struct sockaddr_in);

        if ((nsockfd = accept(sockfd, (struct sockaddr *) &addr_remote, &sin_size)) == -1) {
            fprintf(stderr, "ERRO: Problema ao bindar a porta. (errno = %d)\n", errno);
            exit(1);
        } else {
            printf("[Servidor] Servidor foi conctado de %s.\n", inet_ntoa(addr_remote.sin_addr));
        }

	/* AQUI É ONDE DEFINIMOS QUAL ARQUIVO SERÁ ENVIADO */
        char* fr_name = "/home/tuta/NetBeansProjects/TrabalhoDeRedes/Recebido_do_Cliente.txt";
	/**************************************************/

        FILE *fr = fopen(fr_name, "a");
        if (fr == NULL) {
            printf("Problema com arquivo %s.\n", fr_name);
        } else {
            bzero(revbuf, LENGTH);
            int fr_block_sz = 0;
            while ((fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0)) > 0) {
                int write_sz = fwrite(revbuf, sizeof (char), fr_block_sz, fr);
                if (write_sz < fr_block_sz) {
                    error("Problema ao salvar o arquivo no servidor.\n");
                }
                bzero(revbuf, LENGTH);
                if (fr_block_sz == 0 || fr_block_sz != 512) {
                    break;
                }
            }
            if (fr_block_sz < 0) {
                if (errno == EAGAIN) {
                    printf("recv() timed out.\n");
                } else {
                    fprintf(stderr, "recv() falhou em virtude de errno = %d\n", errno);
                    exit(1);
                }
            }
            printf("[Servidor] Arquivo recebido com sucesso!\n");
            fclose(fr);
        }
    }
}
