#include "rwr/schedule/list.h"

static void timerlist_node_free(timerlist_node_t *node) {
    SDL_RemoveTimer(node->timer);

    free(node);
}

static timerlist_node_t* timerlist_node_add(timerlist_node_t **root, SDL_TimerID timer) {
    if(root == NULL) {
        timerlist_node_t *node = malloc(sizeof(*node));
        node->timer = timer;
        node->next = NULL;
        return node;
    }

    return timerlist_node_add(&(*root)->next, timer);
}

void timerlist_new(timerlist_t *list) {
    list->root = NULL;
}

timerlist_node_t* timerlist_add(timerlist_t *list, SDL_TimerID id) { return timerlist_node_add(&list->root, id); }

void timerlist_remove(timerlist_t *list, timerlist_node_t *node) {
    if(node == list->root) {
        list->root = list->root->next;
        timerlist_node_free(node);
    }
    timerlist_node_t *find = list->root;
    while(find != NULL && find->next != node) {
        find = find->next;
    }

    if(find != NULL && find->next == node) {
        find->next = node->next;
        timerlist_node_free(node);
    }
}

void timerlist_free(timerlist_t *list) {
    timerlist_node_t *node = list->root;
    while(node != NULL) {
        timerlist_node_t *next = node;
        timerlist_node_free(node);
        node = next;
    }
}
