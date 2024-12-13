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
    // name and type
    sj_object_insert(saveentity, "name", sj_new_str(name));
    sj_object_insert(saveentity, "type", sj_new_str(type));
    // position
    SJson* saveposition = sj_array_new();
    sj_array_append(saveposition, sj_new_float(x));
    sj_array_append(saveposition, sj_new_float(y));
    sj_array_append(saveposition, sj_new_float(z));
    sj_object_insert(saveentity, "position", saveposition);

    sj_array_append(parse, saveentity);

    sj_save(file, filename);
    slog("saved entity placement to file");

    sj_free(file);
}