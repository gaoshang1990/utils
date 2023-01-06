#if (UCL_USE_SQLITE == 1)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sqlite3.h"

#include "ucl_sqlite.h"
#include "file_.h"


//#ifdef _WIN32
//#    pragma comment(lib, "sqlite3.lib")
//#endif


Semaphore g_sqlLock;


/* 初始化互斥锁 */
int sqlInit_(void)
{
    g_sqlLock = Semaphore_create(1);
    return 0;
}


int sqlDestroy_(void)
{
    Semaphore_destroy(g_sqlLock);
    return 0;
}


int Ucl_sqlCreateTable(char* database, char* tbInfo)
{
    sqlite3* db;

    int rc = sqlite3_open(database, &db);
    if (rc) {
        printf("create_talbe: Can't open database: %s!\n", database);
        return -1;
    }

    rc = sqlite3_exec(db, tbInfo, NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        printf("create_talbe: SQL error!!!\n");
    }

    sqlite3_close(db);
    return 0;
}


/* 返回值：有记录则返回所在行号 */
int Ucl_sqlIsRecord(const char* database, const char* table, const char* colName, char* element)
{
    Semaphore_wait(g_sqlLock);

    sqlite3* db = NULL;
    if (sqlite3_open(database, &db) != SQLITE_OK) {
		printf("Ucl_sqlIsRecord: Can't open database: %s!\n", database);
		return -1;
	}

	char* sql = sqlite3_mprintf("SELECT %s FROM %s WHERE %s = %s;", colName, table, colName, element);
	if (sql == NULL) {
		return -2;
	}

	int    row;
	char** res;
	sqlite3_get_table(db, sql, &res, &row, NULL, NULL); /* 不传res变量会segmentation fault */

	sqlite3_free(sql);
	sqlite3_free_table(res);
	sqlite3_close(db);

    Semaphore_post(g_sqlLock);

	return row ? row : 0;
}


int sqlGetRecordById_(char* database, char* table, char* colName, int id, char* record)
{
    if (id < 0) {
        printf("SQL_getRecord: Sqlite3 error: id < 0!\n");
        return -1;
    }
    if (record == NULL) {
        printf("SQL_getRecord: Sqlite3 error: record == NULL!\n");
    }

    char szId[32] = {0};
    sprintf(szId, "%d", id);
	if (Ucl_sqlIsRecord(database, table, "id", szId) <= 0) {
		return -2;
	}

    Semaphore_wait(g_sqlLock);

    sqlite3* db  = NULL;
    if (sqlite3_open(database, &db) != SQLITE_OK) {
        return -3;
    }

    char*  sql = sqlite3_mprintf("SELECT %s FROM %s WHERE id = %d;", colName, table, id);
    char** res = NULL;
    sqlite3_get_table(db, sql, &res, NULL, NULL, NULL);
    if (res == NULL) {
        printf("There's not the column: '%s'\n!", colName);
        return -4;
    }
    if (res[1] != NULL) {
        strcpy(record, res[1]);
    }

    sqlite3_free(sql);
    sqlite3_free_table(res);
    sqlite3_close(db);

    pthread_mutex_unlock(&g_sqlLock);
    Semaphore_post(g_sqlLock);

    return 0;
}


uint8_t Ucl_sqlIsTableExist(char* database, char* table)
{
    uint8_t ret = 0;

    Semaphore_wait(g_sqlLock);

    sqlite3* db = NULL;
    if (sqlite3_open(database, &db) != SQLITE_OK) {
        printf("SQL_setRecord: can't open database %s\n", database);
        return 0;
    }

    char* sql = sqlite3_mprintf("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='%s';", table);
    char** res = NULL;
    if (sqlite3_get_table(db, sql, &res, NULL, NULL, NULL) != SQLITE_OK) {
        printf("SQL_setRecord failed!\n");
    }
    if (res == NULL) {
        return 0;
    }
    if (res[1] != NULL) {
        ret = (uint8_t)atoi(res[1]);
    }

    sqlite3_free(sql);
    sqlite3_close(db);
    sqlite3_free_table(res);

    Semaphore_post(g_sqlLock);

    return ret;
}


int sqlSetRecordById_(char* database, char* table, char* colName, char* element, int id)
{
    if (!Ucl_isFileExist(database)) {
        char tbInfo[512] = {0};
        sprintf(tbInfo, "CREATE TABLE %s(%s);", table, colName);
        printf("SQL_setRecord: Create table: %s\n", tbInfo);
        Ucl_sqlCreateTable(database, tbInfo);
    }
    if(!Ucl_sqlIsTableExist(database, table)) {
        char tbInfo[512] = {0};
        sprintf(tbInfo, "CREATE TABLE %s(%s);", table, colName);
        printf("SQL_setRecord: Create table: %s\n", tbInfo);
        Ucl_sqlCreateTable(database, tbInfo);
    }

    char  szId[32] = { 0 };
	sprintf(szId, "%d", id);
	uint8_t bRecord = Ucl_sqlIsRecord(database, table, "id", szId);

    Semaphore_wait(g_sqlLock);

    sqlite3* db = NULL;
    if (sqlite3_open(database, &db) != SQLITE_OK) {
        printf("SQL_setRecord: can't open database %s\n", database);
        return -1;
    }

    char* sql = NULL;
    if (bRecord > 0) { /* 更新数据 */
        sql = sqlite3_mprintf("UPDATE %s SET (%s) = (%s) WHERE id = %d;", table, colName, element, id);
    }
    else { /* 插入数据 */
        sql = sqlite3_mprintf("INSERT INTO %s (%s) VALUES (%s);", table, colName, element);
    }

    if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
        printf("SQL_setRecord failed!\n");
    }

    sqlite3_free(sql);
    sqlite3_close(db);

    Semaphore_post(g_sqlLock);
    
    return 0;
}


int sqlDeleteTable_(char* database, char* table)
{
    if (!Ucl_sqlIsTableExist(database, table)) {
        return 0;
    }

    Semaphore_wait(g_sqlLock);

    sqlite3* db = NULL;
    if (sqlite3_open(database, &db) != SQLITE_OK) {
        printf("SQL_setRecord: can't open database %s\n", database);
        return -1;
    }

    char* sql = sqlite3_mprintf("DROP TABLE %s;", table);
    if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
        printf("SQL_setRecord failed!\n");
    }

    sqlite3_free(sql);
    sqlite3_close(db);

    Semaphore_post(g_sqlLock);

    return 0;
}

#endif