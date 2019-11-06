// MMList.c ... implementation of List-of-Mail-Messages ADT
// Written by John Shepherd, Feb 2019

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "MMList.h"
#include "MailMessage.h"

// data structures representing MMList

typedef struct MMListNode *Link;

typedef struct MMListNode {
	MailMessage data; // message associated with this list item
	Link next;		  // pointer to node containing next element
} MMListNode;

typedef struct MMListRep {
	Link first; // node containing first value
	Link last;  // node containing last value
	Link curr;  // current node (for iteration)
} MMListRep;

static Link newMMListNode (MailMessage mesg);

// create a new empty MMList
MMList newMMList (void)
{
	MMListRep *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate MMList");
	*new = (MMListRep) { };
	return new;
}

// free up memory associated with list
// note: does not remove Mail Messages
void dropMMList (MMList L)
{
	assert (L != NULL);
	Link curr = L->first;
	while (curr != NULL) {
		Link next = curr->next;
		free (curr);
		curr = next;
	}
	free (L);
}

// display list as one integer per line on stdout
void showMMList (MMList L)
{
	assert (L != NULL);
	for (Link curr = L->first; curr != NULL; curr = curr->next)
		showMailMessage (curr->data, 0);
}

// insert mail message in order
// ordering based on MailMessageDateTime
// MMLISTINSERT FUNCTION
void MMListInsert (MMList L, MailMessage mesg)
{

//validity checking
assert (L != NULL);
assert (mesg != NULL);
MMListIsOrdered(L);

// create node & store the datetime value for the newnode
Link new_node = newMMListNode(mesg);
DateTime new_node_date = MailMessageDateTime(mesg);


// case 1: linked list has no nodes
if (L->first == NULL && L->last == NULL){
L->first = new_node;
new_node->next = NULL;
L->curr = L->last = new_node;
return;
}
    
// after checking for NULL
// store datetimes for first and last node    
DateTime first_node_date = MailMessageDateTime(L->first->data);
DateTime last_node_date = MailMessageDateTime(L->last->data);

// case 2: linked list has only 1 node
	// subcase 2.1: inserting before only node

if (L->first == L->last){
    if (DateTimeBefore(new_node_date,first_node_date)){
			new_node->next = L->first;
			L->first->next=NULL;
			L->first= L->last;
			L->first=new_node;
			L->curr = new_node;
        return;
    } else {

	// subcase 2.2: inserting after only node
			L->first->next = new_node;
			new_node->next = NULL;
			L->last = new_node;
			L->curr = new_node;
        return;
    }  
}
// case 3: adding before the first node of list   
if(DateTimeBefore(new_node_date,first_node_date)){
		new_node->next = L->first;
		L->first = new_node;
		L->curr = new_node;
    return;
   }


// case 4: adding after the last node of list
if(DateTimeAfter(new_node_date,last_node_date))
    {
		L->last->next = new_node;
		L->last = new_node;
		new_node->next = NULL; 
    return;
    }

// case 5: adding in the middle of list
Link prev = L->first;
Link curr = L->first->next;
while (curr != NULL && prev != NULL){
DateTime curr_date = MailMessageDateTime(curr->data);

    if (DateTimeBefore(new_node_date, curr_date)){
			new_node->next = curr;
			prev->next = new_node;
        break;
    	}
curr = curr->next;
prev = prev->next;
	}
}

// create a new MMListNode for mail message
// (this function is used only within the ADT)
static Link newMMListNode (MailMessage mesg)
{
	Link new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate MMList node");
	*new = (MMListNode) { .data = mesg };
	return new;
}

// check whether a list is ordered (by MailMessageDate)
bool MMListIsOrdered (MMList L)
{
	DateTime prevDate = NULL;
	for (Link n = L->first; n != NULL; n = n->next) {
		DateTime currDate = MailMessageDateTime (n->data);
		if (prevDate != NULL && DateTimeAfter (prevDate, currDate))
			return false;
		prevDate = currDate;
	}
	return true;
}

// start scan of an MMList
void MMListStart (MMList L)
{
	assert (L != NULL);
	L->curr = L->first;
}

// get next item during scan of an MMList
MailMessage MMListNext (MMList L)
{
	assert (L != NULL);
	if (L->curr == NULL)
		// this is probably an error
		return NULL;

	MailMessage mesg = L->curr->data;
	L->curr = L->curr->next;
	return mesg;
}

// check whether MMList scan is complete
bool MMListEnd (MMList L)
{
	assert (L != NULL);
	return (L->curr == NULL);
}
