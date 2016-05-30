#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

typedef struct Product {
	int id;
	char* name;
	double price;
} Product;

void printMenu();
void printChange();

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];

	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	printMenu();
	portno = atoi(argv[2]);

	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	server = gethostbyname(argv[1]);

	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		exit(1);
	}

	/* Now ask for a message from the user, this message
	   * will be read by server
	*/

	printf("Please enter the barcodes: ");

	for (;;) {
		int number_to_send;
		scanf("%d", &number_to_send);

		if (number_to_send == 0) {
			int converted_number = htonl(number_to_send);
			n = write(sockfd, &converted_number, sizeof(converted_number));
			break;
		}

		if (number_to_send < 0 || number_to_send >10) {
			printf("No such product.\n");
			continue;
		}

		int converted_number = htonl(number_to_send);
		n = write(sockfd, &converted_number, sizeof(converted_number));

		if (n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}
	}

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}

	/* Now read server response */
	double sum = 0;
	n = read(sockfd, &sum, sizeof(sum));

	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("Sum is: %.2f\n", sum);
	printChange(sum);

	return 0;
}

void printChange(double sum) {
	double money;
	double change;
	printf("Give me money.\n");
	scanf("%lf", &money);
	change = money - sum;

	while (change < 0) {
		printf("Insufficient funds. Give me more money.\n");
		scanf("%lf", &money);
		change = money - sum;
	}
	printf("Change: %.2f\n", change);
}

void printMenu() {
	printf("Menu:\n");

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

	int i;
	for (i = 0; i < 10; i++) {
		printf("Barcode: %d,  %s,  %.2f$ \n", allProducts[i].id, allProducts[i].name, allProducts[i].price);
	}
}
