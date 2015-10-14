/*****************************************************************************
 * testlibhdb.c
 *
 * Computer Science 3357a - Fall 2015
 * Author: Jeff Shantz
 *
 * Set of unit tests to test `libhdb`.
 * Build with `make test`
 ****************************************************************************/

#include <assert.h>
#include <check.h>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdb.h"

/*****************************************************************************
 * Test data
 ****************************************************************************/

#define USER1 "henry"
#define USER2 "abby"

#define FILE1 "etc/redis/redis.conf"
#define FILE2 "var/run/crond.pid"
#define FILE3 "bin/bash"
#define FILE4 "usr/share/man/man1/test.1.gz"

#define CKSUM1 "8BADF00D"
#define CKSUM2 "DEADBEEF"
#define CKSUM3 "FACEB00C"
#define CKSUM4 "CAFED00D"

#define REMOVE_SUCCESS 1
#define REMOVE_FAILURE 0

// ZOMG -- the instructor gave us teh codez that use a GLOBAL VARIABLE!!!!1111oneone
// 
// Don't do this in real code.  This is just to allow the setup() and
// teardown() functions to initialize / free the Redis server context,
// while allowing the test functions to access that context.
hdb_connection* con; 

// Run before each test
void setup(void)
{
  con = hdb_connect("127.0.0.1");

  // Execute these directly against the Redis server to prevent
  // crashed tests from leaving state on the server that might
  // cause other tests to fail.
  redisReply* reply = redisCommand((redisContext*)con,"FLUSHALL");
  freeReplyObject(reply);
}

// Run after each test
void teardown(void)
{
  hdb_disconnect(con);
}

// Helper function -- adds a file to HooliDB.
void add_file(char* username, char* filename, char* checksum) {
  hdb_record rec = {
    .username = username,
    .filename = filename,
    .checksum = checksum
  };

  hdb_store_file(con, &rec);
}

/*****************************************************************************
 * hdb_store_file()
 ****************************************************************************/

// Exercised indirectly

/*****************************************************************************
 * hdb_remove_file()
 ****************************************************************************/

START_TEST(test_removing_file_returns_1)
{
  add_file(USER1, FILE1, CKSUM1);
  int retval = hdb_remove_file(con, USER1, FILE1);

  ck_assert_msg(retval == REMOVE_SUCCESS,
    "Expected hdb_remove_file() to return %d after removing existing file, "
    "but found %d", REMOVE_SUCCESS, retval);
}
END_TEST

START_TEST(test_removing_nonexistent_file_returns_0)
{
  int retval = hdb_remove_file(con, USER1, FILE1);

  ck_assert_msg(retval == REMOVE_FAILURE,
    "Expected hdb_remove_file() to return %d after removing non-existent "
    "file, but found %d", REMOVE_FAILURE, retval);
}
END_TEST

/*****************************************************************************
 * hdb_file_checksum()
 ****************************************************************************/

START_TEST(test_correct_checksum_returned_for_file)
{
  add_file(USER1, FILE1, CKSUM1);
  char* result = hdb_file_checksum(con, USER1, FILE1);

  if (strncmp(result, CKSUM1, strlen(CKSUM1))) {
    ck_abort_msg("Expected hdb_file_checksum() to return %s for user %s, "
      "file %s, but found %s", CKSUM1, USER1, FILE1, result);
  }

  free(result);
}
END_TEST

START_TEST(test_null_checksum_returned_for_nonexistent_file)
{
  add_file(USER1, FILE1, CKSUM1);
  char* result = hdb_file_checksum(con, USER1, FILE2);

  ck_assert_msg(result == NULL,
    "Expected hdb_file_checksum() to return NULL for non-existent file, but "
    "found %s", result);
}
END_TEST

START_TEST(test_correct_checksum_returned_when_file_updated)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER1, FILE1, CKSUM2);
  char* result = hdb_file_checksum(con, USER1, FILE1);

  if (strncmp(result, CKSUM2, strlen(CKSUM2))) {
    ck_abort_msg("Expected hdb_file_checksum() to return %s for user %s, "
      "file %s, but found %s", CKSUM2, USER1, FILE1, result);
  }

  free(result);
}
END_TEST

/*****************************************************************************
 * hdb_file_count()
 ****************************************************************************/

START_TEST(test_file_count_is_zero_for_nonexistent_user)
{
  int count = hdb_file_count(con, USER1);

  ck_assert_msg(count == 0,
    "Expected hdb_file_count() to return 0 for non-existent user, but found "
    "%d", count);
}
END_TEST

