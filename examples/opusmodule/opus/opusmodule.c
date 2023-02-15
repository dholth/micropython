// Include MicroPython API.
#include "py/runtime.h"
#include "py/stream.h"

#include <opusfile.h>

typedef struct _mp_obj_opus_t
{
    mp_obj_base_t base;
    mp_obj_t stream; // retain a reference to prevent GC from reclaiming it
    OggOpusFile *of;
} mp_obj_opus_t;

// int (*op_read_func)(void *_stream,unsigned char *_ptr,int _nbytes);
STATIC int op_stream_posix_read(void *_stream, unsigned char *_ptr, int _nbytes)
{
    return (int)mp_stream_posix_read(_stream, _ptr, _nbytes);
}

STATIC const OpusFileCallbacks mp_opus_callbacks = {
    &op_stream_posix_read,
    NULL,
    NULL,
    NULL};

#if !MICROPY_ENABLE_DYNRUNTIME
// STATIC const mp_obj_type_t mp_opus_type;
#endif

// Test if a file is opus.
STATIC mp_obj_t opus_test(mp_obj_t a_obj, mp_obj_t b_obj)
{
    OpusHead head;
    const unsigned char initial_data[57] = {0};
    int is_opus = op_test(&head, initial_data, 57);
    return mp_obj_new_int(is_opus);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(opus_test_obj, opus_test);

STATIC mp_obj_t mp_opus_free(mp_obj_t self_in)
{
    mp_obj_opus_t *self = MP_OBJ_TO_PTR(self_in);
    op_free(self->of);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_opus_free_obj, mp_opus_free);

STATIC mp_obj_opus_t *opus_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_opus_t *self = m_new_obj(mp_obj_opus_t);
    mp_obj_t *stream = args[0];
    self->base.type = (mp_obj_type_t *)type;
    self->stream = stream;

    // Allow string and auto-open?
    // if (mp_obj_is_str(stream))
    // {
    //     stream = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), stream, MP_ROM_QSTR(MP_QSTR_rb));
    // }

    int error;
    self->of = op_open_callbacks(stream, &mp_opus_callbacks, NULL, 0, &error);
    if (error != 0)
    {
        mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("opus error %d"), error);
    }

    return MP_OBJ_FROM_PTR(self);
}

// For the opusfile type
STATIC const mp_rom_map_elem_t opus_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&mp_opus_free_obj)},
};
STATIC MP_DEFINE_CONST_DICT(opus_locals_dict, opus_locals_dict_table);

// This defines the type(opus) object.
MP_DEFINE_CONST_OBJ_TYPE(
    mp_opus_type,
    MP_QSTR_opus,
    MP_TYPE_FLAG_NONE,
    make_new, opus_make_new,
    locals_dict, &opus_locals_dict);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t opus_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_opus)},
    {MP_ROM_QSTR(MP_QSTR_op_test), MP_ROM_PTR(&opus_test_obj)},
    {MP_ROM_QSTR(MP_QSTR_opus), MP_ROM_PTR(&mp_opus_type)},
};
STATIC MP_DEFINE_CONST_DICT(opus_module_globals, opus_module_globals_table);

// Define module object.
const mp_obj_module_t opus_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&opus_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_opus, opus_module);
