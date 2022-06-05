#include "common.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
client *clients[MAX_CLIENTS] = {NULL};
int client_count = 0;
struct pollfd fds[MAX_CLIENTS];

// create local socket
int create_local_socket(char *socket_path) {
    int sockfd;
    struct sockaddr_un server_addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }


    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, socket_path);
    unlink(server_addr.sun_path);
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(1);
    }

    printf("LOCAL Server is listening on %s\n", socket_path);
    printf("LOCAL socket fd: %d\n", sockfd);

    return sockfd;
}

// create network socket
int create_inet_socket(char *port) {
    int sockfd;
    struct sockaddr_in server_addr_inet;
    socklen_t server_addr_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    server_addr_inet.sin_family = AF_INET;
    server_addr_inet.sin_port = htons(atoi(port));
    server_addr_inet.sin_addr.s_addr = inet_addr("127.0.0.1");
    // resuse if socket is in TIME_WAIT state
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


    server_addr_len = sizeof(server_addr_inet);

    // set port and IP address
    if (bind(sockfd, (struct sockaddr *) &server_addr_inet, server_addr_len) < 0) {
        perror("bind");
        exit(1);
    }

    // listen for connections
    if (listen(sockfd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(1);
    }

    printf("INET Server is listening on port %s\n", port);
    printf("INET socket fd: %d\n", sockfd);

    return sockfd;
}

int get_oponent(int idx){
  return idx % 2 == 0 ? idx + 1 : idx - 1;
}

void free_client(int i) {
    int oponent_index = get_oponent(i);
    if (clients[oponent_index] != NULL) {
        free(clients[oponent_index]->name);
        free(clients[oponent_index]);
        clients[oponent_index] = NULL;
        client_count--;
    }

    free(clients[i]->name);
    free(clients[i]);
    clients[i] = NULL;
    client_count--;

}

void delete_non_available_clients(){
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            if (! clients[i]->available) {
                free_client(i);
            }
        }
    }
}

void ping_clients() {
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            if (send(clients[i]->fd, "ping: ", MAX_MSG_LEN, 0) < 0) {
                perror("send");
                exit(1);
            }
            clients[i]->available = false;
        }
    }
}

void* ping(){
  while (1) {
    printf("ping\n");
    pthread_mutex_lock(&mutex);
    delete_non_available_clients();
    ping_clients();
    pthread_mutex_unlock(&mutex);

    sleep(10);
  }
}


int check_messages(int local_socket, int network_socket){
  int cnt = 0;

  memset(&fds[0], 0, MAX_POLL * sizeof(struct pollfd));
  fds[0].fd = network_socket;
  fds[0].events = POLLIN;
  fds[1].fd = local_socket;
  fds[1].events = POLLIN;

  pthread_mutex_lock(&mutex);
  for(int i = 0; i < MAX_CLIENTS; i++){
      if(clients[i] == NULL){
          continue;
      }
      cnt++;
      fds[i + 2].fd = clients[i]->fd;
      fds[i + 2].events = POLLIN;
  }
  pthread_mutex_unlock(&mutex);

  poll(fds, 2 + cnt, -1);

  int fd_ret = -1;
  for(int i = 0; i < MAX_POLL; i++) {
      if(fds[i].revents & POLLIN){
          fd_ret = fds[i].fd;
          break;
      }
  }

  if(fd_ret == network_socket || fd_ret == local_socket){
      fd_ret = accept(fd_ret, NULL, NULL);
  }
  return fd_ret;
}



int add_client(int fd, char* name) {
    // check if client with the same name exists
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            if (strcmp(clients[i]->name, name) == 0) {
                return -1;
            }
        }
    }

    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            idx = i;
            break;
        }
    }

    if (idx != -1) {
        client *new_client = calloc(1, sizeof(client));
        new_client->name = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(new_client->name, name);
        new_client->fd = fd;
        new_client->available = true;

        clients[idx] = new_client;
        client_count++;
    }

    return idx;
}

int get_client_by_name(char *name){
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i] != NULL) {
      if (strcmp(clients[i]->name, name) == 0) {
        return i;
      }
    }
  }
  return -1;
}

int main(int argc, char *argv[]) {
  // parse server program arguements containing port number and socket path
  if (argc != 3) {
    printf("Usage: %s <port> <socket path>\n", argv[0]);
    exit(1);
  }

  char *port = argv[1];
  char *socket_path = argv[2];

  int local_socket = create_local_socket(socket_path);
  int network_socket = create_inet_socket(port);


  pthread_t t;
  pthread_create(&t, NULL, &ping, NULL);
  char buffer[MAX_MSG_LEN + 1] = {};
  
  while (1) {
    int client_fd = check_messages(local_socket, network_socket);
    
    sleep(1);
    if (recv(client_fd, buffer, MAX_MSG_LEN, 0) == -1) {
      perror("recv");
      exit(1);
    }

    printf("Received: %s\n", buffer);
    char *command = strtok(buffer, ":");
    char *arg = strtok(NULL, ":");
    char *name = strtok(NULL, ":");

    pthread_mutex_lock(&mutex);

    if (strcmp(command, "add") == 0) {
      int index = add_client(client_fd, name);

      if (index == -1) {
        send(client_fd, "add:name_taken", MAX_MSG_LEN, 0);
        close(client_fd);
      } else if (index % 2 == 0){
        send(client_fd, "add:no_enemy", MAX_MSG_LEN, 0);
      } else {
        int random_num = rand() % 100;
        int first, second;

        if (random_num % 2 == 0){
          first = index;
          second = get_oponent(index);
        } else {
          second = index;
          first = get_oponent(index);
        }

        send(clients[first]->fd, "add:O", MAX_MSG_LEN, 0);
        send(clients[second]->fd, "add:X", MAX_MSG_LEN, 0);
        printf("paired clients\n");
      }
    }

    if (strcmp(command, "move") == 0) {
      int move = atoi(arg);
      int player = get_client_by_name(name);

      sprintf(buffer, "move:%d", move);
      send(clients[get_oponent(player)]->fd, buffer, MAX_MSG_LEN, 0);
    }
    
    if (strcmp(command, "end") == 0) {
      free_client(get_client_by_name(name));
    }
    
    if (strcmp(command, "pong") == 0) {
      int player = get_client_by_name(name);
      if (player != -1) {
        clients[player]->available = true;
      }
    }

    pthread_mutex_unlock(&mutex);
  }

  return 0;
}
