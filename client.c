//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode

#include "zhelpers.h"
#include "msgpack.h"

int main (int argc, char *argv [])
{
  //  Socket to talk to server
  printf ("Collecting updates from weather server…\n");
  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);
  int rc = zmq_connect (subscriber, "tcp://localhost:5556");
  assert (rc == 0);

  rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0);
  assert (rc == 0);

  //  Process 100 updates
  int update_nbr;
  for (update_nbr = 0; update_nbr < 100; update_nbr++) {
    zmq_msg_t msg;
    rc = zmq_msg_init (&msg);
    assert (rc == 0);
    /* Block until a message is available to be received from socket */
    rc = zmq_msg_recv (&msg, subscriber, 0);
    assert (rc != -1);
    printf ("Received something here\n");
    msgpack_unpacked unpacked_msg;
    msgpack_unpacked_init(&unpacked_msg);
    bool success = msgpack_unpack_next(&unpacked_msg, zmq_msg_data(&msg), zmq_msg_size(&msg), NULL);

    assert(success);
    const char *string = unpacked_msg.data.via.raw.ptr;

    int zipcode, temperature, relhumidity;
    sscanf (string, "%d %d %d",
            &zipcode, &temperature, &relhumidity);
    printf ("zip code is %d, temp %d, relhumidity %d\n", zipcode, temperature, relhumidity);
    msgpack_unpacked_destroy(&unpacked_msg);
    zmq_msg_close(&msg);
  }
  zmq_close (subscriber);
  zmq_ctx_destroy (context);
  return 0;
}
