/* --------------------------------------------------------------------
 * File    : list.h
 * --------------------------------------------------------------------
 * Description : Public interface for the generic singly-linked list
 *               module. Nodes store arbitrary data as void* pointers.
 *               The list does NOT own the data it holds; callers are
 *               responsible for managing element lifetimes.
 * --------------------------------------------------------------------
 * 08/11/2008   SR   Initial implementation
 * 23/10/2015   SR   Comments
 * --------------------------------------------------------------------
 */

#ifndef _LIST_H_
#define _LIST_H_

/*
 * struct _list_node - Single node in a singly-linked list
 *
 * @data: Generic pointer to the element stored at this node
 * @next: Pointer to the next node, or NULL if this is the tail
 */
struct _list_node
{
    void *data;
    struct _list_node *next;
};
typedef struct _list_node s_node;

/* -------------------------------------------------------------------
 * Lifecycle
 * ------------------------------------------------------------------- */

/*
 * list_create - Create an empty list
 *
 * Returns: NULL, which represents an empty list head. Provided as a
 *          named constructor for readability at call sites.
 */
s_node *list_create(void);

/*
 * list_destroy - Free all nodes in the list
 * @head: Head of the list to destroy
 *
 * Frees every node structure. The data pointers stored in the nodes
 * are NOT freed — element ownership stays with the caller.
 *
 * Returns: NULL, suitable for assigning back to the head pointer to
 *          prevent dangling references.
 */
s_node *list_destroy(s_node *head);

/* -------------------------------------------------------------------
 * Insertion
 * ------------------------------------------------------------------- */

/*
 * list_insert - Allocate a new node and prepend it to the list
 * @head: Current head of the list (may be NULL for an empty list)
 * @data: Element pointer to store in the new node
 *
 * O(1) insertion at the front of the list.
 *
 * Returns: The new head of the list.
 */
s_node *list_insert(s_node *head, void *data);

/*
 * list_append - Allocate a new node and append it to the tail
 * @head: Current head of the list (may be NULL for an empty list)
 * @data: Element pointer to store in the new node
 *
 * O(n) — traverses the full list to reach the tail. Prefer
 * list_append2() when performing multiple successive appends.
 *
 * Returns: The (unchanged) head of the list.
 */
s_node *list_append(s_node *head, void *data);

/*
 * list_append2 - Append a new node using a known tail pointer (fast path)
 * @head: Address of the head pointer (updated if the list was empty)
 * @tail: Known tail node, or NULL if the tail is unknown
 * @data: Element pointer to store in the new node
 *
 * O(1) when @tail is non-NULL. Falls back to O(n) traversal when
 * @tail is NULL. Callers should cache the returned tail and pass it
 * on subsequent calls to keep bulk-append loops at O(1) per step.
 *
 * Returns: The new tail node (pass this as @tail on the next call).
 */
s_node *list_append2(s_node **head, s_node *tail, void *data);

/*
 * list_orderedAppend - Insert @param before the first node that compares greater
 * @head:    Address of the head pointer (may be updated on insertion at front)
 * @compare: Comparison callback; receives (node, param) and must return
 *           a positive value when @node is strictly greater than @param
 * @param:   Element to insert (also passed as the second argument to @compare)
 *
 * Maintains ascending sorted order. If @param is already present in
 * the list (compare returns 0), no node is inserted.
 *
 * Returns: The (possibly updated) head of the list.
 */
s_node *list_orderedAppend(s_node **head, int (*compare)(s_node *, void *),
                            void *param);

/* -------------------------------------------------------------------
 * Removal
 * ------------------------------------------------------------------- */

/*
 * list_remove - Remove the first node whose data pointer matches @data
 * @head: Current head of the list
 * @data: Data pointer to search for (compared by address)
 *
 * Frees the matching node structure. The data itself is NOT freed.
 * Only the first match is removed; duplicates are left untouched.
 *
 * Returns: The (possibly updated) head of the list.
 */
s_node *list_remove(s_node *head, void *data);

/*
 * list_headRemove - Remove and free the head node
 * @head: Current head of the list
 *
 * O(1). The data stored in the removed node is NOT freed.
 *
 * Returns: The new head of the list (NULL if the list is now empty).
 */
s_node *list_headRemove(s_node *head);

/* -------------------------------------------------------------------
 * Traversal
 * ------------------------------------------------------------------- */

/*
 * list_process - Apply a function to each node until told to stop
 * @head:  Head of the list to traverse
 * @fct:   Callback invoked for each node; receives (node, param) and
 *         must return 0 to continue or 1 to stop the traversal early
 * @param: Caller-supplied context passed through to every @fct call
 * @last:  Set to the last node processed before the traversal ended;
 *         useful to resume iteration or inspect the stopping point
 *
 * Returns: 1 if the traversal was stopped early by @fct, 0 if the
 *          entire list was visited.
 */
int list_process(s_node *head, int (*fct)(s_node *node, void *param),
                 void *param, s_node **last);

/* -------------------------------------------------------------------
 * Access
 * ------------------------------------------------------------------- */

/*
 * list_isEmpty - Test whether a list is empty
 * @head: Head of the list to test
 *
 * Returns: 1 if @head is NULL (empty list), 0 otherwise.
 */
extern int list_isEmpty(s_node *head);

/*
 * list_next - Advance to the next node
 * @node: Current node
 *
 * Returns: The successor node, or NULL if @node is the tail.
 */
extern s_node *list_next(s_node *node);

/*
 * list_get_data - Retrieve the data pointer stored in a node
 * @node: Source node
 *
 * Returns: The void* data pointer stored at @node.
 */
extern void *list_get_data(s_node *node);

/* -------------------------------------------------------------------
 * Convenience macros
 * ------------------------------------------------------------------- */

/*
 * GET_LIST_DATA - Read a node's data pointer cast to a specific type
 * @node: Source node
 * @type: Target pointer type (e.g. char*, struct Noeud*)
 *
 * Example: char *word = GET_LIST_DATA(node, char*);
 */
#define GET_LIST_DATA(node, type) ((type)(node->data))

/*
 * SET_LIST_DATA - Write a typed pointer into a node's data field
 * @node:   Target node
 * @mydata: Value to store (cast to void* internally)
 *
 * Example: SET_LIST_DATA(node, my_struct_ptr);
 */
#define SET_LIST_DATA(node, mydata) (node->data = (void *)(mydata))

#endif /* _LIST_H_ */