START_TEST(test_adding_file_increases_file_count)
{
  add_file(USER1, FILE1, "A0B1C2D3");
  int count = hdb_file_count(con, USER1);

  ck_assert_msg(count == 1,
    "Expected hdb_file_count() to return 1 for a user with one file added, "
    "but found %d", count);
}
END_TEST

START_TEST(test_updating_file_does_not_change_users_file_count)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER1, FILE1, CKSUM2);
  int count = hdb_file_count(con, USER1);

  ck_assert_msg(count == 1,
    "Expected hdb_file_count() to return 1 when a single file is updated, "
    "but found %d", count);
}
END_TEST

START_TEST(test_adding_file_does_not_change_other_users_file_count)
{
  add_file(USER1, FILE1, "A0B1C2D3");
  int count = hdb_file_count(con, USER2);

  ck_assert_msg(count == 0,
    "Expected hdb_file_count() to return 0 for %s when a file is added for "
    "%s, but found %d", USER2, USER1, count);
}
END_TEST

START_TEST(test_removing_file_decrements_users_file_count)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_remove_file(con, USER1, FILE1);
  int count = hdb_file_count(con, USER1);

  ck_assert_msg(count == 0,
    "Expected hdb_file_count() to return 0 when a user's only file is "
    "removed, but found %d", count);
}
END_TEST

START_TEST(test_removing_nonexistent_file_does_not_decrement_users_file_count)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_remove_file(con, USER1, FILE2);
  int count = hdb_file_count(con, USER1);

  ck_assert_msg(count == 1,
    "Expected hdb_file_count() not to change when attempting to delete a "
    "non-existent file for a user, but found %d", count);
}
END_TEST

START_TEST(test_removing_file_does_not_change_other_users_file_count)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER2, FILE2, CKSUM2);

  hdb_remove_file(con, USER1, FILE1);
  int count = hdb_file_count(con, USER2);

  ck_assert_msg(count == 1,
    "Expected hdb_file_count() not to change for %s after deleting a file "
    "for %s, but found %d", USER2, USER1, count);
}
END_TEST

START_TEST(test_file_count_is_zero_for_deleted_user)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_delete_user(con, USER1);
  int count = hdb_file_count(con, USER1);

  ck_assert_msg(count == 0,
    "Expected hdb_file_count() to return 0 for a deleted user, but found %d",
    count);
}
END_TEST

/*****************************************************************************
 * hdb_user_exists()
 ****************************************************************************/

START_TEST(test_user_does_not_exist_before_files_have_been_added)
{
  ck_assert_msg(! hdb_user_exists(con, USER1),
    "Expected hdb_user_exists() to return false for a non-existent user, but "
    "found true");
}
END_TEST

START_TEST(test_user_exists_after_file_added)
{
  add_file(USER1, FILE1, CKSUM1);

  ck_assert_msg(hdb_user_exists(con, USER1),
    "Expected hdb_user_exists() to return true after adding a user, but "
    "found false");
}
END_TEST

START_TEST(test_user2_does_not_exist_after_file_added_for_user1)
{
  add_file(USER1, FILE1, CKSUM1);

  ck_assert_msg(! hdb_user_exists(con, USER2),
    "Expected hdb_user_exists() to return false for %s after %s is added, "
    "but found true", USER2, USER1);
}
END_TEST

START_TEST(test_user_still_exists_after_one_file_removed)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER1, FILE2, CKSUM2);

  hdb_remove_file(con, USER1, FILE1);

  ck_assert_msg(hdb_user_exists(con, USER1),
    "Expected hdb_user_exists() to return true when one (but not all) of a"
    "user's files is removed, but found false");
}
END_TEST

START_TEST(test_user_does_not_exist_after_all_files_removed)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER1, FILE2, CKSUM2);

  hdb_remove_file(con, USER1, FILE1);
  hdb_remove_file(con, USER1, FILE2);

  ck_assert_msg(! hdb_user_exists(con, USER1),
    "Expected hdb_user_exists() to return false when all of a user's files "
    "are removed, but found true");
}
END_TEST

START_TEST(test_user_does_not_exist_after_user_is_deleted)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_delete_user(con, USER1);

  ck_assert_msg(! hdb_user_exists(con, USER1),
    "Expected hdb_user_exists() to return false when a user is deleted, but "
    "found true");
}
END_TEST

/*****************************************************************************
 * hdb_file_exists()
 ****************************************************************************/

START_TEST(test_file_does_not_exist_before_being_added)
{
  add_file(USER1, FILE1, CKSUM1);

  ck_assert_msg(! hdb_file_exists(con, USER1, FILE2),
    "Expected hdb_file_exists() to return false for non-existent file, but "
    "found true");
}
END_TEST

