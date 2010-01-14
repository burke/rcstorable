#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ruby.h"

VALUE thaw(VALUE, VALUE);
static VALUE read_object();
static int read_extended_size();
static int read_compact_size();
static void read_n_hash_pairs(VALUE, int);
static void read_n_array_entries(VALUE, int);
static VALUE read_string(bool);
static void read_magic_numbers();

enum states
{
  READ_EXTENDED_SIZE,
  READ_COMPACT_SIZE,
  READ_NEW_OBJECT
};

enum perl_types
{
  PT_HASH_KEY   = 1,
  PT_ARRAY      = 2,
  PT_HASH       = 3,
  PT_VECTOR     = 4,
  PT_UNDEF      = 5,
  PT_STRING     = 10,
  PT_STRING_ALT = 23
};

// Used globally. Raptors. I know.
static unsigned char *serialized;
static unsigned char *serialized_end;

static void
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

static void
read_magic_numbers()
{
  serialized++;
  serialized++;
}

static VALUE
read_object()
{
  extern unsigned char *serialized;
  check_pointer(serialized);
  int type = *serialized++;
  int size = read_extended_size();

  VALUE object;

  switch(type) {
  case PT_HASH:
    object = rb_hash_new();
    read_n_hash_pairs(object, size);
    break;
  case PT_ARRAY:
    object = rb_ary_new();
    read_n_array_entries(object, size);
    break;
  case PT_UNDEF:
    object = Qnil;
    break;
  }
  
  return object;
}

  
static void
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
  case PT_UNDEF:
    rb_hash_aset(hash, read_string(true), Qnil);
    break;
  case PT_VECTOR:
    temp = read_object();
    str = read_string(true);
    rb_hash_aset(hash, str, temp);
    break;
  case PT_STRING:
  case PT_STRING_ALT:
    temp = read_string(false);
    rb_hash_aset(hash, read_string(true), temp);
    break;
  }

  read_n_hash_pairs(hash, num-1);
}

static void
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
  case PT_HASH_KEY:
    item = read_string(true);
    break;
  }

  rb_ary_push(array, item);
  
  read_n_array_entries(array, num-1);
}


/*
 * Given a size, read in a string of that size. Note that Storable seems to use 319 as a
 * magic value, meaning the string should be read until a very low character is found.
 * I should test this more specifically, but it's somewhere lower than "0", aka 48.
 */
static VALUE
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
 * This should really be read as a uint_32t, I guess.
 */
static int
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
static int
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
