#include "simple_logger.h"
#include "entity.h"

#include "entitykiller.h"

void sentence_to_death(Entity* self) { // freeing in the middle of a function crashes the game, so use this to kill an entity
	if (self && self->_inuse) {
		//slog("something was just sentenced to death");
		entity_free(self);
	}
}