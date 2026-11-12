/* DICU Tudor - 313CD */
#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	struct node *next, *data;
} node;

typedef struct {
	node *head, *tail;
} queue;

typedef struct {
	node *top;
} stack;

void enqueue(queue *q, void *data) {
	node *new_node = (node *)malloc(sizeof(node));

	if (new_node == NULL)
		return;
	new_node->data = data;
	new_node->next = NULL;

	if (q->tail == NULL)
	{
		q->head = new_node;
		q->tail = new_node;
	}
	else
	{
		q->tail->next = new_node;
		q->tail = new_node;
	}
}

void *dequeue(queue *q) {
	node *temp, *data;

	if (q->head == NULL)
		return NULL;

	temp = q->head;
	data = temp->data;
	q->head = q->head->next;

	if (q->head == NULL)
		q->tail = NULL;

	free(temp);
	return data;
}

void push(stack *s, void *data){
	node *new_node = (node *)malloc(sizeof(node));

	if (new_node == NULL)
		return;

	new_node->data = data;
	new_node->next = s->top;
	s->top = new_node;
}

void *pop(stack *s) {
	node *temp, *data;

	if (s->top == NULL)
		return NULL;

	temp = s->top;
	data = temp->data;
	s->top = s->top->next;

	free(temp);
	return data;
}

#endif