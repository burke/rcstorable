#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ruby.h"

VALUE thaw(VALUE, VALUE);
VALUE read_object();
int read_extended_size();
int read_compact_size();
void read_n_hash_pairs(VALUE, int);
void read_n_array_entries(VALUE, int);
VALUE read_string(bool);
void read_magic_numbers();

enum states
{
  READ_EXTENDED_SIZE,
  READ_COMPACT_SIZE,
  READ_NEW_OBJECT
};

enum object_types
{
  O_STR       = 1,
  O_ARRAY     = 2,
  O_HASH      = 3,
  O_UNDEFINED = 5
};

enum hash_key_types
{
  H_VECTOR     = 4,
  H_EMPTY      = 5,
  H_STRING     = 10,
  H_STRING_ALT = 23
};

// Used globally. Raptors. I know.
unsigned char *serialized;
unsigned char *serialized_end;

void
check_pointer(unsigned char *ptr)
{
  extern unsigned char *serialized_end;
  if (ptr > serialized_end) {
    rb_raise(rb_eRangeError, "malformed data");
  }
}

VALUE
thaw(VALUE self, VALUE str)
{
  Check_Type(str, T_STRING);
  extern unsigned char *serialized, *serialized_end;

  serialized = RSTRING_PTR(str);
  serialized_end = serialized + RSTRING_LEN(str);
  
  read_magic_numbers();
  
  return read_object();
}

void
read_magic_numbers()
{
  serialized++;
  serialized++;
}

VALUE
read_object()
{
  extern unsigned char *serialized;
  check_pointer(serialized);
  int type = *serialized++;
  int size = read_extended_size();

  VALUE object;

  switch(type) {
  case O_HASH:
    object = rb_hash_new();
    read_n_hash_pairs(object, size);
    break;
  case O_ARRAY:
    object = rb_ary_new();
    read_n_array_entries(object, size);
    break;
  case O_UNDEFINED:
    object = Qnil;
    break;
  }
  
  return object;
}

  
void
read_n_hash_pairs(VALUE hash, int num)
{
  extern unsigned char *serialized;
  check_pointer(serialized);
  
  if (num == 0) {
    return;
  }
  
  int type = *serialized++;
  VALUE temp;
  VALUE str;
  
  switch(type) {
  case H_EMPTY:
    rb_hash_aset(hash, read_string(true), Qnil);
    break;
  case H_VECTOR:
    temp = read_object();
    str = read_string(true);
    rb_hash_aset(hash, str, temp);
    break;
  case H_STRING:
  case H_STRING_ALT:
    temp = read_string(false);
    rb_hash_aset(hash, read_string(true), temp);
    break;
  }

  read_n_hash_pairs(hash, num-1);
}

void
read_n_array_entries(VALUE array, int num)
{
  extern unsigned char *serialized;
  check_pointer(serialized);
  
  if (num == 0) {
    return;
  }
  
  VALUE item = Qnil;
  int type = *serialized++;

  switch(type) {
  case O_STR:
    item = read_string(true);
    break;
  }

  rb_ary_push(array, item);
  
  read_n_array_entries(array, num-1);
}


VALUE
read_string(bool extended_size)
{
  extern unsigned char *serialized;
  check_pointer(serialized);
  
  int size = extended_size ? read_extended_size() : read_compact_size();
  int actual_size = 0;
  int rem;
  char *tp = serialized;

  if (size == 319) { // apparently Storable uses \000\000\001\077 to mean "read until n<10"
    while (*tp++ >= 10) {
      check_pointer(tp);
      actual_size++;
    }
    size = actual_size;
  }
  rem = size;
  
  char *np = malloc(size * sizeof(char) + 1);
  char *cnp = np;
  
  while (rem > 0) {
    check_pointer(serialized);
    rem--;
    *cnp++ = *serialized++;
  }

  *cnp++ = '\0';

  return rb_str_new(np, size);
}

/*
 * Extended sizes are given as [w,x,y,z], where the size is 256*y + z.
 */
int
read_extended_size()
{
  extern unsigned char *serialized;
  int size = 0;
  check_pointer(serialized);
  
  serialized++;
  serialized++;
  size += 256*(*serialized++);
  size += *serialized++;

  return size;
}

/*
 * Just one byte.
 */
int
read_compact_size() {
  check_pointer(serialized);
  extern unsigned char *serialized;
  return *serialized++;
}


void
Init_rcstorable()
{
  VALUE mRcstorable = rb_define_module("RCStorable");
  rb_define_singleton_method(mRcstorable, "thaw", thaw, 1);
}
