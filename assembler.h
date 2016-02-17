#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

/*
 * "Copyright (c) 2014 by Fritz Sieker."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written
 * agreement is hereby granted, provided that the above copyright notice
 * and the following two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHOR
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND THE AUTHOR NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 */

/** @file assembler.h
 *  @brief Defines the interface to assembler.c functions (do not modify)
 *  @details This file defines the interface to a C file assembler.c that
 *  you will complete. This is the main portion of an assembler for LC3.
 *  <p>
 *  This is a substantial assignment. One way to attack larger projects is to
 *  break the problem into pieces and code those pieces individually. You
 *  have already done this for the files <code>field.c</code>, 
 *  <code>+.c</code> and
 *  <code>symbol.c</code>. Similarly, instead of writing large functions,
 *  decompose them into a series of calls to shorter functions which do
 *  required subtasks. The decomposition may be continued until each low level
 *  function is easy to write correctly. If you find any one function getting
 *  "too" large, think about what the code does and decompose it into several
 *  smaller functions. In fact, if you can give the task a good symbolic name,
 *  you can postpone writing it until later. This is because the name
 *  implies "what" the function does. "How" it is done, will be determined when
 *  that function is coded. It is perfectly fine to have methods that are only
 *  called once from the code. The idea is that each of these functions 
 *  peform a straight forward subtask. And it is perfectly fine to have
 *  functions that are only a few lines of code.
 *  <p>
 *  Most of the methods documented here are suggestions for abstractions that
 *  perform, small, useful tasks. They are for your convenience. By creating
 *  more abstractions, you will find the code easier to write and debug.
 *  Only the function with the prefix <code>asm_</code> are called from the
 *  main program.
 *  These helper methods should be <code>static</code>, but are declared here
 *  for documentation purposes.
 *  <p>
 *  @author Fritz Sieker
 */

#include "lc3.h"

#include "symbol.h"

#ifndef LC3AS_VAR
#define LC3AS_VAR extern
#endif

/** Error messages passed to function <code>asm_error()</code> */
#define ERR_OPEN_READ       "could not open '%s' for reading."
#define ERR_OPEN_WRITE      "could not open '%s' for writing."
#define ERR_LINE_TOO_LONG   "source line too long (max is %d)"
#define ERR_NO_ORIG         "no .ORIG directive found"
#define ERR_NO_END          "no .END directive found"
#define ERR_ORIG_NOT_1ST    "instruction(s) appear before .ORIG"
#define ERR_END_NOT_LAST    "instruction(s) appear after .END"
#define ERR_MISSING_ORIG    ".ORIG not found"
#define ERR_MISSING_END     ".END not found"
#define ERR_EXPECTED_COMMA  "expected comma, got '%s'"
#define ERR_EXPECTED_REG    "expected register (R0-R7), got '%s'"
#define ERR_EXPECT_REG_IMM  "expected register or immediate, got '%s'"
#define ERR_BAD_LABEL       "label '%s' contains illegal characters"
#define ERR_MISSING_OP      "expected LC3 op, got '%s'"
#define ERR_MISSING_OPERAND "too few operand(s)"
#define ERR_EXTRA_OPERAND   "extra operand(s) '%s'"
#define ERR_DUPLICATE_LABEL "label '%s' previosly defined"
#define ERR_MISSING_LABEL   "label '%s' never defined"
#define ERR_BAD_PCOFFSET    "PCoffset to '%s' out of range"
#define ERR_BAD_IMM         "immediate '%s' (bad format)"
#define ERR_IMM_TOO_BIG     "immediate '%s' out of range"
#define ERR_EXPECTED_STR    "expected quoted string, got '%s'"
#define ERR_BAD_STR         "unterminated string '%s'"

/** A global variable defining the line in the source file */
LC3AS_VAR int srcLineNum;

/** A global variable defining the LC3 address of the current instruction */
LC3AS_VAR int currAddr;

/** A global variable defining the number of errors found */
LC3AS_VAR int numErrors;

/** Typedef of structure type */
typedef struct line_info line_info_t;

/** Structure containing all the values that might be found in a source line.
 *  The contents of this structure are set during <code>pass_one()</code> and
 *  used to build <code>machineCode</code> during <code>pass_two</code>
 */
struct line_info {
  line_info_t* next;         /**< Allow for linked list                   */
  int          lineNum;      /**< Line number in source code              */
  int          address;      /**< LC3 address of instruction              */
  int          machineCode;  /**< The 16 bit LC3 instruction              */
  opcode_t     opcode;       /**< opcode of instruction                   */
  int          form;         /**< which form of instruction (ADD/ADDI)    */
  int          reg1;         /**< DR or SR, if present                    */
  int          reg2;         /**< SR1 or BaseR, if present                */
  int          reg3;         /**< SR2, if present                         */
  int          immediate;    /**< Immediate value if present              */
  char*        reference;    /**< Label referenced by instruction, if any */
};


