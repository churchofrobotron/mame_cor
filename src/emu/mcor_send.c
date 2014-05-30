#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "mcor_send.h"
#include "machine/nvram.h"
#include "debug/debugcon.h"
#include "debug/debugcmd.h"

int mini_printf(running_machine &machine, char *buffer, const char *format, int params, UINT64 *param);
//int debug_command_parameter_number(running_machine &machine, const char *param, UINT64 *result);

int mcor_sock;
struct sockaddr_in server_addr;
struct sockaddr_in broadcast_address;

void mcor_send_init(const char *ip_address, const char* bc_address, int port){
	if ((mcor_sock = socket(AF_INET, SOCK_DGRAM, /*IPPROTO_UDP*/ 0))==-1){
		perror("[mcor] socket");
	}
  int broadcastEnable=1;
  setsockopt(mcor_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
  setsockopt(mcor_sock, SOL_SOCKET, SO_REUSEADDR, &broadcastEnable, sizeof(broadcastEnable));
	memset((char *) &server_addr, 0, sizeof(server_addr));
  memset((char*) &broadcast_address, 0, sizeof(broadcast_address));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
  broadcast_address.sin_family = AF_INET;
  broadcast_address.sin_port = htons(port);
	printf("[mcor] Initializing sender for %s:%d...\n", ip_address, port);
	if (inet_aton(ip_address, &server_addr.sin_addr) == 0) {
		fprintf(stderr, "[mcor] inet_aton() failed\n");
		exit(1);
	}

  if (inet_aton(bc_address, &broadcast_address.sin_addr) == 0)
  {
    fprintf(stderr, "[mcor] inet_aton() for broadcast failed\n");
    exit(1);
  }

  if (bind(mcor_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
  {
    perror("[mcor] bind failed");
    exit(1);
  }
}

void mcor_send_cleanup(){
	close(mcor_sock);
}

void mcor_send(const char *data, int length){
	//printf("Sending data of length %d\n", length);
	int rc = sendto(mcor_sock, data, length, 0, (struct sockaddr*)&broadcast_address, sizeof(broadcast_address));
	if (rc == -1){
		perror("[mcor] sendto() failed!");
		exit(1);
	}
}

void execute_mcor_init(running_machine &machine, int ref, int params, const char **param)
{
  if (params < 2)
  {
    fprintf(stderr, "[mcor] init failed, not enough parameters\n");
    exit(1);
  }
  mcor_send_init(param[0], param[1], MCOR_PORT);
}

void execute_mcor_send(running_machine &machine, int ref, int params, const char **param)
{
	UINT64 values[MAX_COMMAND_PARAMS];
	char buffer[1024];
	int i;

	/* validate the other parameters */
	for (i = 1; i < params; i++)
		if (!debug_command_parameter_number(machine, param[i], &values[i]))
			return;

	/* then do a printf */
	if (mini_printf(machine, buffer, param[0], params - 1, &values[1]))
   {
      mcor_send(buffer, strlen(buffer));
   }
}

void execute_mcor_dumpscores(running_machine& machine, int ref, int params, const char** param)
{
  machine.nvram_save();
}
