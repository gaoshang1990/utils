#include "lstLib_.h"

#ifndef NULL
#    ifdef __cplusplus
#        define NULL 0
#    else
#        define NULL ((void*)0)
#    endif
#endif

#define HEAD node.next     /* first node in list */
#define TAIL node.previous /* last node in list */


NODE* lstFirst(LIST* pList)
{
    return pList->node.next;
}


NODE* lstGet(LIST* pList)
{
    NODE* pNode = pList->HEAD;

    if (pNode != NULL) {           /* is list empty? */
        pList->HEAD = pNode->next; /* make next node be 1st */

        if (pNode->next == NULL) { /* is there any next node? */
            pList->TAIL = NULL;    /*   no - list is empty */
        }
        else {
            pNode->next->previous = NULL; /*   yes - make it 1st node */
        }
        pList->count--; /* update node count */
    }

    return (pNode);
}


NODE* lstLast(LIST* pList)
{
    NODE* pNode = lstFirst(pList);

    while (pNode != NULL) {
        if (pNode->next == NULL) {
            return pNode;
        }

        pNode = pNode->next;
    }

    return NULL;
}


NODE* lstNStep(NODE* pNode, int nStep)
{
    int   i;
    NODE* pNodeTmp = pNode;

    for (i = 0; i < nStep; i++) {
        pNodeTmp = lstNext(pNodeTmp);
    }

    return pNodeTmp;
}


NODE* lstNext(NODE* pNode)
{
    if (pNode == NULL) {
        return NULL;
    }

    return pNode->next;
}


/************************************************************************
*
* lstNth - find the Nth node in a list
*
* This routine returns a pointer to the node specified by a number <nodenum>
* where the first node in the list is numbered 1.
* Note that the search is optimized by searching forward from the beginning
* if the node is closer to the head, and searching back from the end
* if it is closer to the tail.
*
* RETURNS:
* A pointer to the Nth node, or
* NULL if there is no Nth node.
*/
NODE* lstNth(LIST* pList, int nodenum)
{
    return lstNStep(pList->node.next, nodenum);
}


NODE* lstPrevious(NODE* pNode)
{
    if (pNode == NULL) {
        return NULL;
    }

    return pNode->previous;
}


int lstCount(LIST* pList)
{
    if (pList == NULL) {
        return 0;
    }

    return pList->count;
}


int lstFind(LIST* pList, NODE* pNode)
{
    NODE* pHead = lstFirst(pList);

    while (pHead) {
        if (pHead == pNode) {
            return 1;
        }
        pHead = pHead->next;
    }

    return 0;
}


void lstAdd(LIST* pList, NODE* pNode)
{
    NODE* pLast = lstLast(pList);

    if (pLast == NULL) {
        pList->node.next = pNode;

        pNode->next     = NULL;
        pNode->previous = NULL;
    }
    else {
        pLast->next     = pNode;
        pNode->next     = NULL;
        pNode->previous = pLast;
    }

    pList->count++;
}


/************************************************************************
 *
 * lstExtract - extract a sublist from a list
 *
 * This routine extracts the sublist that starts with <pStartNode> and ends
 * with <pEndNode> from a source list.  It places the extracted list in
 * <pDstList>.
 *
 * RETURNS: N/A
 */

void lstExtract(LIST* pSrcList, NODE* pStartNode, NODE* pEndNode, LIST* pDstList)
{
    int   i;
    NODE* pNode;

    /* fix pointers in original list */

    if (pStartNode->previous == NULL) {
        pSrcList->HEAD = pEndNode->next;
    }
    else {
        pStartNode->previous->next = pEndNode->next;
    }
    if (pEndNode->next == NULL) {
        pSrcList->TAIL = pStartNode->previous;
    }
    else {
        pEndNode->next->previous = pStartNode->previous;
    }

    /* fix pointers in extracted list */

    pDstList->HEAD = pStartNode;
    pDstList->TAIL = pEndNode;

    pStartNode->previous = NULL;
    pEndNode->next       = NULL;

    /* count number of nodes in extracted list and update counts in lists */

    i = 0;
    for (pNode = pStartNode; pNode != NULL; pNode = pNode->next) {
        i++;
    }
    pSrcList->count -= i;
    pDstList->count = i;
}


void lstDelete(LIST* pList, NODE* pNode)
{
    int bFind = lstFind(pList, pNode);
    if (bFind) {
        if (pNode->previous == NULL && pNode->next == NULL) {
            pList->count         = 0;
            pList->node.next     = NULL;
            pList->node.previous = NULL;
            return;
        }

        if (pNode->previous) {
            pNode->previous->next = pNode->next;
        }
        if (pNode->next) {
            pNode->next->previous = pNode->previous;
        }
        if (pList->node.next == pNode) {
            pList->node.next = pNode->next;
        }

        pList->count--;
    }
}


void lstFree(LIST* pList)
{
    pList->count         = 0;
    pList->node.next     = NULL;
    pList->node.previous = NULL;
}


void lstInit(LIST* pList)
{
    pList->count         = 0;
    pList->node.next     = NULL;
    pList->node.previous = NULL;
}

/************************************************************************
 *
 * lstInsert - insert a node in a list after a specified node
 *
 * This routine inserts a specified node in a specified list.
 * The new node is placed following the list node <pPrev>.
 * If <pPrev> is NULL, the node is inserted at the head of the list.
 *
 * RETURNS: N/A
 */

void lstInsert(LIST* pList, /* pointer to list descriptor */
               NODE* pPrev, /* pointer to node after which to insert */
               NODE* pNode  /* pointer to node to be inserted */
)
{
    NODE* pNext;

    if (pPrev == NULL) { /* new node is to be first in list */
        pNext       = pList->HEAD;
        pList->HEAD = pNode;
    }
    else { /* make prev node point fwd to new */
        pNext       = pPrev->next;
        pPrev->next = pNode;
    }

    if (pNext == NULL)
        pList->TAIL = pNode; /* new node is to be last in list */
    else
        pNext->previous = pNode; /* make next node point back to new */


    /* set pointers in new node, and update node count */

    pNode->next     = pNext;
    pNode->previous = pPrev;

    pList->count++;
}
