    AQ  Written                 QQ// ThreadTree.c ... implementation of Tree-of-Mail-Threads ADT
// Written by John Shepherd, Feb 2019

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "MMList.h"
#include "MMTree.h"
#include "MailMessage.h"
#include "ThreadTree.h"

// Representation of ThreadTree's

typedef struct ThreadTreeNode *Link;

typedef struct ThreadTreeNode {
	MailMessage mesg;
	Link next, replies;
} ThreadTreeNode;

typedef struct ThreadTreeRep {
	Link messages;
} ThreadTreeRep;

// Auxiliary data structures and functions
static Link NewThreadTreeNode(MailMessage message);
static void InsertThreadTreeNode(Link root, Link new);
static Link SearchThreadTree(Link root, Link new);



// Add any new data structures and functions here ...

static void doDropThreadTree (Link t);
static void doShowThreadTree (Link t, int level);



// END auxiliary data structures and functions

// create a new empty ThreadTree
ThreadTree newThreadTree (void)
{
	ThreadTreeRep *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate ThreadTree");
	*new = (ThreadTreeRep) { };
	return new;
}

void dropThreadTree (ThreadTree tt)
{
	assert (tt != NULL);
	doDropThreadTree (tt->messages);
}

// free up memory associated with list
static void doDropThreadTree (Link t)
{
	if (t == NULL)
		return;

	for (Link curr = t, next; curr != NULL; curr = next) {
		next = curr->next;
		doDropThreadTree (curr->replies);
		// don't drop curr->mesg, in case referenced elsehwere
		free (curr);
	}
}

void showThreadTree (ThreadTree tt)
{
	assert (tt != NULL);
	doShowThreadTree (tt->messages, 0);
}

// display thread tree as hiearchical list
static void doShowThreadTree (Link t, int level)
{
	if (t == NULL)
		return;
	for (Link curr = t; curr != NULL; curr = curr->next) {
		showMailMessage (curr->mesg, level);
		doShowThreadTree (curr->replies, level + 1);
	}
}


// unused header file
#ifndef __unused
#define __unused __attribute__((unused))
#endif

// insert mail message into ThreadTree
// if a reply, insert in appropriate replies list
// whichever list inserted, must be in timestamp-order


ThreadTree ThreadTreeBuild (MMList mesgs, MMTree msgids __unused)
{
    // check list is ordered
    assert(mesgs != NULL);
    assert (MMListIsOrdered(mesgs));
    // initialize start of mesgs traversal
    MMListStart(mesgs);

    //create new threadtree
    ThreadTree tree = newThreadTree();

    MailMessage first = MMListNext(mesgs);
    //fprintf(stderr, "\nInserting ID %s, replies to ID %s\n", MailMessageID(first), MailMessageRepliesTo(first) == NULL ? "null" : MailMessageRepliesTo(first));
    //initialize root of threadtree
    tree->messages = NewThreadTreeNode(first);


    //traverse through messages
    while(!MMListEnd(mesgs)){
        // create new node for each message of list
        Link new = NewThreadTreeNode(MMListNext(mesgs));
        //showThreadTree(tree);
        //printf("===================\n");
        // insert the node 
        InsertThreadTreeNode(tree->messages,new);

    }
    assert(MMListEnd(mesgs));
    return tree;
}


// create new thread tree node 
static Link NewThreadTreeNode(MailMessage message){

    Link new = malloc(sizeof (ThreadTreeNode));
    assert (new != NULL);
    new->mesg = message;
    new->next = new->replies = NULL;
    return new;
}


// insert thread tree node 
static void InsertThreadTreeNode(Link email, Link new){

    //fprintf(stderr, "\nInserting ID %s, replies to ID %s\n", MailMessageID(new->mesg), MailMessageRepliesTo(new->mesg) == NULL ? "null" : MailMessageRepliesTo(new->mesg));

    if (MailMessageRepliesTo(new->mesg) != NULL){ // if a reply
        Link parent = SearchThreadTree(email,new);
        assert(parent != NULL);
        // case 1: lone node (reply)
        if (parent->replies == NULL){
            parent->replies = new;
        } else {  // case 2: lone node (multiple replies)
            Link curr = parent->replies;
            while (curr->next != NULL){
                curr = curr->next;
            }
            curr->next = new;
        }
    }
    if (MailMessageRepliesTo(new->mesg) == NULL){ //if no replies
        // case 3: lone node (!reply aka next)
        Link curr = email;
        if (curr->next == NULL){
            curr->next = new; //insert
        } else { // case 4: lone node (multiple !replies aka multiple nexts)
        Link tmp = curr->next;
        while (tmp->next != NULL){
            tmp = tmp->next;
            }
        tmp->next = new;
        }
    }
}




// search for the thread tree node 
static Link SearchThreadTree(Link root, Link new){

    if (root == NULL) return NULL; //check the node is non-null

    if (strcmp(MailMessageID(root->mesg),MailMessageRepliesTo(new->mesg)) == 0) {
        return root; //if the new node is a reply to the current node, return the node to insert function
    } else {
        Link searchNext = SearchThreadTree(root->next,new); //if not found, search for the node on the next branch of the current node
        if (searchNext == NULL){
            searchNext = SearchThreadTree(root->replies, new); //repeat on the replies branch of node
        }
        return searchNext; //return node when found
    }
}

















