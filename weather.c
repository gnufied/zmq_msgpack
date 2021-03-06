   //  Binds PUB socket to tcp://*:5556
   //  Publishes random weather updates

#include "zhelpers.h"
#include "msgpack.h"
#include <unistd.h>

int main (void)
{
  //  Prepare our context and publisher
  void *context = zmq_ctx_new ();
  void *publisher = zmq_socket (context, ZMQ_PUB);
  int rc = zmq_bind (publisher, "tcp://*:5556");
  assert (rc == 0);
  rc = zmq_bind (publisher, "ipc://weather.ipc");
  assert (rc == 0);
  msgpack_sbuffer* buffer;
  msgpack_packer* pk;

  //  Initialize random number generator
  srandom ((unsigned) time (NULL));
  while (1) {
    buffer = msgpack_sbuffer_new();
    pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
    //  Get values that will fool the boss
    int zipcode, temperature, relhumidity;
    zipcode     = randof (100000);
    temperature = randof (215) - 80;
    relhumidity = randof (50) + 10;

    //  Send message to all subscribers
    char update [20];
    sprintf (update, "%d %d %d", zipcode, temperature, relhumidity);
    msgpack_pack_raw(pk, 20);
    msgpack_pack_raw_body(pk, update, 20);
    printf ("zip code is %d, temp %d, relhumidity %d\n", zipcode, temperature, relhumidity);
    zmq_send(publisher, buffer->data, buffer->size, 0);
    sleep (1);
    msgpack_packer_free(pk);
    msgpack_sbuffer_free(buffer);
  }
  zmq_close (publisher);
  zmq_ctx_destroy (context);
  return 0;
}
