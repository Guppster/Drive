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

	reply = redisCommand((redisContext*)con, "HGET %s %s", username, filename);

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

	if (reply->integer == 1)
		return true;
	return false;

}//End of hdb_user_exists method

bool hdb_file_exists(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;

	reply = redisCommand((redisContext*)con, "HEXISTS %s %s", username, filename);

	if (reply->integer == 1)
		return true;
	return false;
}

hdb_record* hdb_user_files(hdb_connection* con, char* username) 
{
	redisReply *reply;
	hdb_record *head = NULL;
	hdb_record *temp;

	reply = redisCommand((redisContext*)con, "HGETALL %s", username);

	if (reply->type == REDIS_REPLY_ARRAY) 
	{
		for (int j = 0; j < reply->elements; j+=2)
		{
			temp = (hdb_record*)malloc(sizeof(hdb_record));		//allocate space for hdb_record 
			temp->username = username;
			//char = const char* (assigning const char to char not allowed)

			temp->filename = reply->element[j]->str;
			temp->checksum = reply->element[j+1]->str;

			temp->next = head;
			head = temp;
		}
	}

	return head;
}//End of hdb_user_files method

// Free up the memory in a linked list allocated by hdb_user_files().
void hdb_free_result(hdb_record* record) 
{
	hdb_record *node = record;
	hdb_record *temp;

	while (node != NULL)
	{
		temp = node;
		node = node->next;
		free(temp);
	}
	
	record = NULL;
}//End of hdb_free_result method

int hdb_delete_user(hdb_connection* con, const char* username) 
{
	redisReply *reply;
	
	reply = redisCommand((redisContext*)con, "DEL %s", username);

	return reply->integer;
}
