#include "hdb.h"
#include <stdlib.h>

hdb_connection* hdb_connect(const char* server) 
{
	redisContext *c;
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

void hdb_disconnect(hdb_connection* con) 
{
	/* Disconnects and frees the context */
	redisFree((redisContext*)con);
}

void hdb_store_file(hdb_connection* con, hdb_record* record) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "HSET %s %s %s", record->username, record->filename, record->checksum);

	freeReplyObject(reply);
}

int hdb_remove_file(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "HDEL %s %s ", username, filename);

	return (int)reply;
}

char* hdb_file_checksum(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "HEXISTS %s %s ", username, filename);

	if (reply->integer == 0)
	{
		return NULL;
	}

	reply = redisCommand((redisContext*)con, "GET %s %s ", username, filename);

	return reply->str;

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
