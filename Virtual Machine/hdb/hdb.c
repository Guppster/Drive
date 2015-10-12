#include "hdb.h"
#include <stdlib.h>

hdb_connection* hdb_connect(const char* server) 
{
	redisContext *c;
	redisReply *reply;
	int port = 6379;
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds

	c = redisConnectWithTimeout(server, port, timeout);

	if (c == NULL || c->err) 
	{
		if (c) 
		{
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		}
		else 
		{
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}

	return *(hdb_connection**)&c;
}

void hdb_disconnect(hdb_connection* con) {
  // "Disconnect" from the Redis server (i.e. free the Redis context)
  // See https://github.com/redis/hiredis/blob/master/examples/example.c
}

void hdb_store_file(hdb_connection* con, hdb_record* record) {
  // Store the specified record in the Redis server.  There are many ways to
  // do this with Redis.  Whichever way you choose, the checksum should be 
  // associated with the file, and the file should be associated with the user.
  //
  // Hint: look up the HSET command. 
  //
  // See https://github.com/redis/hiredis/blob/master/examples/example.c for
  // an example of how to execute it on the Redis server.
}

int hdb_remove_file(hdb_connection* con, const char* username, const char* filename) {
  // Remove the specified file record from the Redis server.

  return -99; // Remove me
}

char* hdb_file_checksum(hdb_connection* con, const char* username, const char* filename) {
  // If the specified file exists in the Redis server, return its checksum.
  // Otherwise, return NULL.

  return "REMOVE ME";
}

int hdb_file_count(hdb_connection* con, const char* username) {
  // Return a count of the user's files stored in the Redis server.

  return -99; // Remove me
}

bool hdb_user_exists(hdb_connection* con, const char* username) {
  // Return a Boolean value indicating whether or not the user exists in
  // the Redis server (i.e. whether or not he/she has files stored).

  return false; // Remove me
}

bool hdb_file_exists(hdb_connection* con, const char* username, const char* filename) {
  // Return a Boolean value indicating whether or not the file exists in
  // the Redis server.

  return false; // Remove me
}

hdb_record* hdb_user_files(hdb_connection* con, const char* username) {
  // Return a linked list of all the user's file records from the Redis
  // server.  See the hdb_record struct in hdb.h  -- notice that it 
  // already has a pointer 'next', allowing you to set up a linked list
  // quite easily.
  //
  // If the user has no files stored in the server, return NULL.

  return NULL; // Remove me
}

void hdb_free_result(hdb_record* record) {
  // Free up the memory in a linked list allocated by hdb_user_files().
}

int hdb_delete_user(hdb_connection* con, const char* username) {
  // Delete the user and all of his/her file records from the Redis server.

  return -99; // Remove me
}
