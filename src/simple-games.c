#include <simple-games.h>
#include <stdio.h>

void pf_uuid_free(pf_uuid_t *uuid) {
	if (!uuid) return;
	free(uuid->id);
	free(uuid->str);
	uuid->id = NULL;
	uuid->str = NULL;
}

pf_event_t *pf_event_create(pf_event_type_t type) {
	pf_event_t *event = calloc(1, sizeof(pf_event_t));
	event->type = type;
	event->id = calloc(1, sizeof(pf_uuid_t));
	if (!event->id) {
		free(event);
		return NULL;
	}
	backend_uuid_generate(event->id);
	if (!event->id->id || !event->id->str) {
		pf_event_destroy(event);
		return NULL;
	}
#ifdef DEBUG
	printf("Event created; UUID: %s, Type ID: %d\n", event->id->str, event->type);
#endif
	return event;
}

void pf_event_destroy(pf_event_t *event) {
	if (!event) return;
	pf_uuid_free(event->id);
	free(event->id);
	event->id = NULL;
	free(event);
}

void pf_event_queue_add(pf_instance_t *inst, pf_event_type_t type) {
	if (!inst) return;

	pf_event_t *event = pf_event_create(type);
	if (!event) return;
	
	pf_event_t **new_queue = realloc(
		inst->event_queue,
		(inst->event_queue_size+1) * sizeof(pf_event_t *)
	);

	if (!new_queue) {
#ifdef DEBUG
		printf("Memory allocation failed\n");
#endif
		return;
	}

	inst->event_queue = new_queue;
	inst->event_queue[inst->event_queue_size] = event;
	inst->event_queue_size++;

#ifdef DEBUG
	printf("Event added to queue: %s\n", event->id->str);
#endif
}

// FIFO
void pf_event_queue_pop(pf_instance_t *inst) {
	if (!inst || inst->event_queue_size <= 0) return;

	char uuid_buffer[37];
	snprintf(uuid_buffer, sizeof(uuid_buffer), "%s", inst->event_queue[0]->id->str);

	pf_event_destroy(inst->event_queue[0]);

	for (size_t i = 1; i < inst->event_queue_size; i++) {
		inst->event_queue[i-1] = inst->event_queue[i];
	}

	inst->event_queue_size--;

	if (inst->event_queue_size == 0) {
		free(inst->event_queue);
		inst->event_queue = NULL;
	}

#ifdef DEBUG
	printf("Event queue popped: %s\n", uuid_buffer);
#endif
}

// FIFO
pf_event_t *pf_event_queue_return_pop(pf_instance_t *inst) {
	if (!inst || inst->event_queue_size <= 0) return NULL;

	//pf_event_destroy(inst->event_queue[0]);
	pf_event_t *event = inst->event_queue[0];

	for (size_t i = 1; i < inst->event_queue_size; i++) {
		inst->event_queue[i-1] = inst->event_queue[i];
	}

	inst->event_queue_size--;

	if (inst->event_queue_size == 0) {
		free(inst->event_queue);
		inst->event_queue = NULL;
	}

	return event;
}

// Requires manual destruction and nullification of event per loop
bool pf_event_queue_poll(pf_instance_t *inst, pf_event_t **event) {
	if (!inst || !event)
		return false;
	
	*event = pf_event_queue_return_pop(inst);

	return *event != NULL;
}

void pf_event_queue_clear(pf_instance_t *inst) {
	if (!inst || !inst->event_queue) return;

	for (size_t i = 0; i < inst->event_queue_size; i++) {
		pf_event_destroy(inst->event_queue[i]);
	}

	free(inst->event_queue);

	inst->event_queue = NULL;
	inst->event_queue_size = 0;

#ifdef DEBUG
	printf("Event queue: cleared\n");
#endif
}

pf_instance_t *pf_instance_create() {
	pf_instance_t *inst = calloc(1, sizeof(pf_instance_t));
	inst->event_queue = NULL;
	inst->backend_persistent_data = NULL;
	backend_init(inst, 640, 480);
	return inst;
}

void pf_instance_destroy(pf_instance_t *inst) {
	if (!inst) return;

	pf_event_queue_clear(inst);
	backend_destroy(inst);
	free(inst);
}

void pf_instance_update(pf_instance_t *inst) {
	backend_pump_events(inst);
	
	pf_event_t *event = NULL;
	while (pf_event_queue_poll(inst, &event)) {
		if (event->type == PF_EVENT_QUIT) inst->running = false;
		game_event(inst, event);

		pf_event_destroy(event);
		event = NULL;
	}

	backend_render_present(inst);
}
