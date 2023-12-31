#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "section.h"
#include "hlxa.h"

void
print_table_header(void) {
	printf("\"Procedure\",\"Description\",\"Result\"\n");
}


void
test_hlxa_assemble_line_01(void) {
	section_t expected;
	section_t actual;
	hlxa_t hlxa;
	bool equal = false;

	printf("hlxa_assemble_line,DC X'01',");

	expected = section_new();
	if(!expected) goto bail;
	section_append_byte(expected, 0x01);

	actual = section_new();
	if(!actual) goto bail;

	hlxa = hlxa_new();
	if(!hlxa) goto bail;

	hlxa_set_section(hlxa, actual);
	hlxa_assemble_line(hlxa, "X'01'");

  equal = section_compare_eq(expected, actual);

bail:
	hlxa_free(&hlxa);
	section_free(&actual);
	section_free(&expected);

	if(equal)  printf("PASS\n");
	else       printf("FAIL\n");
}


void
test_hlxa_assemble_line_02(void) {
	section_t expected;
	section_t actual;
	hlxa_t hlxa;
	bool equal = false;

	printf("hlxa_assemble_line,DC X'0123',");

	expected = section_new();
	if(!expected) goto bail;
	section_append_byte(expected, 0x01);
	section_append_byte(expected, 0x23);

	actual = section_new();
	if(!actual) goto bail;

	hlxa = hlxa_new();
	if(!hlxa) goto bail;

	hlxa_set_section(hlxa, actual);
	hlxa_assemble_line(hlxa, "X'0123'");

  equal = section_compare_eq(expected, actual);

bail:
	hlxa_free(&hlxa);
	section_free(&actual);
	section_free(&expected);

	if(equal)  printf("PASS\n");
	else       printf("FAIL\n");
}


void
test_hlxa_assemble_line_03(void) {
	section_t expected;
	section_t actual;
	hlxa_t hlxa;
	bool equal = false;

	printf("hlxa_assemble_line,DC X'01234567',");

	expected = section_new();
	if(!expected) goto bail;
	section_append_byte(expected, 0x01);
	section_append_byte(expected, 0x23);
	section_append_byte(expected, 0x45);
	section_append_byte(expected, 0x67);

	actual = section_new();
	if(!actual) goto bail;

	hlxa = hlxa_new();
	if(!hlxa) goto bail;

	hlxa_set_section(hlxa, actual);
	hlxa_assemble_line(hlxa, "X'01234567'");

  equal = section_compare_eq(expected, actual);

bail:
	hlxa_free(&hlxa);
	section_free(&actual);
	section_free(&expected);

	if(equal)  printf("PASS\n");
	else       printf("FAIL\n");
}


void
test_hlxa_assemble_line_04(void) {
	section_t expected;
	section_t actual;
	hlxa_t hlxa;
	bool equal = false;

	printf("hlxa_assemble_line,DC X'0123456',");

	expected = section_new();
	if(!expected) goto bail;

	actual = section_new();
	if(!actual) goto bail;

	hlxa = hlxa_new();
	if(!hlxa) goto bail;

	hlxa_set_section(hlxa, actual);
	hlxa_assemble_line(hlxa, "X'0123456'");
	// The above is a syntax error; so we expect an
	// empty section due to an error.

  equal = section_compare_eq(expected, actual);

bail:
	hlxa_free(&hlxa);
	section_free(&actual);
	section_free(&expected);

	if(equal)  printf("PASS\n");
	else       printf("FAIL\n");
}


void
test_hlxa_assemble_line_05(void) {
	section_t expected;
	section_t actual;
	hlxa_t hlxa;
	bool equal = false;

	printf("hlxa_assemble_line,DC '01234567',");

	expected = section_new();
	if(!expected) goto bail;

	actual = section_new();
	if(!actual) goto bail;

	hlxa = hlxa_new();
	if(!hlxa) goto bail;

	hlxa_set_section(hlxa, actual);
	hlxa_assemble_line(hlxa, "'01234567'");
	// The above is a syntax error; so we expect an
	// empty section due to an error.

  equal = section_compare_eq(expected, actual);

bail:
	hlxa_free(&hlxa);
	section_free(&actual);
	section_free(&expected);

	if(equal)  printf("PASS\n");
	else       printf("FAIL\n");
}


void
test_hlxa_assemble_line_06(void) {
	section_t expected;
	section_t actual;
	hlxa_t hlxa;
	bool equal = false;

	printf("hlxa_assemble_line,DC X'01234567\\\",");

	expected = section_new();
	if(!expected) goto bail;

	actual = section_new();
	if(!actual) goto bail;

	hlxa = hlxa_new();
	if(!hlxa) goto bail;

	hlxa_set_section(hlxa, actual);
	hlxa_assemble_line(hlxa, "X'01234567\"");
	// The above is a syntax error; so we expect an
	// empty section due to an error.

  equal = section_compare_eq(expected, actual);

bail:
	hlxa_free(&hlxa);
	section_free(&actual);
	section_free(&expected);

	if(equal)  printf("PASS\n");
	else       printf("FAIL\n");
}


int
main(int argc, char *argv[]) {
	print_table_header();

	test_hlxa_assemble_line_01();
	test_hlxa_assemble_line_02();
	test_hlxa_assemble_line_03();
	test_hlxa_assemble_line_04();
	test_hlxa_assemble_line_05();
	test_hlxa_assemble_line_06();
}
