/* =========================================================================
 * Created on: <Tue Apr 21 19:08:19 +01 2026>
 * Time-stamp: <Thu Apr 23 10:15:02 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/echoservers.c -
 *
 * Figure 12.8 Concurrent echo server based on I/O multiplexing. Each
 * server iteration echoes a text line from each ready descriptor.
 * See [[file:echoserverp.c]], [[file:echoservert.c]]
 * ========================================================================= */
#include "../include/csapp.h"

/**
 * Represents a pool of connected descriptors.
 * @field maxfd: Largest fd in read_set to provide for select()
 *               cardinality first argument.
 * @field maxi : Largest index into clientfd array precluding
 *               check_clients() function from searching up into
 *               empty parts of the FD_SETSIZE-element array
 */
typedef struct {
  int maxfd;        /* Largest descriptor in read_set */
  fd_set read_set;  /* Set of all active descriptors */
  fd_set ready_set; /* Subset of descriptors ready for reading  */
  int nready;       /* Number of ready descriptors returned by select() */
  int maxi;         /* Highwater index into client array */
  int clientfd[FD_SETSIZE];    /* Set of active descriptors */
  rio_t clientrio[FD_SETSIZE]; /* Set of active read buffers */
} pool_t;

void init_pool(int listenfd, pool_t *p);
void add_client(int connfd, pool_t *p);
void check_clients(pool_t *p);

/* Maintain a cumulative tally of all bytes received from clients. */
int byte_cnt = 0;

int main(int argc, char **argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  static pool_t pool;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  listenfd = Open_listenfd(argv[1]);
  init_pool(listenfd, &pool); /* init pool */

  while (1) {
    /* Wait for listening/connected descriptor(s) to become ready */
    pool.ready_set = pool.read_set;
    pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

    /* (1) Is listening descriptor ready, add new client to pool */
    if (FD_ISSET(listenfd, &pool.ready_set)) { /* listenfd ready */
      clientlen = sizeof(struct sockaddr_storage);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      add_client(connfd, &pool);
    }

    /* (2) Is any client connected descriptor ready, service it. */
    check_clients(&pool);
  }
}

/* Figure 12.9 */
/* init_pool initializes the pool of active clients. */
void init_pool(int listenfd, pool_t *p) {
  /* Initially, set of connected descriptors is empty */
  int i;
  p->maxi = -1;
  for (i = 0; i < FD_SETSIZE; i++)
    p->clientfd[i] = -1;

  /* Initially, listenfd is only member of select read set */
  p->maxfd = listenfd;
  FD_ZERO(&p->read_set);
  FD_SET(listenfd, &p->read_set);
}

/* Figure 12.10 */
/* add_client adds a new client connection to the pool. */
void add_client(int connfd, pool_t *p) {
  int i;
  p->nready--;

  for (i = 0; i < FD_SETSIZE; i++) { /* Find an available slot */
    if (p->clientfd[i] < 0) {
      /* Add connected descriptor to the pool */
      p->clientfd[i] = connfd;
      /* Initialize corresponding Rio read buffer */
      Rio_readinitb(&p->clientrio[i], connfd);

      /* Add the descriptor to descriptor set */
      FD_SET(connfd, &p->read_set);

      /* Update max descriptor and pool highwater mark */
      if (connfd > p->maxfd)
        p->maxfd = connfd;
      if (i > p->maxi)
        p->maxi = i;
      break;
    }
  }
  if (i == FD_SETSIZE) /* Couldn't find an empty slot */
    app_error("add_client error: Too many clients");
}

/* Figure 12.11 */
/* check_clients services ready client connections. */
void check_clients(pool_t *p) {
  int i, connfd, n;
  char buf[MAXLINE];
  rio_t rio;

  for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
    connfd = p->clientfd[i];
    rio = p->clientrio[i];

    /* If the descriptor is ready, echo a text line from it */
    if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
      p->nready--;
      if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        /* Successfully read line from client */
        byte_cnt += n; /* Keep global numBytes tally. */
        printf("Server received %d (%d total) bytes on fd %d\n", n, byte_cnt,
               connfd);
        /* echo line back to client */
        Rio_writen(connfd, buf, n);
      }

      /* EOF detected, remove descriptor from pool */
      else {
        Close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[i] = -1;
      }
    }
  }
}
