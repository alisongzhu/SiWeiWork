/*
 *  Copyright 2016 MZ Automation GmbH
 *
 *  This file is part of lib60870-C
 *
 *  lib60870-C is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lib60870-C is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lib60870-C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */


#include <string.h>

//#include <sys/select.h>

#include "hal_socket.h"
#include "hal_thread.h"
#include "lib_memory.h"

#include <cmsis_os.h>
#include <ring_buffer.h>
#include <usart.h>

#define INET6_ADDRSTRLEN 46
#define DEBUG_SOCKET 0

struct sSocket {
  int fd;
  uint32_t connectTimeout;
};

struct sServerSocket {
  int fd;
  int backLog;
};

struct sHandleSet {
//  fd_set handles;
  int maxHandle;
};

static int connected = 0;

extern SemaphoreHandle_t uart_sem;
extern struct ring_buf rs485_rb;


HandleSet Handleset_new(void)
{
  HandleSet result = (HandleSet) GLOBAL_MALLOC(sizeof(struct sHandleSet));

//  if (result != NULL) {
//    FD_ZERO(&result->handles);
//    result->maxHandle = -1;
//  }
  return result;
}

void Handleset_reset(HandleSet self) {
//  FD_ZERO(&self->handles);
//  self->maxHandle = -1;
}

void Handleset_addSocket(HandleSet self, const Socket sock)
{
//  if (self != NULL && sock != NULL && sock->fd != -1)
//  {
//    FD_SET(sock->fd, &self->handles);
//    if (sock->fd > self->maxHandle) {
//      self->maxHandle = sock->fd;
//    }
//  }
}

int Handleset_waitReady(HandleSet self, unsigned int timeoutMs)
{
  int result;

//  if (self != NULL && self->maxHandle >= 0) {
//    struct timeval timeout;
//
//    timeout.tv_sec = timeoutMs / 1000;
//    timeout.tv_usec = (timeoutMs % 1000) * 1000;
//    result = select(self->maxHandle + 1, &self->handles, NULL, NULL, &timeout);
//  } else {
//    result = -1;
//  }

  if(xSemaphoreTake(uart_sem,timeoutMs) == pdTRUE)
  {
    result = -1;
  }
  else
  {
    result = 0;
  }

  return result;
//  return 0;
}

void Handleset_destroy(HandleSet self)
{
  GLOBAL_FREEMEM(self);
}

#if (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1)
static void
activateKeepAlive(int sd)
{
#if defined SO_KEEPALIVE
  int optval;
  socklen_t optlen = sizeof(optval);

  optval = CONFIG_TCP_KEEPALIVE_IDLE;
  setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
  optval = 1;
  setsockopt(sd, SOL_SOCKET, SO_NOSIGPIPE, &optval, optlen);

#if defined TCP_KEEPCNT
  optval = CONFIG_TCP_KEEPALIVE_INTERVAL;
  setsockopt(sd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen);

  optval = CONFIG_TCP_KEEPALIVE_CNT;
  setsockopt(sd, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen);
#endif /* TCP_KEEPCNT */

#endif /* SO_KEEPALIVE */
}
#endif /* (CONFIG_ACTIVATE_TCP_KEEPALIVE == 1) */

//static bool prepareServerAddress(const char* address, int port,
//                                 struct sockaddr_in* sockaddr)
//{
//  return true;
//}

ServerSocket TcpServerSocket_create(const char* address, int port)
{
//  ServerSocket serverSocket = NULL;
//
//  int fd;
//
//  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
//    struct sockaddr_in serverAddress;
//
//    if (!prepareServerAddress(address, port, &serverAddress)) {
//      close(fd);
//      return NULL;
//    }
//
//    int optionReuseAddr = 1;
//    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char * ) &optionReuseAddr,
//               sizeof(int));
//
//    if (bind(fd, (struct sockaddr * ) &serverAddress, sizeof(serverAddress))
//        >= 0) {
//      serverSocket = GLOBAL_MALLOC(sizeof(struct sServerSocket));
//      serverSocket->fd = fd;
//      serverSocket->backLog = 0;
//    } else {
//      close(fd);
//      return NULL;
//    }
//
//#if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
//    activateKeepAlive(fd);
//#endif
//
//    setSocketNonBlocking((Socket) serverSocket);
//  }
  ServerSocket serverSocket = NULL;

  serverSocket = GLOBAL_MALLOC(sizeof(struct sServerSocket));

  return serverSocket;
}

void ServerSocket_listen(ServerSocket self)
{

}

Socket ServerSocket_accept(ServerSocket self)
{
  int fd;

  Socket conSocket = NULL;

  if(connected == 0)
  {
    connected = 1;
    conSocket = TcpSocket_create();
    conSocket->fd = fd;
  }

  return conSocket;
}

//void ServerSocket_setBacklog(ServerSocket self, int backlog)
//{
//  self->backLog = backlog;
//}


void ServerSocket_destroy(ServerSocket self)
{
  int fd = self->fd;

  self->fd = -1;

//  connected = 0;

  Thread_sleep(10);

  GLOBAL_FREEMEM(self);
}

Socket TcpSocket_create()
{
  Socket self = GLOBAL_MALLOC(sizeof(struct sSocket));

  self->fd = -1;

  return self;
}

void Socket_setConnectTimeout(Socket self, uint32_t timeoutInMs)
{
  self->connectTimeout = timeoutInMs;
}

bool Socket_connect(Socket self, const char* address, int port)
{
  return true;
}

char*
Socket_getPeerAddress(Socket self)
{
  return NULL;
}

int Socket_read(Socket self, uint8_t* buf, int size)
{
//  if (self->fd == -1)
//    return -1;
//
//  int read_bytes = recv(self->fd, buf, size, MSG_DONTWAIT);
//
//  if (read_bytes == 0)
//    return -1;
//
//  if (read_bytes == -1) {
//    int error = errno;
//
//    switch (error) {
//
//      case EAGAIN:
//        return 0;
//      case EBADF:
//        return -1;
//
//      default:
//        return -1;
//    }
//  }

//  return read_bytes;
  int read_bytes;

  read_bytes = ring_buf_get(&rs485_rb, buf, size);

  if(read_bytes == 0)
  {
    read_bytes = -1;
  }

  return read_bytes;
}

int Socket_write(Socket self, uint8_t* buf, int size)
{
  HAL_StatusTypeDef status;
  status = HAL_UART_Transmit(&huart3, buf, size, 0xff);

  if(status == HAL_OK)
  {
    return size;
  }else{
    return -1;
  }
}

void Socket_destroy(Socket self)
{
  int fd = self->fd;

  self->fd = -1;

  connected = 0;
//  closeAndShutdownSocket(fd);

  Thread_sleep(10);

  GLOBAL_FREEMEM(self);
}
