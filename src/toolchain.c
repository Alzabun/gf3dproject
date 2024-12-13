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
            sj_array_delete_nth(parse, i);
            sentence_to_death(other);
            // this debug log does not show the right values
            slog("deleted entity: [name: '%s', type: '%s', position: '%.2f, %.2f, %.2f']", other->name, other->type, other->position.x,
                other->position.y, other->position.z);
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