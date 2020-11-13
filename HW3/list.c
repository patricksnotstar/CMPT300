#include "list.h"
#include <stdio.h>
#include <stdlib.h>

List listArray[LIST_MAX_NUM_HEADS];
Node nodeArray[LIST_MAX_NUM_NODES];

static int initialized = 0;

// To establish boundary on the node array

// Keeps track of first node
static Node *firstFreeNode = NULL;

static void linkTheNodes()
{
    firstFreeNode = nodeArray;

    for (int i = 1; i < LIST_MAX_NUM_NODES; i++)
    {
        nodeArray[i - 1].next = &nodeArray[i];
    }

    initialized++;
}

// To establish the boundary of the list array

// Keeps track of the first list
static List *firstFreeList = NULL;

static void linkTheLists()
{
    firstFreeList = listArray;

    for (int i = 1; i < LIST_MAX_NUM_HEADS; i++)
    {
        listArray[i - 1].next = &listArray[i];
    }

    initialized++;
}

static Node *createNode(List *pList, void *pItem)
{
    // Claim a new node
    Node *node;
    node = firstFreeNode;
    firstFreeNode = firstFreeNode->next;

    node->item = pItem;
    pList->numNodes++;

    return node;
}

static void addNodeToEmptyList(List *pList, Node *node)
{
    pList->head = node;
    pList->tail = node;
    node->next = NULL;
    node->prev = NULL;
    pList->current = node;
    pList->position = LIST_NOOB;
}

static void freeNode(Node *node)
{
    node->prev = NULL;
    node->item = NULL;
    node->next = firstFreeNode;
    firstFreeNode = node;
}

static void freeHead(List *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->numNodes = 0;
    list->position = LIST_OOB_START;
    list->next = firstFreeList;
    firstFreeList = list;
}

List *List_create()
{

    if (initialized < 2)
    {
        linkTheLists();
        linkTheNodes();
    }

    // If there are no more free lists
    if (firstFreeList == NULL)
    {
        return NULL;
    }

    List *list;
    list = firstFreeList;
    firstFreeList = firstFreeList->next;

    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->numNodes = 0;
    list->position = LIST_OOB_START;
    return list;
}

int List_add(List *pList, void *pItem)
{
    // If there are no more free nodes
    if (firstFreeNode == NULL)
    {
        return -1;
    }

    Node *node = createNode(pList, pItem);

    // If the list if empty
    if (pList->head == NULL)
    {
        addNodeToEmptyList(pList, node);
        return 0;
    }

    if (pList->position == LIST_NOOB)
    {

        if (pList->current == pList->tail)
        {
            // If current is the tail
            pList->tail->next = node;
            node->prev = pList->tail;
            node->next = NULL;
            pList->tail = node;
            pList->current = node;
            return 0;
        }
        node->next = pList->current->next->prev;
        pList->current->next->prev = node;
        node->prev = pList->current;
        pList->current->next = node;
        pList->current = node;
        return 0;
    }
    else if (pList->position == LIST_OOB_START)
    {
        pList->head->prev = node;
        node->next = pList->head;
        pList->head = node;
        node->prev = NULL;
        pList->current = node;
        return 0;
    }
    else if (pList->position == LIST_OOB_END)
    {
        pList->tail->next = node;
        node->prev = pList->tail;
        pList->tail = node;
        node->next = NULL;
        pList->current = node;
        return 0;
    }
    pList->position = LIST_NOOB;
    return 0;
}

int List_count(List *pList)
{
    return pList->numNodes;
}

void *List_first(List *pList)
{
    if (pList->head == NULL)
    {
        pList->current = NULL;
        pList->position = LIST_OOB_START;
        return NULL;
    }

    pList->position = LIST_NOOB;
    pList->current = pList->head;
    return pList->head->item;
}

void *List_last(List *pList)
{
    if (pList->head == NULL)
    {
        pList->current = NULL;
        pList->position = LIST_OOB_START;
        return NULL;
    }

    pList->position = LIST_NOOB;
    pList->current = pList->tail;
    return pList->tail->item;
}

void *List_next(List *pList)
{
    if (pList->head == NULL)
    {
        return NULL;
    }
    if (pList->position == LIST_OOB_START)
    {
        pList->current = pList->head;
        pList->position = LIST_NOOB;
        return pList->head->item;
    }

    if (pList->position == LIST_OOB_END)
    {
        return NULL;
    }

    if (pList->current == pList->tail)
    {
        pList->position = LIST_OOB_END;
        return NULL;
    }
    pList->current = pList->current->next;
    return pList->current->item;
}

void *List_prev(List *pList)
{
    if (pList->head == NULL)
    {
        return NULL;
    }
    if (pList->current == pList->head)
    {
        pList->position = LIST_OOB_START;
        return NULL;
    }

    if (pList->position == LIST_OOB_END)
    {
        pList->current = pList->tail;
        pList->position = LIST_NOOB;
        return pList->tail->item;
    }

    if (pList->position == LIST_OOB_START)
    {
        return NULL;
    }

    pList->current = pList->current->prev;
    return pList->current->item;
}

void *List_curr(List *pList)
{
    if (pList->position == LIST_OOB_START || pList->position == LIST_OOB_END)
    {
        return NULL;
    }
    else if (pList->current != NULL)
    {
        return pList->current->item;
    }
    return NULL;
}

