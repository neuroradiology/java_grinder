/**
 *  Java Grinder
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2014-2015 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "MC6809.h"

#define LOCALS(i) (i * 2)

#define CHECK_INT16(a) \
  if (a > 65535 || a < -32768) \
  { \
    printf("Error: literal value %d bigger than 16 bit.\n", a); \
    return -1; \
  }

// ABI is:

//                        ==     !=     yum    <=      >     >=
const char *branch[] = { "beq", "bne", "blt", "ble", "bgt", "bge" };

MC6809::MC6809() :
  start_org(0),
  ram_start(0x0600),
  ram_end(0x06ff),
  reg(0),
  reg_max(9),
  stack(0),
  is_main(0)
{

}

MC6809::~MC6809()
{
}

int MC6809::open(const char *filename)
{
  if (Generator::open(filename) != 0) { return -1; }

  fprintf(out, ".6809\n");

  // Set where RAM starts / ends
  fprintf(out, "ram_start equ 0x%04x\n", ram_start);
  fprintf(out, "ram_end equ 0x0x%04x\n", ram_end);

  return 0;
}

int MC6809::start_init()
{
  // Add any set up items (stack, registers, etc).
  fprintf(out, ".org 0x%04x\n", start_org);
  fprintf(out, "start:\n");
  fprintf(out, "  lds #0x%04x ; Set SP stack pointer\n", ram_end + 1);

  return 0;
}

int MC6809::insert_static_field_define(const char *name, const char *type, int index)
{
  return -1;
}

int MC6809::init_heap(int field_count)
{
  fprintf(out, "  ;; Set up heap and static initializers\n");
  //fprintf(out, "  mov #ram_start+%d, &ram_start\n", (field_count + 1) * 2);

  return -1;
}

int MC6809::insert_field_init_boolean(char *name, int index, int value)
{

  return -1;
}

int MC6809::insert_field_init_byte(char *name, int index, int value)
{
  return insert_field_init_int(name, index, value);
}

int MC6809::insert_field_init_short(char *name, int index, int value)
{
  return insert_field_init_int(name, index, value);
}

int MC6809::insert_field_init_int(char *name, int index, int value)
{
  return -1;
}

int MC6809::insert_field_init(char *name, int index)
{
  return -1;
}

void MC6809::method_start(int local_count, int max_stack, int param_count, const char *name)
{
  //is_main = (strcmp(name, "main") == 0) ? 1 : 0;

  fprintf(out, "%s:\n", name);

  fprintf(out, "  tfr s,d\n");
  fprintf(out, "  subd #%d\n", local_count * 2);
  fprintf(out, "  tfr d,s\n");
  fprintf(out, "  pshs u\n");    // really only needed when not main()
  fprintf(out, "  tfr s,u\n");
}

void MC6809::method_end(int local_count)
{
  fprintf(out, "  puls u\n");    // really only needed when not main()
  fprintf(out, "  tfr s,d\n");
  fprintf(out, "  addd #%d\n", local_count * 2);
  fprintf(out, "  tfr d,s\n");
  fprintf(out, "\n");
}

int MC6809::push_integer(int32_t n)
{
  CHECK_INT16(n);

  fprintf(out, "  ldd #0x%04x\n", n & 0xffff);
  fprintf(out, "  pshs a,b\n");

  return 0;
}

int MC6809::push_integer_local(int index)
{
  fprintf(out, "  ldd %d,u\n", LOCALS(index));
  fprintf(out, "  pshs a,b\n");
  return 0;
}

int MC6809::push_ref_static(const char *name, int index)
{
  return -1;
}

int MC6809::push_ref_local(int index)
{
  return push_integer_local(index);
}

int MC6809::push_fake()
{
  fprintf(out, "  pshs x\n");
  return 0;
}

int MC6809::push_long(int64_t n)
{
  return -1;
}

int MC6809::push_float(float f)
{
  return -1;
}

int MC6809::push_double(double f)
{
  return -1;
}

int MC6809::push_byte(int8_t b)
{
  int32_t value = (int32_t)b;

  return push_integer(value);
}

int MC6809::push_short(int16_t s)
{
  int32_t value = (int32_t)s;

  return push_integer(value);
}

int MC6809::push_ref(char *name)
{
  // Need to move the address of name to the top of stack
  return -1;
}

int MC6809::pop_integer_local(int index)
{
  fprintf(out, "  ; pop_integer_local()\n");
  fprintf(out, "  puls a,b\n");
  fprintf(out, "  std %d,u\n", LOCALS(index));
  return 0;
}

int MC6809::pop_ref_local(int index)
{
  return pop_integer_local(index);
}

int MC6809::pop()
{
  fprintf(out, "  ; pop()\n");
  fprintf(out, "  puls x\n");
  return 0;
}

int MC6809::dup()
{
  fprintf(out, "  ; dup()\n");
  fprintf(out, "  puls x\n");
  fprintf(out, "  pshs x\n");
  fprintf(out, "  pshs x\n");
  return 0;
}

int MC6809::dup2()
{
  fprintf(out, "  ; dup2()\n");
  fprintf(out, "  puls x\n");
  fprintf(out, "  puls y\n");
  fprintf(out, "  pshs x\n");
  fprintf(out, "  pshs y\n");
  fprintf(out, "  pshs x\n");
  fprintf(out, "  pshs y\n");
  return 0;
}

int MC6809::swap()
{
  fprintf(out, "  ; swap()\n");
  fprintf(out, "  puls x\n");
  fprintf(out, "  puls y\n");
  fprintf(out, "  pshs y\n");
  fprintf(out, "  pshs x\n");
  return 0;
}

int MC6809::add_integer()
{
  fprintf(out, "  ; add_integer()\n");
  fprintf(out, "  puls a,b\n");
  fprintf(out, "  addd ,s\n");
  fprintf(out, "  std ,s\n");
  return 0;
}

int MC6809::add_integer(int num)
{
  CHECK_INT16(num);
  fprintf(out, "  ; add_integer()\n");
  fprintf(out, "  ldd #0x%04x\n", num & 0xffff);
  fprintf(out, "  addd ,s\n");
  fprintf(out, "  std ,s\n");
  return 0;
}

int MC6809::sub_integer()
{
  fprintf(out, "  ; sub_integer()\n");
  fprintf(out, "  puls a,b\n");
  fprintf(out, "  subd ,s\n");
  fprintf(out, "  std ,s\n");
  return 0;
}

int MC6809::sub_integer(int num)
{
  CHECK_INT16(num);
  fprintf(out, "  ; sub_integer()\n");
  fprintf(out, "  ldd #0x%04x\n", num & 0xffff);
  fprintf(out, "  subd ,s\n");
  fprintf(out, "  std ,s\n");
  return 0;
}

int MC6809::mul_integer()
{
  return -1;
}

int MC6809::div_integer()
{
  return -1;
}

int MC6809::mod_integer()
{
  return -1;
}

int MC6809::neg_integer()
{
  fprintf(out, "  ; neg_integer()\n");
  fprintf(out, "  neg ,s\n");
  //fprintf(out, "  puls a,b\n");
  //fprintf(out, "  neg d\n");
  //fprintf(out, "  pshs a,b\n");
  return 0;
}

int MC6809::shift_left_integer()
{
  return -1;
}

int MC6809::shift_left_integer(int num)
{
  return -1;
}

int MC6809::shift_right_integer()
{
  return -1;
}

int MC6809::shift_right_integer(int num)
{
  return -1;
}

int MC6809::shift_right_uinteger()
{
  return -1;
}

int MC6809::shift_right_uinteger(int num)
{
  return -1;
}

int MC6809::and_integer()
{
  return -1;
}

int MC6809::and_integer(int num)
{
  return -1;
}

int MC6809::or_integer()
{
  return -1;
}

int MC6809::or_integer(int num)
{
  return -1;
}

int MC6809::xor_integer()
{
  return -1;
}

int MC6809::xor_integer(int num)
{
  return -1;
}

int MC6809::inc_integer(int index, int num)
{
  fprintf(out, "  ; inc_integer() index=%d\n", index);
  fprintf(out, "  ldd %d,u\n", LOCALS(index));

  return 0;
}

int MC6809::integer_to_byte()
{
  fprintf(out, "  ; integer_to_byte()\n");
  fprintf(out, "  puls a,b\n");
  fprintf(out, "  sex ; (oh yeah, harder!)\n");
  fprintf(out, "  pshs a,b\n");
  return 0;
}

int MC6809::integer_to_short()
{
  fprintf(out, "  ; integer_to_short() - do nothing\n");
  return 0;
}

int MC6809::jump_cond(const char *label, int cond, int distance)
{
  bool use_long = distance > 5;

  fprintf(out, "  ; jump_cond()\n");
  fprintf(out, "  puls a,b\n");
  fprintf(out, "  cmpd #0\n");
  fprintf(out, "  %s%s %s\n", use_long ? "l":"", branch[cond], label);

  return 0;
}

int MC6809::jump_cond_integer(const char *label, int cond, int distance)
{
  bool use_long = distance > 5;

  fprintf(out, "  ; jump_cond()\n");
  fprintf(out, "  puls a,b\n");
  fprintf(out, "  puls y\n");
  fprintf(out, "  cmpd -2,s\n");
  fprintf(out, "  %s%s %s\n", use_long ? "l":"", branch[cond], label);

  return 0;
}

int MC6809::return_local(int index, int local_count)
{
  return -1;
}

int MC6809::return_integer(int local_count)
{
  return -1;
}

int MC6809::return_void(int local_count)
{
  return -1;
}

int MC6809::jump(const char *name, int distance)
{
  if (distance < 20)
  {
    fprintf(out, "  bra %s\n", name);
  }
    else
  {
    fprintf(out, "  jmp %s\n", name);
  }

  return 0;
}

int MC6809::call(const char *name)
{
  return -1;
}

int MC6809::invoke_static_method(const char *name, int params, int is_void)
{
  return -1;
}

int MC6809::put_static(const char *name, int index)
{
  return -1;
}

int MC6809::get_static(const char *name, int index)
{
  return -1;
}

int MC6809::brk()
{
  return -1;
}

int MC6809::new_array(uint8_t type)
{
  return -1;
}

int MC6809::insert_array(const char *name, int32_t *data, int len, uint8_t type)
{
  if (type == TYPE_BYTE)
  { return insert_db(name, data, len, TYPE_INT); }
    else
  if (type == TYPE_SHORT)
  { return insert_dw(name, data, len, TYPE_INT); }
    else
  if (type == TYPE_INT)
  { return insert_dc32(name, data, len, TYPE_INT); }

  return -1;
}

int MC6809::insert_string(const char *name, uint8_t *bytes, int len)
{
  return -1;
}

int MC6809::push_array_length()
{
  return -1;
}

int MC6809::push_array_length(const char *name, int field_id)
{
  return -1;
}

int MC6809::array_read_byte()
{
  return -1;
}

int MC6809::array_read_short()
{
  return -1;
}

int MC6809::array_read_int()
{
  return -1;
}

int MC6809::array_read_byte(const char *name, int field_id)
{
  return -1;
}

int MC6809::array_read_short(const char *name, int field_id)
{
  return -1;
}

int MC6809::array_read_int(const char *name, int field_id)
{
  return -1;
}

int MC6809::array_write_byte()
{
  return -1;
}

int MC6809::array_write_short()
{
  return -1;
}

int MC6809::array_write_int()
{
  return -1;
}

int MC6809::array_write_byte(const char *name, int field_id)
{
  return -1;
}

int MC6809::array_write_short(const char *name, int field_id)
{
  return -1;
}

int MC6809::array_write_int(const char *name, int field_id)
{
  return -1;
}
