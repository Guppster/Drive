/*
Author:	Gurpreet Singh
Description:
*/

#include "hdb.h"
#include <stdlib.h>
#include <string.h>

/* Connect to the specified Hooli database server, returning the initialized connection */
hdb_connection* hdb_connect(const char* server) 
{
	redisContext *c;								//An object used to connect to the redis server
	int port = 6379;								//The port at which we will connect
	struct timeval timeout = { 1, 500000 };			//1.5 seconds to connect

	//Attempt to connect to Redis with specificed server/port/timeout
	c = redisConnectWithTimeout(server, port, timeout);

	//Check to see if the returned connection is valid (if not display appropriate error messsage)
	if (c == NULL || c->err) 
	{
		if (c) 
		{
			printf("Connection error: %s\n", c->errstr);

			//Free the memory assigned to the connection object
			redisFree(c);
		}
		else 
		{
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}

	//Returns a renamed pointer to redisContext for data hiding 
	return *(hdb_connection**)&c;
}//End of hdb_connect method

/* Disconnect from the Hooli database server. */
void hdb_disconnect(hdb_connection* con) 
{
	/* Disconnects and frees the context */
	redisFree((redisContext*)con);

}//End of hdb_disconnect method

/* Store a file record in the Hooli database. */
void hdb_store_file(hdb_connection* con, hdb_record* record) 
{
	redisReply *reply;	//An object used to store the reply from Redis

	//Uses the HSET Command with 3 parameters retrieved from the record to set a record on the server
	reply = redisCommand((redisContext*)con, "HSET %s %s %s", record->username, record->filename, record->checksum);

	//Free the reply object
	freeReplyObject(reply);
}//End of hdb_store_file method


/* Remove a file record from the Hooli database. */
int hdb_remove_file(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;	//An object used to store the reply from Redis

	//Uses the HDEL Command to delete the specified record
	reply = redisCommand((redisContext*)con, "HDEL %s %s", username, filename);

	//Create a temp object to store the reply so we can free reply before returning
	int temp = reply->integer;

	//Free the reply object
	freeReplyObject(reply);

	return temp;
}//End of hdb_remove_file method

/* If the specified file is found in the Hooli database, return its checksum. Otherwise, return NULL.*/
char* hdb_file_checksum(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;	//An object used to store the reply from Redis
	
	//Uses the HEXISTS Command to check for an existing record
	reply = redisCommand((redisContext*)con, "HEXISTS %s %s", username, filename);

	//If it doesnt exist exit
	if (reply->integer == 0)
	{
		//Free the reply object
		freeReplyObject(reply);
		return NULL;
	}

	//Free the reply object
	freeReplyObject(reply);

	//If it exists return the checksum using HGET
	reply = redisCommand((redisContext*)con, "HGET %s %s", username, filename);

	//Create a temp object to store the reply so we can free reply before returning
	char* temp = (char*)malloc((strlen(reply->str) + 1) * sizeof(char));
	strcpy(temp, reply->str);

	//Free the reply object
	freeReplyObject(reply);

	return temp;
}//End of hdb_file_checksum method

/* Get the number of files stored in the Hooli database for the specified user. */
int hdb_file_count(hdb_connection* con, const char* username) 
{
	redisReply *reply;	//An object used to store the reply from Redis
	
	reply = redisCommand((redisContext*)con, "HLEN %s", username);

	//Create a temp object to store the reply so we can free reply before returning
	int temp = reply->integer;

	//Free the reply object
	freeReplyObject(reply);

	return temp;
}//End of hdb_file_count method

/* Return a Boolean value indicating whether or not the specified user exists in the Hooli database */
bool hdb_user_exists(hdb_connection* con, const char* username) 
{
	redisReply *reply;	//An object used to store the reply from Redis

	reply = redisCommand((redisContext*)con, "EXISTS %s", username);

	if (reply->integer == 1)
	{
		freeReplyObject(reply);
		return true;
	}

	freeReplyObject(reply);
	return false;
}//End of hdb_user_exists method

/* Return a Boolean value indicating whether or not the specified file exists in the Hooli database */
bool hdb_file_exists(hdb_connection* con, const char* username, const char* filename) 
{
	redisReply *reply;	//An object used to store the reply from Redis

	reply = redisCommand((redisContext*)con, "HEXISTS %s %s", username, filename);

	if (reply->integer == 1)
	{
		freeReplyObject(reply);
		return true;
	}

	freeReplyObject(reply);
	return false;
}//End of hdb_file_exists method

/* Return a linked list of all of the specified user's file records stored in the Hooli database */
hdb_record* hdb_user_files(hdb_connection* con, const char* username)
{
	redisReply *reply;	//An object used to store the reply from Redis
	hdb_record *head = NULL;
	hdb_record *temp;

	//Check for errors (no connection / invalid username)
	if (con == NULL || username == NULL || !hdb_user_exists(con, username))
		return NULL;

	//Connect to the redis server and run the command to get a list of all keys and values
	reply = redisCommand((redisContext*)con, "HGETALL %s", username);

	if (reply->type == REDIS_REPLY_ARRAY) 
	{
		for (int j = 0; j < reply->elements; j+=2)
		{
			//Allocate space for hdb_record 
			temp = (hdb_record*)malloc(sizeof(hdb_record));		

			//Allocate space for fields of hdb_record
			temp->username = (char*)malloc((strlen(username) + 1) * sizeof(char));
			temp->filename = (char*)malloc((strlen(reply->element[j]->str) + 1) * sizeof(char));
			temp->checksum = (char*)malloc((strlen(reply->element[j + 1]->str) + 1) * sizeof(char));

			//Populate fields with data
			strcpy(temp->username, username);
			strcpy(temp->filename, reply->element[j]->str);
			strcpy(temp->checksum, reply->element[j+1]->str);

			//Point the next pointer to the head and assign new object as head
			temp->next = head;
			head = temp;
		}
	}

	//Free the reply object
	freeReplyObject(reply);
	return head;
}//End of hdb_user_files method

/* Free the linked list returned by hdb_user_files() */
void hdb_free_result(hdb_record* record) 
{
	hdb_record *node = record;
	hdb_record *temp;

	while (node != NULL)
	{
		temp = node;
		node = node->next;

		free(temp->username);
		free(temp->filename);
		free(temp->checksum);
		free(temp);
	}

	record = NULL;
}//End of hdb_free_result method

/* Delete the specifid user and all of his/her file records from the Hooli database. */
int hdb_delete_user(hdb_connection* con, const char* username) 
{
	redisReply *reply;	//An object used to store the reply from Redis
	
	reply = redisCommand((redisContext*)con, "DEL %s", username);

	//Create a temp object to store the reply so we can free reply before returning
	int temp = reply->integer;

	//Free the reply object
	freeReplyObject(reply);

	return temp;
}//End of hdb_delete_user method
