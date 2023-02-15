// Include MicroPython API.
#include "py/runtime.h"

// Used to get the time in the Timer class example.
#include "py/mphal.h"

#include <opusfile.h>

// Test if a file is opus.
STATIC mp_obj_t opus_test(mp_obj_t a_obj, mp_obj_t b_obj) {
    OpusHead head;
    const unsigned char initial_data[57] = {0};
    int is_opus = op_test(&head, initial_data, 57);
    return mp_obj_new_int(is_opus);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(opus_test_obj, opus_test);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t opus_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_opus) },
    { MP_ROM_QSTR(MP_QSTR_op_test), MP_ROM_PTR(&opus_test_obj) },
};
STATIC MP_DEFINE_CONST_DICT(opus_module_globals, opus_module_globals_table);

// Define module object.
const mp_obj_module_t opus_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&opus_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_opus, opus_module);
