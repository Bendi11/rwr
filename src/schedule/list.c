#include "rwr/schedule/list.h"

static void timerlist_node_free(timerlist_node_t *node) {
    SDL_RemoveTimer(node->timer);

    free(node);
}

static timerlist_node_t* timerlist_node_add(timerlist_node_t **root, timerlist_node_t *prev, SDL_TimerID timer) {
    if(root == NULL) {
        timerlist_node_t *node = malloc(sizeof(*node));
        node->timer = timer;
        node->prev = prev;
        node->next = NULL;
        return node;
    }

    return timerlist_node_add(&(*root)->next, *root, timer);
}

void timerlist_new(timerlist_t *list) {
    list->root = NULL;
}

void timerlist_add(timerlist_t *list, SDL_TimerID id) {
    timerlist_node_add(&list->root, NULL, id);
}
