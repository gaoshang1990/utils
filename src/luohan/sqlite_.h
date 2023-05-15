#ifndef _USER_SQLITE_H_
#define _USER_SQLITE_H_

#include "hal_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Semaphore g_sqlLock;


extern int sqlInit_(void);
extern int sqlDestroy_(void);
extern int sqlDeleteTable_(char* database, char* table);
extern int sqlSetRecordById_(char* database, char* table, char* colName, char* element, int id);
extern int sqlGetRecordById_(char* database, char* table, char* colName, int id, char* record);


#ifdef __cplusplus
}
#endif

#endif /* _USER_SQLITE_H_ */