START_TEST(test_file_exists_after_being_added)
{
  add_file(USER1, FILE1, CKSUM1);

  ck_assert_msg(hdb_file_exists(con, USER1, FILE1),
    "Expected hdb_file_exists() to return true for existing file, but found "
    "false");
}
END_TEST

START_TEST(test_file_does_not_exist_after_being_removed)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_remove_file(con, USER1, FILE1);

  ck_assert_msg(! hdb_file_exists(con, USER1, FILE1),
    "Expected hdb_file_exists() to return false after file deleted, but "
    "found true");
}
END_TEST

START_TEST(test_file_does_not_exist_after_user_deleted)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_delete_user(con, USER1);

  ck_assert_msg(! hdb_file_exists(con, USER1, FILE1),
    "Expected hdb_file_exists() to return false after user deleted, but "
    "found true");
}
END_TEST

START_TEST(test_file_does_not_exist_for_other_user)
{
  add_file(USER1, FILE1, CKSUM1);

  ck_assert_msg(! hdb_file_exists(con, USER2, FILE1),
    "Expected hdb_file_exists() to return false for %s after file added for "
    "%s, but found true", USER2, USER1);
}
END_TEST

/*****************************************************************************
 * hdb_user_files()
 ****************************************************************************/

START_TEST(test_user_file_list_is_null_when_user_does_not_exist)
{
  hdb_record* list = hdb_user_files(con, USER1);

  ck_assert_msg(list == NULL,
    "Expected hdb_user_files() to return NULL for non-existent user, "
    "but found non-NULL value");
}
END_TEST

START_TEST(test_user_file_list_is_null_when_all_users_files_have_been_deleted)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_remove_file(con, USER1, FILE1);
  hdb_record* list = hdb_user_files(con, USER1);

  ck_assert_msg(list == NULL,
    "Expected hdb_user_files() to return NULL when all of a user's "
    "files have been deleted, but found non-NULL value");
}
END_TEST

START_TEST(test_user_file_list_is_null_when_user_deleted)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_delete_user(con, USER1);
  hdb_record* list = hdb_user_files(con, USER1);

  ck_assert_msg(list == NULL,
    "Expected hdb_user_files() to return NULL when a user is deleted, "
    "but found non-NULL value");
}
END_TEST

START_TEST(test_user_file_list_contains_one_record_when_one_file_exists)
{
  add_file(USER1, FILE1, CKSUM1);
  hdb_record* record = hdb_user_files(con, USER1);

  ck_assert_msg(record != NULL,
    "Expected hdb_user_files() to return non-NULL list when a user "
    "has a file, but found NULL value");

  ck_assert_str_eq(record->username, USER1);
  ck_assert_str_eq(record->filename, FILE1);
  ck_assert_str_eq(record->checksum, CKSUM1);

  ck_assert_msg(record->next == NULL,
    "Expected hdb_user_files() to set last record's 'next' pointer to "
    "NULL, but found non-NULL value");

  hdb_free_result(record);
}
END_TEST

START_TEST(test_user_file_list_contains_correct_records_when_multiple_files_exist)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER1, FILE2, CKSUM2);
  hdb_record* record = hdb_user_files(con, USER1);

  ck_assert_msg(record != NULL,
    "Expected hdb_user_files() to return non-NULL list when a user "
    "has two files, but found NULL value");

  ck_assert_msg(record->next != NULL,
    "Expected hdb_user_files() to return two records when a user "
    "has two files, but found only one");

  ck_assert_msg(record->next->next == NULL,
    "Expected hdb_user_files() to return only two records when a user "
    "has two files, but found more than two");

  hdb_record* file1;
  hdb_record* file2;

  // Can't guarantee that Redis will return results in order
  if (strncmp(record->filename, FILE1, 3) == 0) {
    file1 = record;
    file2 = record->next;
  }
  else {
    file2 = record;
    file1 = record->next;
  }

  ck_assert_str_eq(file1->username, USER1);
  ck_assert_str_eq(file1->filename, FILE1);
  ck_assert_str_eq(file1->checksum, CKSUM1);

  ck_assert_str_eq(file2->username, USER1);
  ck_assert_str_eq(file2->filename, FILE2);
  ck_assert_str_eq(file2->checksum, CKSUM2);

  hdb_free_result(record);
}
END_TEST

