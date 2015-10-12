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
}//End of hdb_connect method

void hdb_disconnect(hdb_connection* con) 
{
	/* Disconnects and frees the context */
	redisFree((redisContext*)con);
}//End of hdb_disconnect method

void hdb_store_file(hdb_connection* con, hdb_record* record) 
{
	redisCommand((redisContext*)con, "HSET %s %s %s", record->username, record->filename, record->checksum);
}//End of hdb_store_file method

int hdb_remove_file(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "HDEL %s %s", username, filename);

	return reply->integer;
}//End of hdb_remove_file method

char* hdb_file_checksum(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "HEXISTS %s %s", username, filename);

	if (reply->integer == 0)
	{
		return NULL;
	}

	reply = redisCommand((redisContext*)con, "GET %s %s", username, filename);

	return reply->str;
}//End of hdb_file_checksum method

int hdb_file_count(hdb_connection* con, const char* username) 
{
	redisReply *reply;
	
	reply = redisCommand((redisContext*)con, "HLEN %s", username);

	return reply->integer;

}//End of hdb_file_count method

bool hdb_user_exists(hdb_connection* con, const char* username) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "EXISTS %s", username);

	switch (reply->integer)
	{
	case 1: 
		freeReplyObject(reply);
		return true;
		break;
	case 2:
		freeReplyObject(reply);
		return false;
		break;
	}

}//End of hdb_user_exists method

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
