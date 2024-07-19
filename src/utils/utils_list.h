#ifndef _UTILS_LIST_H___
#define _UTILS_LIST_H___

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _node /* Node of a linked list. */
{
    struct _node* next;     /* Points at the next node in the list */
    struct _node* previous; /* Points at the previous node in the list */
} NODE;


typedef struct _list /* Header for a linked list. */
{
    NODE node;  /* Header list node */
    int  count; /* Number of nodes in list */
} LIST;


/* function declarations */
NODE* lstFirst(LIST* pList);
NODE* lstGet(LIST* pList);
NODE* lstLast(LIST* pList);
NODE* lstNStep(NODE* pNode, int nStep);
NODE* lstNext(NODE* pNode);
NODE* lstNth(LIST* pList, int nodenum);
NODE* lstPrevious(NODE* pNode);
int   lstCount(LIST* pList);
int   lstFind(LIST* pList, NODE* pNode);
void  lstAdd(LIST* pList, NODE* pNode);
void  lstDelete(LIST* pList, NODE* pNode);
void  lstExtract(LIST* pSrcList, NODE* pStartNode, NODE* pEndNode, LIST* pDstList);
void  lstFree(LIST* pList);
void  lstInit(LIST* pList);
void  lstInsert(LIST* pList, NODE* pPrev, NODE* pNode);

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_LIST_H___ */
