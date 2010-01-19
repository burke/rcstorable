#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ruby.h"

typedef unsigned char uchar;

VALUE thaw(VALUE, VALUE);
static VALUE read_object();
static VALUE read_boolean();
static uint32_t read_32_bit_integer();
static uint32_t read_compact_size();
static void read_n_hash_pairs(VALUE, uint32_t);
static void read_n_array_entries(VALUE, uint32_t);
static VALUE read_string(bool);
static void read_magic_numbers();
static void check_pointer(uchar*);

enum perl_types
{
  PT_HASH_KEY   = 1,
  PT_ARRAY      = 2,
  PT_HASH       = 3,
  PT_VECTOR     = 4,
  PT_UNDEF      = 5,
  PT_BOOLEAN    = 8,
  PT_INT32      = 9,
  PT_STRING     = 10,
  PT_STRING_ALT = 23
};

// Used globally. Raptors. I know.
static uchar *serialized;
static uchar *serialized_end;

/*
 * Given a perl Storable frozen blob, decode it into a ruby data structure.
 */
VALUE
thaw(VALUE self, VALUE str)
{
  Check_Type(str, T_STRING);
  extern uchar *serialized, *serialized_end;

  serialized = RSTRING_PTR(str);
  serialized_end = serialized + RSTRING_LEN(str);
  
  read_magic_numbers();
  
  return read_object();
}

/*
 * Malformed strings can theoretically cause segfaults. Segfaults are bad.
 * We'll check pretty much everything we do against the pre-computed end-of-string.
 */
static void
check_pointer(uchar *ptr)
{
  extern uchar *serialized_end;
  if (ptr > serialized_end) {
    rb_raise(rb_eRangeError, "malformed data");
  }
}

/*
 * Certain test cases start with \005\006. Other ones don't.
 * This will need to be fixed eventually.
 */
static void
read_magic_numbers()
{
  extern uchar *serialized;
  check_pointer(serialized+1);
  serialized += 2;
}

/*
 * Figure out what type of object is at the front of serialized, read it in, potentially
 * recursively creating several other sub-objects in the process, and return it.
 */
static VALUE
read_object()
{
  extern uchar *serialized;
  check_pointer(serialized);
  uint32_t type = *serialized++;
  uint32_t size;

  VALUE object;

  switch(type) {
  case PT_UNDEF:
    object = Qnil;
    break;
  case PT_HASH:
    object = rb_hash_new();
    size = read_32_bit_integer();
    read_n_hash_pairs(object, size);
    break;
  case PT_INT32:
    object = read_32_bit_integer();
    break;
  case PT_ARRAY:
    object = rb_ary_new();
    size = read_32_bit_integer();
    read_n_array_entries(object, size);
    break;
  case PT_BOOLEAN:
    object = read_boolean();
    break;
  case PT_STRING:
  case PT_STRING_ALT:
    object = read_string(false);
    break;
  case PT_VECTOR:
    object = read_object(); // This is a marker we can just ignore...
    break;
  }
  
  return object;
}

  
/*
 * We've already created a hash, and read the size of it.
 * Now we need to read in n items, and add them to the hash.
 */
static void
read_n_hash_pairs(VALUE hash, uint32_t num)
{
  if (num == 0) { return; }
  VALUE temp = read_object();
  rb_hash_aset(hash, read_string(true), temp);
  read_n_hash_pairs(hash, num-1);
}

static VALUE
read_boolean()
{
  extern uchar *serialized;
  check_pointer(serialized);
  return (*serialized++ == 128) ? Qfalse : Qtrue;
}

/*
 * We've already created an array, and read the size of it.
 * Now we need to read in n items, and add them to the array.
 */
static void
read_n_array_entries(VALUE array, uint32_t num)
{
  if (num == 0) { return; }
  rb_ary_push(array, read_object());
  read_n_array_entries(array, num-1);
}


/*
 * Given a size, read in a string of that size. Note that Storable seems to use 319 as a
 * magic value, meaning the string should be read until a very low character is found.
 * I should test this more specifically, but it's somewhere lower than "\t", aka 7.
 */
static VALUE
read_string(bool extended_size)
{
  extern uchar *serialized;
  check_pointer(serialized);

  uint32_t size = extended_size ? read_32_bit_integer() : read_compact_size();

  uint32_t actual_size = 0;
  uint32_t rem;
  uchar *tp = serialized;

  if (size == 319) { // apparently Storable uses \000\000\001\077 to mean "read until n<7"
    while (*tp++ >= 7) {
      check_pointer(tp);
      actual_size++;
    }
    size = actual_size;
  }
  rem = size;
  
  uchar *np = ALLOC_N(char, size+1);
  uchar *cnp = np;
  
  check_pointer(serialized+rem-1);
  while (rem > 0) {
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
static uint32_t
read_32_bit_integer()
{
  extern uchar *serialized;

  uint32_t size = 0;

  check_pointer(serialized+3);

  // I don't want to deal with byte-order. This is just easier. 
  size += (*serialized++)*16777216;
  size += (*serialized++)*65536;
  size += (*serialized++)*256;
  size += (*serialized++);

  return size;
}

/*
 * Just one byte.
 */
static uint32_t
read_compact_size() {
  extern uchar *serialized;
  check_pointer(serialized);
  return *serialized++;
}


void
Init_rcstorable()
{
  VALUE mRcstorable = rb_define_module("RCStorable");
  rb_define_singleton_method(mRcstorable, "thaw", thaw, 1);
}
