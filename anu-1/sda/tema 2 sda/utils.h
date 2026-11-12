#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_LEN 105

/* file in the doubly linked list */
typedef struct FileNode {
	char *id;
	char **keywords;
	int score, kw_count, kw_capacity;
	struct FileNode *next, *prev;
} FileNode;

/* reference to a file, stored inside a retrieval tree terminal node */
typedef struct FileRef {
	FileNode *file;
	struct FileRef *next;
} FileRef;

typedef struct RetrievalNode {
	struct RetrievalNode *children[ALPHABET_SIZE];
	FileRef *file_refs;
	int is_end;
} RetrievalNode;

typedef struct Heap {
	FileNode **data;
	int size, capacity;
} Heap;

typedef struct IndexSystem {
	FileNode *head, *tail;
	RetrievalNode *root;
} IndexSystem;

RetrievalNode *create_retrieval_node(void);
FileNode *create_file_node(
char *id, int score);
void free_file_node(FileNode *file);
IndexSystem *init_system(void);
void cleanup_system(IndexSystem *sys);

FileNode *find_file(IndexSystem *sys, char *id);
int file_has_keyword(FileNode *file, char *kw);
void add_keyword_to_file(FileNode *file, char *kw);
void remove_keyword_from_file(FileNode *file, char *kw);
void list_append(IndexSystem *sys, FileNode *file);
void list_remove(IndexSystem *sys, FileNode *file);

void retrieval_insert(RetrievalNode *root, char *word, FileNode *file);
RetrievalNode *retrieval_find(RetrievalNode *root, char *word);
void retrieval_remove_file(IndexSystem *sys, char *word, FileNode *file);

Heap *heap_create(int cap);
void heap_free(Heap *h);
void heap_push(Heap *h, FileNode *f);
FileNode *heap_pop(Heap *h);

void do_add(IndexSystem *sys, char *id, int score, int num_kw, FILE *in, FILE *out);
void do_del(IndexSystem *sys, char *id, FILE *out);
void do_addkw(IndexSystem *sys, char *id, char *word, FILE *out);
void do_delkw(IndexSystem *sys, char *id, char *word, FILE *out);
void do_find(IndexSystem *sys, char *word, FILE *out);
void do_topk(IndexSystem *sys, char *word, int k, FILE *out);
void do_prefix(IndexSystem *sys, char *prefix, FILE *out);
void do_print(IndexSystem *sys, FILE *out);

#endif