START_TEST(test_user_file_list_contains_correct_records_when_files_have_been_deleted)
{
  add_file(USER1, FILE1, CKSUM1);
  add_file(USER1, FILE2, CKSUM2);
  add_file(USER1, FILE3, CKSUM3);
  add_file(USER1, FILE4, CKSUM4);

  hdb_remove_file(con, USER1, FILE3);

  hdb_record* record = hdb_user_files(con, USER1);

  ck_assert_msg(record != NULL,
    "Expected hdb_user_files() to return non-NULL list when a user "
    "has three files, but found NULL value");

  ck_assert_msg(record->next != NULL,
    "Expected hdb_user_files() to return three records when a user "
    "has three files, but found only one");

  ck_assert_msg(record->next->next != NULL,
    "Expected hdb_user_files() to return three records when a user "
    "has three files, but found only two");

  ck_assert_msg(record->next->next->next == NULL,
    "Expected hdb_user_files() to return only three records when a "
    "user has three files, but found more than three");

  hdb_record* cur = record;

  // Can't guarantee that Redis will return results in order
  while (cur != NULL) {
    if (strncmp(cur->filename, FILE1, 3) == 0) {
      ck_assert_str_eq(cur->username, USER1);
      ck_assert_str_eq(cur->filename, FILE1);
      ck_assert_str_eq(cur->checksum, CKSUM1);
    }
    else if (strncmp(cur->filename, FILE2, 3) == 0) {
      ck_assert_str_eq(cur->username, USER1);
      ck_assert_str_eq(cur->filename, FILE2);
      ck_assert_str_eq(cur->checksum, CKSUM2);
    }
    else {
      ck_assert_str_eq(cur->username, USER1);
      ck_assert_str_eq(cur->filename, FILE4);
      ck_assert_str_eq(cur->checksum, CKSUM4);
    }

    cur = cur->next;
  }

  hdb_free_result(record);
}
END_TEST

/*****************************************************************************
 * hdb_free_result
 ****************************************************************************/

// Exercised indirectly

/*****************************************************************************
 * hdb_delete_user
 ****************************************************************************/

// Exercised indirectly

Suite* suite() {
  Suite* s;
  TCase* tc_core;
             
  s = suite_create("libhoolidb");
  tc_core = tcase_create("All tests");
                       
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_removing_file_returns_1);
  tcase_add_test(tc_core, test_removing_nonexistent_file_returns_0);
  tcase_add_test(tc_core, test_correct_checksum_returned_for_file);
  tcase_add_test(tc_core, test_null_checksum_returned_for_nonexistent_file);
  tcase_add_test(tc_core, test_correct_checksum_returned_when_file_updated);
  tcase_add_test(tc_core, test_file_count_is_zero_for_nonexistent_user);
  tcase_add_test(tc_core, test_file_count_is_zero_for_deleted_user);
  tcase_add_test(tc_core, test_adding_file_increases_file_count);
  tcase_add_test(tc_core, test_updating_file_does_not_change_users_file_count);
  tcase_add_test(tc_core, test_adding_file_does_not_change_other_users_file_count);
  tcase_add_test(tc_core, test_removing_file_decrements_users_file_count);
  tcase_add_test(tc_core, test_removing_nonexistent_file_does_not_decrement_users_file_count);
  tcase_add_test(tc_core, test_removing_file_does_not_change_other_users_file_count);
  tcase_add_test(tc_core, test_user_does_not_exist_before_files_have_been_added);
  tcase_add_test(tc_core, test_user_exists_after_file_added);
  tcase_add_test(tc_core, test_user2_does_not_exist_after_file_added_for_user1);
  tcase_add_test(tc_core, test_user_still_exists_after_one_file_removed);
  tcase_add_test(tc_core, test_user_does_not_exist_after_all_files_removed);
  tcase_add_test(tc_core, test_user_does_not_exist_after_user_is_deleted);
  tcase_add_test(tc_core, test_file_does_not_exist_before_being_added);
  tcase_add_test(tc_core, test_file_exists_after_being_added);
  tcase_add_test(tc_core, test_file_does_not_exist_after_being_removed);
  tcase_add_test(tc_core, test_file_does_not_exist_after_user_deleted);
  tcase_add_test(tc_core, test_file_does_not_exist_for_other_user);
  tcase_add_test(tc_core, test_user_file_list_is_null_when_user_does_not_exist);
  tcase_add_test(tc_core, test_user_file_list_is_null_when_all_users_files_have_been_deleted);
  tcase_add_test(tc_core, test_user_file_list_is_null_when_user_deleted);
  tcase_add_test(tc_core, test_user_file_list_contains_one_record_when_one_file_exists);
  tcase_add_test(tc_core, test_user_file_list_contains_correct_records_when_multiple_files_exist);
  tcase_add_test(tc_core, test_user_file_list_contains_correct_records_when_files_have_been_deleted);

  suite_add_tcase(s, tc_core);
  return s;
}

int main() {
  int number_failed;
  Suite* s;
  SRunner* sr;
 
  s = suite();
  sr = srunner_create(s);
 
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
