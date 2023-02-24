// Include MicroPython API.
#include "py/runtime.h"
#include "py/stream.h"

#include <oggz/oggz.h>
#include <opus/opus.h>

typedef struct _mp_obj_oggz_t
{
    mp_obj_base_t base;
    mp_obj_t stream; // retain a reference to prevent GC from reclaiming it
    OGGZ *oggz;
} mp_obj_oggz_t;

STATIC mp_obj_oggz_t *mp_oggz_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_oggz_t *self = m_new_obj(mp_obj_oggz_t);

    // make sure we have a stream
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ);

    mp_obj_t *stream = args[0];

    self->base.type = (mp_obj_type_t *)type;
    self->stream = stream;

    self->oggz = oggz_new(OGGZ_READ);
    if (self->oggz == NULL)
    {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error"));
    }

    int error;
    error = oggz_io_set_read(self->oggz, (OggzIORead)&mp_stream_posix_read, self->stream);
    if (error != 0)
    {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"), error);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t mp_oggz_close(mp_obj_t self_in)
{
    mp_obj_oggz_t *self = MP_OBJ_TO_PTR(self_in);
    int error = oggz_close(self->oggz);
    self->oggz = NULL;
    if (error != 0)
    {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"), error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_oggz_close_obj, mp_oggz_close);

STATIC mp_obj_t mp_oggz_read(mp_obj_t self_in, mp_obj_t n)
{
    mp_obj_oggz_t *self = MP_OBJ_TO_PTR(self_in);
    int bytes_read = oggz_read(self->oggz, mp_obj_get_int(n));
    if (bytes_read < 0)
    {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"), bytes_read);
    }
    return mp_obj_new_int(bytes_read);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_oggz_read_obj, mp_oggz_read);

STATIC const mp_rom_map_elem_t oggz_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_oggz_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_oggz_read_obj)},
};
STATIC MP_DEFINE_CONST_DICT(oggz_locals_dict, oggz_locals_dict_table);

// This defines the type(opus) object.
MP_DEFINE_CONST_OBJ_TYPE(
    mp_oggz_type,
    MP_QSTR_oggz,
    MP_TYPE_FLAG_NONE,
    make_new, mp_oggz_make_new,
    locals_dict, &oggz_locals_dict);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t oggz_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_oggz)},
    {MP_ROM_QSTR(MP_QSTR_oggz), MP_ROM_PTR(&mp_oggz_type)},
};
STATIC MP_DEFINE_CONST_DICT(oggz_module_globals, oggz_module_globals_table);

// Define module object.
const mp_obj_module_t oggz_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&oggz_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_oggz, oggz_module);
