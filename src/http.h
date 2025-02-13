#ifndef HTTP_H
#define HTTP_H

#include <arpa/inet.h>
#include <b64/cencode.h>
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4242
#define MAX_CONNECTIONS 50
#define MAX_VLINE_SIZE 128
#define MAX_HEADER_KEY_SIZE 64
#define MAX_HEADER_VAL_SIZE 128
#define MAX_HEADERS 20

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// http status codes
#define NOT_FOUND 404
#define INTERNAL_SERVER_ERROR 500
#define BAD_REQUEST 400
#define OK 200

struct {
  char key[MAX_HEADER_KEY_SIZE];
  char value[MAX_HEADER_VAL_SIZE];
} typedef header;

struct {
  char version_line[MAX_VLINE_SIZE];
  header headers[MAX_HEADERS];
  int header_count;
  char *body;
} typedef http_response;

struct {
  int connfd;
  char ip[INET_ADDRSTRLEN];
  char method[4];
  char path[64];
  header headers[MAX_HEADERS];
  int header_count;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len;
} typedef connection;

struct {
  int opcode;
  int length;
  unsigned char frame[1024];
  unsigned char mask[1024];
  unsigned char message[1024];
} typedef ws_frame;

/*
 * Finds the value of a request header by key
 * @return value of header, NULL if header not found
 * @param request object
 * @param header key
 */
char *get_header_val(connection *conn, char *key);

/*
 * Handles sending protocol upgrade response via the HTTP protocol
 * @return error code (0 if successful)
 * @param socket file descriptor
 * @param base64 encoded ws key
 */
int send_ws_upgrade_response(int fd, char *encoded_key);

/*
 * Handles sending text/html content via the HTTP protocol
 * @return error code (0 if successful)
 * @param socket file descriptor
 * @param text or html content
 */
int send_http_response(int fd, int status_code, char* message, char *content);

/*
 * Handles sending text/html content via the HTTP protocol
 * @return error code (0 if successful)
 * @param socket file descriptor
 * @param request struct to be populated
 */
int parse_http_request(connection *conn);

/*
 * Routes requests to static assets or websocket upgrade
 * @return error code (0 if successful)
 * @param socket file descriptor
 * @param request struct to be populated
 */
int route_request(connection *conn);

/**
 * Handles sending the home page, websocket protocol upgrade
 * @return void*
 * @param client id
 */
void *handle_conn(void *arg);

/**
 * Upgrades the connection to websocket protocol
 * @return void*
 * @param socket file descriptor
 * @param Httprequest struct
 */
void upgrade_conn(connection *conn);

#endif
