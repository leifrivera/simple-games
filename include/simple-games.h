#ifndef SIMPLE_GAMES_H
#define SIMPLE_GAMES_H
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	void *id;
	char *str;
} pf_uuid_t;

typedef enum {
	PF_EVENT_NULL = 0,
	PF_EVENT_QUIT	= 1,
	PF_EVENT_ACCEPT	= 2,
	PF_EVENT_RETURN	= 3
} pf_event_type_t;

typedef struct {
	pf_event_type_t type;
	pf_uuid_t *id;
} pf_event_t;

typedef struct {
	pf_event_t **event_queue;
	size_t event_queue_size;
	void *backend_persistent_data;
	bool running;
} pf_instance_t;

pf_instance_t *pf_instance_create();
void pf_instance_destroy(pf_instance_t *inst);
void pf_instance_update(pf_instance_t *inst);

pf_event_t *pf_event_create(pf_event_type_t type);
void pf_event_destroy(pf_event_t *event);

void pf_event_queue_add(pf_instance_t *inst, pf_event_type_t type);
void pf_event_queue_pop(pf_instance_t *inst);
pf_event_t *pf_event_queue_return_pop(pf_instance_t *inst);
bool pf_event_queue_poll(pf_instance_t *inst, pf_event_t **event);
void pf_event_queue_clear(pf_instance_t *inst);

void backend_init(pf_instance_t *inst, size_t viewport_width, size_t viewport_height);
void backend_destroy(pf_instance_t *inst);
void backend_pump_events(pf_instance_t *inst);
void backend_render_present(pf_instance_t *inst);

void backend_uuid_generate(pf_uuid_t *uuid);
bool backend_uuid_same(pf_uuid_t *uuid1, pf_uuid_t *uuid2);
void pf_uuid_free(pf_uuid_t *uuid);

void game_init(pf_instance_t *inst);
void game_update(pf_instance_t *inst);
void game_render(pf_instance_t *inst);
void game_event(pf_instance_t *inst, pf_event_t *event);

#endif