/** A function to print error messages. This function takes a minimum of one
 *  parameter. It is exaclty like the <code>printf()</code> function. The first
 *  parameter is a formatting string. The remaining parameters (if any) are
 *  the actual values to be printed. It must be used for reporting all errors.
 *  Do not modify.
 *  @param msg - the formating string
 */
void asm_error (char* msg, ...);

/** Do whatever initialization is necessary for the assembler */
void asm_init (void);

/** A function to initialize all the fields of the structure to default values
 *  The lineNum and address fields are initialized on return. All other fields
 *  are set to "default" values.
 *  Do not modify.
 * @param info - pointer to information about a source line
 */
void asm_init_line_info (line_info_t* info);

/** This function performs the processing required in the first pass. At a
 *  minimum, it must check the syntax of each instruction and create the symbol
 *  table. For this assignment, it will also create a data structure for use
 *  by the second pass. The flow of this function is:
 *  <ol>
 *  <li>open the source file and report an error (think about your convenience
 *      functions)</li>
 *  <li>read the lines one at a time using <code>fgets()</code>. The function
 *      <code>fgets()</code> takes advantage of C's ability to return multiple
 *      values in a single call by using pointers. The values it reads are
 *      stored in the character buffer passed as the first argument.
 *      Success/failure is indicated by the actual return value.</li>
 *  <li>convert the line to a list of tokens</li>
 *  <li>if there is an opcode on the line, then</li>
 *  <ol>
 *     <li>allocate a new <code>line_info_t</code> store it in the
 *         global variable <code>currInfo</code> and initialize it</li>
 *     <li>convert the tokens to values and set the appropriate fields
 *         of <code>currInfo</code></li>
 *     <li>add it to the linked list defined by <code>infoHead</code> and
 *         <code>infoTail</code></li>
 *     <li>update the current address</li>
 *  </ol>
 *  <li>If there were no errors, write the symbol table file using
 *      <code>lc3_write_sym_tab()</code>.</li>
 *      </li>
 *  </ol>
 *  @param asm_file_name - name of the file to assemble
 *  @param sym_file_name - name of the symbol table file
 */
void asm_pass_one (char* asm_file_name, char* sym_file_name);

/** This function generates the object file. It is only called if no errors were
 *  found during <code>asm_pass_one()</code>. The basic structure of this code
 *  is to loop over the data structure created in <code>asm_pass_one()</code>,
 *  generate object code (16 bit LC3 instructions) and write it to the object
 *  file.
 *  @param obj_file_name - name of the object file for this source code
 */
void asm_pass_two(char* obj_file_name);

/** A function to print the infomation extracted from a source line. This is
 *  used for debugging.
 *  Do not modify.
 *  @ info - pointer to informaton about a source line
 */
void asm_print_line_info (line_info_t* info);

/** Initialze everything used by the assembler */
void asm_init (void);

/** Cleanup everything used by the assembler */
void asm_term (void);

/** A function to check if the token is a label. In LC3 assembly language,
 *  the optional label may preceed the opcode. Therefore, if the token is
 *  <b>not</b> a valid opcode, assume it is label. If it is a valid label,
 *  add it to the symbol table. Report any errors found using
 *  <code>asm_error()</code>.
 *  @param token - the token to consider
 *  @return If the token <b>is</b> a label, then return the <b>next</b> token.
 *  Otherwise, return the parameter.
 */
char* check_for_label (char* token);

/** A function to check the syntax of a source line. At the conclusion of this
 *  function, the appropriate fields of the global <code>currInfo</code> are
 *  initialized. The basic flow of the function is:
 *  <ol>
 *  <li>determine if the first token is a label</li>
 *  <li>convert the first/next token to an opcode and store it into your
 *      data structure. It is legal for an
 *      LC3 statemet to have only a label. This will be the case if the
 *      value returned by <code>check_for_label()</code> is
 *	<code>NULL</code>.</li>
 *  <li>determine the list of operands corresponding to this
 *      opcode. How can you get this from the <code>lc3</code> module?</li>
 *  <li>if this opcode has operands associated with it then
 *         call <code>scan_operands()</code></li>
 *  <li>make sure there are no extra operands</li>
 *  </ol> 
 *  @param token - the first token on the line. This could be a label or an
 *  operator (e.g. <code>ADD</code> or <code>.FILL</code>).
 */
void check_line_syntax (char *token);

