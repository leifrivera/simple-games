#include <stdio.h>
#include <simple-games.h>
#if defined(__linux)
#include <uuid/uuid.h>
#endif
#include <SDL3/SDL.h>

int main() {
	pf_instance_t *instance = pf_instance_create();

	while (instance->running) {
		pf_instance_update(instance);
	}

	pf_instance_destroy(instance);
	return 0;
}

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	size_t window_width;
	size_t window_height;
} backend_data_t;

void backend_init(pf_instance_t *inst, size_t viewport_width, size_t viewport_height) {
	inst->backend_persistent_data = (backend_data_t *)calloc(1, sizeof(backend_data_t));
	backend_data_t *data = (backend_data_t *)inst->backend_persistent_data;
	
	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		printf("SDL failed to init: %s\n", SDL_GetError());
	}
	
	data->window = SDL_CreateWindow("Test window", viewport_width, viewport_height, SDL_WINDOW_RESIZABLE);
	data->window_width = viewport_width;
	data->window_height = viewport_height;
	if (!data->window) {
		printf("Window creation failed: %s\n", SDL_GetError());
		SDL_Quit();
	}
	data->renderer = SDL_CreateRenderer((SDL_Window *)data->window, "software");
	if (!data->renderer) {
		printf("Renderer creation failed: %s\n", SDL_GetError());
	}

	inst->running = true;
}

void backend_destroy(pf_instance_t *inst) {
	backend_data_t *data = (backend_data_t *)inst->backend_persistent_data;
	SDL_DestroyRenderer(data->renderer);
	SDL_DestroyWindow(data->window);
	SDL_Quit();
	free(inst->backend_persistent_data);
	inst->backend_persistent_data = NULL;
}

void backend_pump_events(pf_instance_t *inst) {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_EVENT_QUIT:
				pf_event_queue_add(inst, PF_EVENT_QUIT);
				break;
			case SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_E) {
					pf_event_queue_add(inst, PF_EVENT_ACCEPT);
				} else if (event.key.key == SDLK_Q) {
					pf_event_queue_add(inst, PF_EVENT_RETURN);
				}
				break;
		}
	}
}

void backend_render_present(pf_instance_t *inst) {
	backend_data_t *data = (backend_data_t *)inst->backend_persistent_data;
	SDL_RenderPresent(data->renderer);
}

#if defined(__linux)
void backend_uuid_generate(pf_uuid_t *uuid) {
	uuid->id = NULL;
	uuid->str = NULL;

	uuid_t *id = malloc(sizeof(uuid_t));
	char *str = malloc(37);

	if (!id || !str) {
		free(id);
		free(str);
		return;
	}

	uuid_generate_random(*id);
	uuid_unparse_lower(*id, str);
	uuid->id = (void *)id;
	uuid->str = str;
}

bool backend_uuid_same(pf_uuid_t *uuid1, pf_uuid_t *uuid2) {
	if (!uuid1 || !uuid2 || !uuid1->id || !uuid2->id) {
        	return false;
    	}

	uuid_t *id1 = (uuid_t *)uuid1->id;
	uuid_t *id2 = (uuid_t *)uuid2->id;

	return uuid_compare(*id1, *id2) == 0;
}
#elif defined(_WIN32)
#endif
