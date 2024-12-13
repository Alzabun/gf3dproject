#include "toolchain.h"

void save_debug_file(char* name, char* type, float x, float y, float z/* , should use file name obtained from load game */) {
    char* filename = "def/levels/toolchain.json"; // placeholder
    SJson* file = sj_load(filename);

    if (!file) {
        slog("could not open file");
        return;
    }

    SJson* parse = sj_object_get_value(file, "entitylist");

    if (!parse) {
        slog("did not find any object name");
        return;
    }

    SJson* saveentity = sj_object_new();

    if (!saveentity) {
        slog("could not make new object");
        return;
    }
    // name and type
    sj_object_insert(saveentity, "name", sj_new_str(name));
    sj_object_insert(saveentity, "type", sj_new_str(type));
    // position
    SJson* saveposition = sj_array_new();
    if (!saveposition) {
        slog("could not make new array for position");
        return;
    }
    sj_array_append(saveposition, sj_new_float(x));
    sj_array_append(saveposition, sj_new_float(y));
    sj_array_append(saveposition, sj_new_float(z));
    sj_object_insert(saveentity, "position", saveposition);

    sj_array_append(parse, saveentity);

    sj_save(file, filename);
    slog("saved entity placement to file");

    sj_free(file);
}

void debug_delete(Entity* other /* get the file name for this */) {
    char* filename = "def/levels/toolchain.json"; // placeholder
    SJson* file = sj_load(filename); // placeholder

    if (!file) {
        slog("could not open file");
        return;
    }

    SJson* parse = sj_object_get_value(file, "entitylist");

    if (!parse) {
        slog("did not find any object name");
        return;
    }

    if (!sj_is_array(parse)) {
        slog("value of object is not an array");
        return;
    }

    for (int i = 0; i < sj_array_get_count(parse); i++) {
        SJson* list = sj_array_get_nth(parse, i);

        char* name = sj_object_get_string(list, "name");

        if (!name) {
            slog("no 'name' found in json");
            return;
        }

        char* type = sj_object_get_string(list, "type");

        if (!type) {
            slog("no 'type' found in json");
            return;
        }

        float x = 0, y = 0, z = 0;
        SJson* position = sj_object_get_value(list, "position");


        if (position) {
            SJson* x_pos = sj_array_get_nth(position, 0);
            sj_get_float_value(x_pos, &x);

            SJson* y_pos = sj_array_get_nth(position, 1);
            sj_get_float_value(y_pos, &y);

            SJson* z_pos = sj_array_get_nth(position, 2);
            sj_get_float_value(z_pos, &z);
        }
        else {
            slog("did not find a position for the scheduled deleted entity");
            return;
        }

        GFC_Vector3D parsedposition = gfc_vector3d(x, y, z);    

        if (strcmp(other->name, name) == 0 && strcmp(other->type, type) == 0 && position_comparison(parsedposition, other->position)) {
            slog("deleted entity: [name: '%s', type: '%s', position: '%.2f, %.2f, %.2f']", other->name, other->type, other->position.x,
                other->position.y, other->position.z);
            sj_array_delete_nth(parse, i);
            sentence_to_death(other);
            break;
        }

    }

    sj_save(file, filename);
    sj_free(file);

}

int position_comparison(GFC_Vector3D self, GFC_Vector3D other) {
    if (self.x == other.x && self.y == other.y && self.z == other.z) {
        return 1;
    }
    return 0;
}

void spawn_entity(char* name, char* type, GFC_Vector3D position) {
    // holy unoptimized
    // but again, idk how to make it better (this is fine for now)

    if (strcmp(name, "terrain") == 0) {
        if (strcmp(type, "floor") == 0) {
            Entity* floor_terrain = terrain_spawn(position);
            floor_terrain->name = name;
            floor_terrain->type = type;
            floor_terrain->position = position;
        }
    }
    if (strcmp(name, "spring") == 0) {
        if (strcmp(type, "yellow") == 0) {
            Entity* spring_yellow = spring_yellow_spawn(position);
            spring_yellow->name = name;
            spring_yellow->type = type;
            spring_yellow->position = position;
        }
    }
    if (strcmp(name, "spikes") == 0) {
        if (strcmp(type, "generic") == 0) {
            Entity* spikes_generic = spikes_spawn(position);
            spikes_generic->name = name;
            spikes_generic->type = type;
            spikes_generic->position = position;
        }
    }
    if (strcmp(name, "moving_platform") == 0) {
        if (strcmp(type, "vertical") == 0) {
            Entity* vertical_moving_platform = v_moving_platform_spawn(position);
            vertical_moving_platform->name = name;
            vertical_moving_platform->type = type;
            vertical_moving_platform->position = position;
        }
    }
    if (strcmp(name, "loop") == 0) {
        if (strcmp(type, "generic") == 0) {
            Entity* loop_generic = loop_spawn(position);
            loop_generic->name = name;
            loop_generic->type = type;
            loop_generic->position = position;
        }
    }
    if (strcmp(name, "itembox") == 0) {
        if (strcmp(type, "fire_shield") == 0) {
            Entity* fire_shield_box = itembox_spawn(position, 1);
            fire_shield_box->name = name;
            fire_shield_box->type = type;
            fire_shield_box->position = position;
        }
        if (strcmp(type, "bubble_shield") == 0) {
            Entity* bubble_shield_box = itembox_spawn(position, 2);
            bubble_shield_box->name = name;
            bubble_shield_box->type = type;
            bubble_shield_box->position = position;
        }
    }
    if (strcmp(name, "bomb_dropper") == 0) {
        if (strcmp(type, "generic") == 0) {
            Entity* generic_bomb_dropper = bomb_spawn(position);
            generic_bomb_dropper->name = name;
            generic_bomb_dropper->type = type;
            generic_bomb_dropper->position = position;
        }
    }

    //slog("spawned '%c' of type '%c'", name, type);
}