/** A second pass function to take one field from the <code>currInfo</code>
 *  structure and place it in the <code>machineCode<field>. The flow of
 *  this function mirrors the code of pass one function
 *  <code>get_operand()</code>.
 *  @param operand - the type of operand
 */
void encode_operand (operand_t operand);

/** This second pass function is used to convert the reference into a PC offset.
 *  There are several errors that may occur. The reference may not occur in
 *  the symbol table, or the offset may be out of range. If successful, this
 *  function puts the PC offset in the <code>machineCode</code> field of
 *  <code>currInfo</code>.
 *  @param width - the number of bits that hold the PC offset
 */
void encode_PC_offset_or_error (int width);

/** A convenience function to make sure the next token is a comma and report
 *  an error if it is not.
 */
void get_comma_or_error (void);

/** A convenience function to convert an token to an immediate value. Used for
 *  the imm5/offset6/trapvect8/.ORIG values. The value is obtained by calling a
 *  function provided for you in the <code>lc3</code> module. If
 *  the value is not in the correct format, or out of range, report an error
 *  using <code>lc3_as_error()</code>. If it is good, store it in the
 *  <code>immediate</code> field of <code>currInfo</code>
 *  @param token - the string to be converted to an immediate
 *  @param width - how many bits are used to store the value
 *  @param isSigned - specifies if number is signed or unsigned
 */
void get_immediate_or_error (char* token, int width, int isSigned);

/** A convenience function to get the label reference used in the
 *  <code>BR/LD/LDI/LEA/ST/STI/JSR</code> instructions.
 *  The code should make sure it is a
 *  valid label. If it is valid, store it in the <code>reference</code> field of
 *  <code>currInfo</code>. If is not a valid label, report an error. You should
 *  understand why this routine may not be able to directly calculate the
 *  PCoffset.
 *  @param token - the reference to check
 */
void get_PC_offset_or_error (char* token);

/** A convenience function to convert the token to a value and store it in
 *  the <code>currInfo</code> data structure.
 *  @param operand - the type of operand that is expected
 *  @param token - the token to be converted
 */
void get_operand (operand_t operand, char* token);

/** A function to convert a string to a register number and report an error
 *  if the string does <b>not</b> represent an register.  Use the function
 *  <code>asm_error()</code> to report errors.
 *  @param token - the string to convert to a register
 *  @return the register number, or -1 on an error
 */
int get_reg_or_error (char* token);

/** Open file for reading and report an error on failure. Use the C function
 *  <code>fopen()</code> and report errors using <code>asm_error()</code>.
 *  @param file_name - name of file to open
 *  @return the file or NULL on error
 */
FILE *open_read_or_error (char* file_name);

/** Open file for writing and report an error on failure. Use the C function
 *  <code>fopen()</code> and report errors using <code>asm_error()</code>.
 *  @param file_name - name of file to open
 *  @return the file or NULL on error
 */
FILE *open_write_or_error (char* file_name);

/** A convenience function to scan all the operands of an LC3 instruction.
 *  The basic flow of this function is:
 *  <ol>
 *  <li>initialize a count of the number of operands expected</li>
 *  <li>initialize a count of the number of errors found before this
 *      function was called.</li>
 *  <li>loop over the all the possible operand types. if the operand type is
 *  used by this LC3 instuction, do the following
 *  <ul>
 *  <li>get the next token and convert it to an operand
 *  <li>On error, simply return which results in continuing with the next
 *      source line. The errors include an operand of the wrong type,
 *      or a missing operand.</li>
 *  <li>If there are additional operands, continue to scan them. Look
 *      at what <code>testTokens</code> returns for a hint on what this
 *      means.</li>
 *  </ul>
 *  </li>
 *  </ol>
 *  @param operands - a "list" of the operands for the current LC3 instruction
 *  encoded as individual bits in an integer.
 */
void scan_operands (operands_t operands);

/** This function is responsible for determing how much space and LC3 
 *  instruction or pseudo-op will take and updating the static global variable
 *  <code>currAddr</code> defined in the file <code>assembler.c</code>.
 *  Most LC3 instructions require a single word. However, there are several
 *  exceptions:
 *  <ul>
 *  <li>a line containing only a label does not change <code>currAddr</code></li>
 *  <li><code>.ORIG</code> replaces the value of <code>currAddr</code> by
 *  its operand</li>
 *  <li><code>.BLKW</code> uses the number of words specified by its operand</li>
 *  <li><code>.STRINGZ</code> uses the length of the string minus one words.
        Make sure you understand how many word(s) a string uses.</li>
 *  <li>Some pseudo ops may expand to multiple LC3 instructions. For example,
 *  if the was a <code>.PUSH</code> pseudo-op, it would use two words.
 *  </ul>
 */
void update_address (void);

#endif
