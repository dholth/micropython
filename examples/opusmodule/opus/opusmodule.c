// Include MicroPython API.
#include "py/runtime.h"
#include "py/stream.h"

#include <opusfile.h>

STATIC int op_stream_posix_read(void *_stream, unsigned char *_ptr,
                                int _nbytes);

typedef struct _mp_obj_opus_t {
  mp_obj_base_t base;
  mp_obj_t stream; // retain a reference to prevent GC from reclaiming it
  OggOpusFile *of;
} mp_obj_opus_t;

STATIC int op_stream_posix_read(void *_stream, unsigned char *_ptr,
                                int _nbytes) {
  // cast return type to int from ssize_t
  return (int)mp_stream_posix_read(_stream, _ptr, _nbytes);
}

STATIC const OpusFileCallbacks mp_opus_callbacks = {&op_stream_posix_read, NULL,
                                                    NULL, NULL};

// Test if a file is opus, given ideally >=57 bytes from the beginnning of the
// file.
STATIC mp_obj_t opus_test(mp_obj_t buf_obj) {
  mp_buffer_info_t bufinfo;
  OpusHead head;

  mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_READ);

  int is_opus = op_test(&head, bufinfo.buf, bufinfo.len);
  if (is_opus < 0) {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("opus error %d"),
                      is_opus);
  }
  return mp_const_true;
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_1(opus_test_obj, opus_test);

// Does not close the underlying file, if any.
STATIC mp_obj_t mp_opus_close(mp_obj_t self_in) {
  mp_obj_opus_t *self = MP_OBJ_TO_PTR(self_in);
  if (self->of == NULL) {
    mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("opus closed"));
  }
  op_free(self->of);
  self->of = NULL;
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_opus_close_obj, mp_opus_close);

// Always decodes as 48khz 16-bit stereo.
STATIC mp_obj_t mp_op_read_stereo(mp_obj_t self_in, mp_obj_t buf_obj) {
  // "It is recommended that [the buffer] be large enough for at least 120 ms
  // of data at 48 kHz per channel (11520 [16-bit] values total)." 48e3 *
  // (120/1000) * 2 = 11520. i.e. 23040. bytes. (Otherwise it may just buffer
  // extra samples internally)

  // dholth: if you know you only use shorter frames, shorter buffers may be
  // fine

  mp_obj_opus_t *self = MP_OBJ_TO_PTR(self_in);
  mp_buffer_info_t bufinfo;
  mp_int_t samples_read;

  mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_WRITE);

  if (self->of == NULL) {
    mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("opus closed"));
  }

  samples_read =
      op_read_stereo(self->of, (opus_int16 *)bufinfo.buf, (int)bufinfo.len);

  if (samples_read < 0) {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("opus error %d"),
                      samples_read);
  }

  return mp_obj_new_int(samples_read);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_op_read_stereo_obj, mp_op_read_stereo);

STATIC mp_obj_opus_t *opus_make_new(const mp_obj_type_t *type, size_t n_args,
                                    size_t n_kw, const mp_obj_t *args) {
  mp_arg_check_num(n_args, n_kw, 1, 1, false);
  mp_obj_opus_t *self = m_new_obj(mp_obj_opus_t);

  // make sure we have a stream
  mp_get_stream_raise(args[0], MP_STREAM_OP_READ);
  // could use stream pointer functions directly in opus interface?

  mp_obj_t *stream = args[0];

  self->base.type = (mp_obj_type_t *)type;
  self->stream = stream;

  int error;
  self->of = op_open_callbacks(stream, &mp_opus_callbacks, NULL, 0, &error);
  if (error != 0) {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("opus error %d"),
                      error);
  }

  return MP_OBJ_FROM_PTR(self);
}

// For the opusfile type
STATIC const mp_rom_map_elem_t opus_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_opus_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&mp_opus_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_read_stereo), MP_ROM_PTR(&mp_op_read_stereo_obj)},
};
STATIC MP_DEFINE_CONST_DICT(opus_locals_dict, opus_locals_dict_table);

// This defines the type(opus) object.
MP_DEFINE_CONST_OBJ_TYPE(mp_opus_type, MP_QSTR_opus, MP_TYPE_FLAG_NONE,
                         make_new, opus_make_new, locals_dict,
                         &opus_locals_dict);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t opus_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_opus)},
    {MP_ROM_QSTR(MP_QSTR_opus_test), MP_ROM_PTR(&opus_test_obj)},
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
