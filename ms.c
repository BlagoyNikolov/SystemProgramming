#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#define ALL_PRODUCTS 10

typedef enum { false, true } bool;

typedef struct Product {
	int id;
	char* name;
	double price;
} Product;

void doprocessing(int sock);
double getSum(int barcodes[]);
void print(int allProducts[]);
double getProductPrice(int barcode);

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	double sum = 0;
	struct sockaddr_in serv_addr, cli_addr;
	int n, pid;


	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = 5001;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	/* Now start listening for the clients, here
	   * process will go in sleep mode and will wait
	   * for the incoming connection
	*/

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}

		/* Create child process */
		pid = fork();

		if (pid < 0) {
			perror("ERROR on fork");
			exit(1);
		}

		if (pid == 0) {
			/* This is the client process */
			doprocessing(newsockfd);
			close(sockfd);
			exit(0);
		}
		else {
			close(newsockfd);
		}

	} /* end of while */
}

void doprocessing(int sock) {
	int choices[10];
	double sum = 0;
	int received_int;
	int n;
	int i;
	double price;

	while (1) {
		n = read(sock, &received_int, sizeof(received_int));
		int product_id = ntohl(received_int);

		if (n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}

		if (product_id == 0) {
			break;
		}

		price = getProductPrice(product_id);
		if (price == -1) {
			break;
		}
		else {
			sum += price;
		}
	}
	n = write(sock, &sum, sizeof(sum));

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}

double getProductPrice(int barcode) {
	Product product0 = { 1,"Coke",1.5 };
	Product product1 = { 2,"Wiskey",5.5 };
	Product product2 = { 3,"3in1",1.2 };
	Product product3 = { 4,"Espresso", 3.4 };
	Product product4 = { 5,"Napkins",56.5 };
	Product product5 = { 6,"Vodka",4.6 };
	Product product6 = { 7,"Jin",4.5 };
	Product product7 = { 8,"Cocaine",200 };
	Product product8 = { 9,"Juice", 2.5 };
	Product product9 = { 10,"Hookah", 15 };

	Product allProducts[] = { product0, product1, product2, product3, product4, product5, product6, product7 ,product8, product9 };

	int i = 0;
	for (i = 0; i < ALL_PRODUCTS; i++) {
		if (allProducts[i].id == barcode) {
			return allProducts[i].price;
		}
	}
	return -1;
}
