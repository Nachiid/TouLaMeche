/* -----------------------------------------------------------
   module list
   liste chainee
   -----------------------------------------------------------
   08/11/08	SR	creation
   23/10/15	SR 	commentaires
   -----------------------------------------------------------
*/

#ifndef _LIST_H_
#define _LIST_H_

struct _list_node
{
   void *data;
   struct _list_node *next;
};
typedef struct _list_node s_node;

// creation d'une nouvelle liste vide
s_node *list_create(void);

// creation et insertion d'un noeud en tete de liste
//   retourne la tete de liste
s_node *list_insert(s_node *head, void *data);

// creation et ajout d'un noeud en queue de liste
//   retourne la tete de liste
s_node *list_append(s_node *head, void *data);

// creation et ajout d'un noeud en queue de liste
//  2nd version plus rapide en cas d'ajouts multiples
//  si la queue de liste n'est pas connue, tail peut etre mis a NULL
//  retourne la queue de liste
//  le tete de liste peut etre modifiee par la fonction
s_node *list_append2(s_node **head, s_node *tail, void *data);

// suppression de la premiere instance d'une donnée dans la liste
//   retourne la tete de liste
s_node *list_remove(s_node *head, void *data);

// suppression de la tete de liste
//   retourne la nouvelle tete de liste
s_node *list_headRemove(s_node *head);

// Destruction d'une liste
//  (La libération des donnees n'est pas prise en charge)
s_node *list_destroy(s_node *head);

// Application d'une fonction sur les donnees enregistrees dans le liste
//   last est le dernier noeud traite'
//    retourne 1 sur le parcours est arrete' avant la fin de la liste
int list_process(s_node *head, int (*fct)(s_node *node, void *param), void *param, s_node **last);

// Ajout d'une donnee avant le premier noeud superieur, au sens de la fonction compare,
//   au parametres param. Pas d'ajout sur la données est déjà dans la liste
// retourne la tete de liste
s_node *list_orderedAppend(s_node **head, int (*compare)(s_node *, void *),
                           void *param);

// liste vide ?
//  retourne 1 si la liste est vide, 0 sinon
extern int list_isEmpty(s_node *head);

// noeud suivant
extern s_node *list_next(s_node *node);
extern void *list_get_data(s_node *node);

// acces aux champs
// type en un type pointeur generique
#define GET_LIST_DATA(node, type) ((type)(node->data))
#define SET_LIST_DATA(node, mydata) (node->data = (void *)(mydata))
#endif