int List_insert(List *pList, void *pItem)
{
    // If there are no more free nodes
    if (firstFreeNode == NULL)
    {
        return -1;
    }

    Node *node = createNode(pList, pItem);

    // If the list if empty
    if (pList->head == NULL)
    {
        addNodeToEmptyList(pList, node);
        return 0;
    }

    if (pList->position == LIST_OOB_START)
    {
        pList->head->prev = node;
        node->next = pList->head;
        pList->head = node;
        node->prev = NULL;
        pList->position = LIST_NOOB;
        pList->current = node;
        return 0;
    }
    else if (pList->position == LIST_OOB_END)
    {
        pList->tail->next = node;
        node->prev = pList->tail;
        pList->tail = node;
        node->next = NULL;
        pList->position = LIST_NOOB;
        pList->current = node;
        return 0;
    }
    else if (pList->position == LIST_NOOB)
    {
        if (pList->current == pList->head)
        {
            node->prev = NULL;
            node->next = pList->head;
            pList->head->prev = node;
            pList->head = node;
            pList->current = node;
            return 0;
        }
        pList->current->prev->next = node;
        node->prev = pList->current->prev;
        node->next = pList->current;
        pList->current->prev = node;
        pList->current = node;
    }
    pList->position = LIST_NOOB;
    return 0;
}

int List_append(List *pList, void *pItem)
{
    // If there are no more free nodes
    if (firstFreeNode == NULL)
    {
        return -1;
    }

    Node *node = createNode(pList, pItem);

    if (pList->head == NULL)
    {
        addNodeToEmptyList(pList, node);
        return 0;
    }

    pList->tail->next = node;
    node->prev = pList->tail;
    node->next = NULL;
    pList->tail = node;
    pList->current = node;
    pList->position = LIST_NOOB;
    return 0;
}

int List_prepend(List *pList, void *pItem)
{
    // If there are no more free nodes
    if (firstFreeNode == NULL)
    {
        return -1;
    }

    Node *node = createNode(pList, pItem);

    if (pList->head == NULL)
    {
        addNodeToEmptyList(pList, node);
        return 0;
    }

    pList->head->prev = node;
    node->prev = NULL;
    node->next = pList->head;
    pList->head = node;
    pList->current = node;
    pList->position = LIST_NOOB;
    return 0;
}

void *List_remove(List *pList)
{
    if (pList->head == NULL)
    {
        pList->position = LIST_OOB_START;
        return NULL;
    }

    if (pList->position == LIST_OOB_START || pList->position == LIST_OOB_END)
    {
        return NULL;
    }

    Node *node = pList->current;
    void *item = pList->current->item;

    if (pList->position == LIST_OOB_END || pList->position == LIST_OOB_START)
    {
        return NULL;
    }
    else if (pList->numNodes == 1)
    {
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL;
        pList->position = LIST_OOB_START;
    }
    else if (pList->current == pList->head)
    {
        pList->head->next->prev = NULL;
        pList->head = pList->head->next;
        pList->current = pList->head;
    }
    else if (pList->current == pList->tail)
    {
        pList->tail->prev->next = NULL;
        pList->tail = pList->tail->prev;
        pList->current = NULL;
        pList->position = LIST_OOB_END;
    }
    else
    {
        pList->current->prev->next = pList->current->next;
        pList->current->next->prev = pList->current->prev;
        pList->current = pList->current->next;
    }

    freeNode(node);
    pList->numNodes--;
    return item;
}

void List_concat(List *pList1, List *pList2)
{
    if (pList1->head == NULL)
    {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList1->current = pList2->current;
        pList1->numNodes = pList2->numNodes;
        pList1->position = LIST_NOOB;
    }
    else if (pList2->head == NULL)
    {
        freeHead(pList2);
        return;
    }
    else
    {
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;
        pList1->numNodes += pList2->numNodes;
        pList1->tail = pList2->tail;

        if (pList1->position == LIST_OOB_END)
        {
            pList1->current = pList2->head;
            pList1->position = LIST_NOOB;
        }
    }
    freeHead(pList2);
}

void List_free(List *pList, FREE_FN pItemFreeFn)
{
    if (pList->head != NULL)
    {
        pList->current = pList->head;
        for (int i = 0; i < pList->numNodes; i++)
        {
            (*pItemFreeFn)(pList->current->item);
            Node *temp = pList->current;
            pList->current = pList->current->next;
            freeNode(temp);
        }
        freeHead(pList);
    }
    else
    {
        freeHead(pList);
    }
}

void *List_trim(List *pList)
{
    if (pList->head == NULL)
    {
        return NULL;
    }
    else if (pList->numNodes == 1)
    {
        void *item = pList->head->item;
        pList->current = pList->head;
        pList->position = LIST_OOB_START;
        freeNode(pList->head);
        pList->head = NULL;
        pList->tail = NULL;
        pList->numNodes = 0;
        return item;
    }
    else
    {
        void *item = pList->tail->item;
        pList->current = pList->tail;
        pList->tail->prev->next = NULL;
        pList->tail = pList->tail->prev;
        freeNode(pList->current);
        pList->current = pList->tail;
        pList->numNodes--;
        return item;
    }
}

void *List_search(List *pList, COMPARATOR_FN pComparator, void *pComparisonArg)
{
    if (pList->head == NULL || pList->position == LIST_OOB_END)
    {
        return NULL;
    }

    if (pList->position == LIST_OOB_START)
    {
        pList->current = pList->head;
        pList->position = LIST_NOOB;
    }

    while (pList->current != NULL)
    {
        if ((*pComparator)(pList->current->item, pComparisonArg))
        {
            return pList->current->item;
        }
        pList->current = pList->current->next;
    }
    pList->position = LIST_OOB_END;
    return NULL;
}