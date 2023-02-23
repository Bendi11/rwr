#include "rwr/schedule/schedule.h"
#include "priv.h"

static void schedule_timer_free(schedule_timer_t *node) {
    SDL_RemoveTimer(node->timer);
    free(node);
}

void schedule_new(schedule_t *list, alr56_t *rwr) {
    list->root = NULL;
    list->rwr = rwr;
}

schedule_timer_t* schedule_add(schedule_t *list, const timerprofile_t *prof) {
    if(list->root == NULL) {
        list->root = schedule_timer_new(list, prof);
        return list->root;
    }

    schedule_timer_t *last = list->root;
    while(last->next != NULL) { last = last->next; }
    
    last->next = schedule_timer_new(list, prof);
    return last->next;
}

void schedule_remove(schedule_t *list, schedule_timer_t *node) {
    if(node == list->root) {
        list->root = list->root->next;
        schedule_timer_free(node);
    }
    schedule_timer_t *find = list->root;
    while(find != NULL && find->next != node) {
        find = find->next;
    }

    if(find != NULL && find->next == node) {
        find->next = node->next;
        schedule_timer_free(node);
    }
}

void schedule_free(schedule_t *list) {
    schedule_timer_t *node = list->root;
    while(node != NULL) {
        schedule_timer_t *next = node;
        schedule_timer_free(node);
        node = next;
    }
}
