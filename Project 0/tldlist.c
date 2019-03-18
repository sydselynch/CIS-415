#include "tldlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


/* http://www.zentut.com/c-tutorial/c-avl-tree/ 


*/

struct tldlist {
	TLDNode *root;
	Date *start_date;
	Date *end_date;
	long count;
};

struct tldnode {
	long count;
	long height;
	char *tld;
	TLDNode *left_child;
	TLDNode *right_child;
	TLDNode *parent;
};

struct tlditerator {
	TLDNode **stack;
	long size;
	long num_elements;
};

TLDList *tldlist_create(Date *begin, Date *end) {
	TLDList *tld_list = malloc(sizeof(TLDList));
	if (tld_list != NULL) {
		tld_list->start_date = begin;
		tld_list->end_date = end;
		tld_list->count = 0;
		tld_list->root = NULL;
	}

	return tld_list;
}

void tldlist_destroy(TLDList *tld) {
	free(tld);
}

/* TLDNode Helper Functions */

static char *to_lower(char *s) {
	// convert all hostnames to lowercase
	for(int i = 0; s[i]; i++){
  		s[i] = tolower(s[i]);
  	}

	return s;
}

static TLDNode *tldnode_create(char *hostname) {
	TLDNode *tld_node = malloc(sizeof(TLDNode));
	if (tld_node != NULL) {
		tld_node->count = 1;
		tld_node->tld = to_lower(hostname);
		tld_node->left_child = NULL;
		tld_node->right_child = NULL;
		tld_node->parent = NULL;
		tld_node->height = 0;
	}

	return tld_node;
}

static TLDNode *tld_find(TLDNode *node, char *hostname) {
	TLDNode *current = node;
	while (current != NULL) {
		if (current->tld == hostname) {
			return current;
		}
		else if (strcmp(current->tld, hostname) < 0) {
			current = current->left_child;
		}
		else {
			current = current->right_child;
		}
	}

	return NULL;
}

static long node_height(TLDNode *node) {
	if (node != NULL) {
		return node->height;
	}

	return -1;
}


static int max(int x, int y) {
	if (x >= y) {
		return x;
	}
	return y;
}

static TLDNode *right_rotate(TLDNode *node) {
	TLDNode *y;
	y = node->right_child;	
	node->right_child = y->left_child;
	y->left_child = node;

	node->height = max(node_height(node->left_child), node_height(node->right_child)) + 1;
	y->height = max(node_height(y->right_child), node->height) + 1;

	return y;
}

static TLDNode *left_rotate(TLDNode *node) {
	TLDNode *y;
	y = node->left_child;	
	node->left_child = y->right_child;
	y->right_child = node;

	node->height = max(node_height(node->left_child), node_height(node->right_child)) + 1;
	y->height = max(node_height(y->left_child), node->height) + 1;

	return y;
}

static TLDNode *double_left_rotate(TLDNode *node) {
	node->left_child = right_rotate(node->left_child);

	return left_rotate(node);
}

static TLDNode *double_right_rotate(TLDNode *node) {
	node->right_child = left_rotate(node->right_child);

	return right_rotate(node);
}

static TLDNode *node_insert(TLDNode *node, char *hostname) {
	if (node == NULL) {
		node = tldnode_create(hostname);
	}
	else if (strcmp(hostname, node->tld) < 0) {
		node->left_child = node_insert(node->left_child, hostname);
		if(node_height(node->left_child) - node_height(node->right_child) == 2) {
			if (strcmp(hostname, node->left_child->tld) < 0) {
				node = left_rotate(node);
			}
			else {
				node = double_left_rotate(node);
			}
		}
	}
	else if (strcmp(hostname, node->tld) > 0) {
		node->right_child = node_insert(node->right_child, hostname);
		if (node_height(node->right_child) - node_height(node->left_child) == 2) {
			if (strcmp(hostname, node->right_child->tld) > 0) {
				node = right_rotate(node);
			}
			else {
				node = double_right_rotate(node);
			}
		}
	}

	node->height = max(node_height(node->left_child), node_height(node->right_child)) + 1;
	return node;
}

/* End TLDNode helper functions */

int tldlist_add(TLDList *tld, char *hostname, Date *d) {
	if (date_compare(d, tld->start_date) >= 0 && date_compare(d, tld->end_date) <= 0) {
		TLDNode *tld_node = tld_find(tld->root, hostname);
		
		if (tld_node != NULL) {
			tld_node->count++;
			tld->count++;
			return 1;
		}

		tld->root = node_insert(tld->root, hostname);
		if (tld->root != NULL) {
			tld->count++;
			return 1;
		}
	}

	return 0;
}

long tldlist_count(TLDList *tld) {
	return tld->count;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {
	TLDIterator *iter = malloc(sizeof(TLDIterator));
	if (iter != NULL) {
		iter->stack = malloc(sizeof(TLDNode*) * tld->root->height);
		iter->size = tld->root->height;
		iter->num_elements = 0;
		TLDNode *current = tld->root;
		iter->stack[0] = tld->root;
		iter->num_elements++;
		while (current->left_child != NULL) {
			current = current->left_child;
			iter->stack[iter->num_elements] = current;
			iter->num_elements++;
		}
	}

	return iter;

}

TLDNode *tldlist_iter_next(TLDIterator *iter) {
	TLDNode *next = iter->stack[iter->num_elements];
	TLDNode *current = next;
	iter->num_elements--;


	return next;


}

void tldlist_iter_destroy(TLDIterator *iter) {
	free(iter->stack);
	free(iter);
}


char *tldnode_tldname(TLDNode *node) {
	return node->tld;
}

long tldnode_count(TLDNode *node) {
	return node->count;
}


int main() {
	TLDList *testlist = tldlist_create(date_create("01/01/1999"), date_create("01/01/2002"));
	return 0;
}


