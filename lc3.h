#ifndef __LC3_H__
#define __LC3_H__

/*
 * "Copyright (c) 2014 by Fritz Sieker."
 * based on ideas/code by Steven S. Lumetta
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

/** @file lc3.h
 *  @brief definitions of the LC3 instruction set architecture (ISA) (do not modify)
 *  @details This defines the details of the LC3 instruction set architecture
 *  (ISA). It is a separate file so that it can be shared by both an assembler
 *  and a simulator. It also include various utility routines.
 * <p>
 * @author Fritz Sieker
 */

#include <stdio.h>

/** A handy way of declaring global variables in a header file */
#ifndef LC3_VAR
#define LC3_VAR extern
#endif

#include "symbol.h" 

/** Prototype for handy function to duplicate a string. This function allocates
 *  dynamic memory and copies the string into that memory. Thus, use of this
 *  function implies the need to free the string at some point.
 */
char* strdup(const char *);

/** LC3 words are 16 bits */
#define LC3_WORD unsigned short

/** The LC3 defines a memory accessed by a 16 bit address */
#define LC3_MEM_SIZE 65536

/** The LC3 contains 8 general purpose register, named R0..R7 */
#define LC3_NUM_REGS 8  

/** Return address stored in R7 */
#define RETURN_ADDR_REG   7

/** Global flag defining whether using .obj file or .hex file */
LC3_VAR int inHex;

/** Global variable holding symbol table */
LC3_VAR sym_table_t* lc3_sym_tab;

/** The LC3 opcodes and pseudo-ops. The codes of OP_BR .. OP_TRAP corresponds
 *  exactly to the numeric values assigned to the 16 LC3 instructions. The
 *  codes assigned to the pseudo-ops is arbitrary. PCi is the incremented PC
 */
typedef enum opcode {
  OP_INVALID = -1, /**< invalid opcode                                       */
  OP_BR,           /**< PC = PCi + PCoffset9 if condition is met             */
  OP_ADD,          /**< DR = SR1 + SR2 or DR = SR1 + imm5                    */
  OP_LD,           /**< DR = mem[PCi + PCoffset9]                            */
  OP_ST,           /**< mem[PCi + PCoffset9] = SR                            */
  OP_JSR_JSRR,     /**< R7 = PCi and (PC = SR or PC = PCi + PCoffest9)       */
  OP_AND,          /**< DR = SR1 & SR2                                       */
  OP_LDR,          /**< DR = mem[BaseR + offset6]                            */
  OP_STR,          /**< mem[BaseR + offset6] = SR                            */
  OP_RTI,          /**< PC = R7, exit supervisor mode                        */
  OP_NOT,          /**< DR = ~SR1                                            */
  OP_LDI,          /**< DR = mem[mem[PCi + PCoffset9]]                       */
  OP_STI,          /**< mem[mem[PCi + offset9]] = SR                         */
  OP_JMP_RET,      /**< PC = R7 (RET) or PC = Rx (JMP Rx)                    */
  OP_RESERVED,     /**< Currently not used                                   */
  OP_LEA,          /**< DR = PCi + PCoffset9                                 */
  OP_TRAP,         /**< R7 = PCi, PC = mem[mem[trap]], enter supervisor mode */
  OP_ORIG,         /**< memory location where code is loaded                 */
  OP_END,          /**< end of propgram - only comments may follow           */
  OP_BLKW,         /**< allocate N words of storage initialized with 0       */
  OP_FILL,         /**< allocate 1 word of storage initialed with operand    */
  OP_STRINGZ,      /**< allocate N+1 words of storage initialized with
                            string and null terminator (1 char per word)     */
  OP_GETC,         /**< Read character from keyboard, no echo      (trap x20)*/
  OP_OUT,          /**< Write one character                        (trap x21)*/
  OP_PUTS,         /**< Write null terminated string               (trap x22)*/
  OP_IN,           /**< Print prompt and read/echo character       (trap x23)*/
  OP_PUTSP,        /**< Write packed null terminated string        (trap x24)*/
  OP_HALT,         /**< Halt execution                             (trap x25)*/
  OP_GETS,         /**< Read string from keyboard, store in memory (trap x26)*/
  OP_ZERO,         /**< DR = 0 (AND DR,DR,#0)                                */
  OP_COPY,         /**< DR = SR1 (ADD DR,SR1,#0)                             */
  OP_NEG,          /**< DR = -DR (NOT DR,DR and ADD DR,DR,#1)                */
  NUM_OPCODES      /**< Initialized by compiler                              */
} opcode_t;

/** A bit field used to define the types of operands an individual
 *  LC3 instruction may have. Each value represents a different bit in
 *  the final result. When you see C code like this, it is likely that
 *  an integer value is used to represent an "array" of up to 32
 *  values. Each value is accessed with a mask that extracts the bit of
 *  interest. See the inst_format_t below.
 */
typedef enum operand {
    FMT_R1    = 0x001, /**< DR or SR                      */
    FMT_R2    = 0x002, /**< SR1 or BaseR                  */
    FMT_R3    = 0x004, /**< SR2                           */
    FMT_CC    = 0x008, /**< condition codes               */
    FMT_IMM5  = 0x010, /**< imm5                          */
    FMT_IMM6  = 0x020, /**< offset6                       */
    FMT_VEC8  = 0x040, /**< trapvect8                     */
    FMT_ASC8  = 0x080, /**< 8-bit ASCII                   */
    FMT_PCO9  = 0x100, /**< label (or address for imm9)   */
    FMT_PCO11 = 0x200, /**< label (or address for imm11)  */
    FMT_IMM16 = 0x400, /**< 16 bits for .FILL. or .ORIG   */
    FMT_STR   = 0x800  /**< operand is string literal     */
} operand_t;

/** Define a combinatin of operands an opcode may have. For example, the
 *  the LD, LDI, ST and STI instructions all have two parameters. The
 *  first is a register, the second is a nine bit offset. This form stores
 *  multiple values in a single integer values by using individual
 *  bits to encode information.
 *  <p>
 *  To determine the number of operands an instruction has, simply count the
 *  1 bits in the word. <a href="https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan">
 *  Here</a> is an algorithum by Brian Kernigan, one of the developers of Unix,
 *  and co-author of the famous book "The C Programing Language (known as K&R)".
 */
typedef enum operands {
  FMT_      = 0,
  FMT_RRR   = (FMT_R1 | FMT_R2 | FMT_R3),
  FMT_RRI5  = (FMT_R1 | FMT_R2 | FMT_IMM5),
  FMT_L     = FMT_PCO9,
  FMT_R     = FMT_R2,
  FMT_I11   = FMT_PCO11,
  FMT_RL    = (FMT_R1 | FMT_PCO9),
  FMT_RRI6  = (FMT_R1 | FMT_R2 | FMT_IMM6),
  FMT_RR    = (FMT_R1 | FMT_R2),
  FMT_V     = FMT_VEC8,
  FMT_A     = FMT_ASC8,
  FMT_16    = FMT_IMM16,
  FMT_S     = FMT_STR
} operands_t;

/** This structure stores the information for one form of an instruction.
 *  Several instructions have multiple forms, but most have only one.
 */
typedef struct inst_format {
 char*      name;      /**< human readable name (e.g. "ADD")           */
 operands_t operands;  /**< operands for this operation (e.g. FMT_RRR) */
 int        prototype; /**< bits that are constant in this instruction */
} inst_format_t;

/** This structure stores the information about a single instruction.
 *  See the usage in lc3.c where the information for each LC3 instruction
 *  is defined. The name of the seond form will be <code>NULL</code> unless
 *  the instruction actually has two forms.
 */
typedef struct LC3_inst {
  int           formBit;    /**< -1 if instruction has only one format,
                                 else bit that differentiates format.   */
  inst_format_t forms[2];   /**< syntax for both possible forms         */
} LC3_inst_t;

/** Convert an escape sequence to a single character
 *  @param c the character <b>after</b> the escape character (the back-slash)
 *  @return the ascaped value of the character
 */ 
char lc3_escaped_char (char c);

/** Get the information for an instruction, given its opcode This is simply
 *  an access into an array of values initialized with the information for
 *  each of the LC3's sixteen instructions and additional pseudo-ops.
 *  @param opcode - the opcode of the instruction/pseudo-op of interest
 *  @return - a pointer to the sytax information for this instruction or NULL
 */
LC3_inst_t* lc3_get_inst_info (opcode_t opcode);

/** Convert a operands_t to a "name"
 *  @param format - the value to convert
 *  @return - a human readable name
 */
const char* lc3_get_format_name (operands_t format);

/** convert an <code>opcode_t</code> to a name
 *  @param op - the opcode to convert
 *  @return - a human readable name
 */
const char* lc3_get_opcode_name (opcode_t op);

/** convert operand type to a "name"
 *  @param operand - the type of operand
 *  @return a human readable name
 */
const char* lc3_get_operand_name (operand_t operand);

/** Read an LC3 word (16 bits) from a file and return the value
 *  @param f - the object file
 *  @return the value, or -1 on EOF
 */
int lc3_read_LC3_word (FILE *f);

/** Read the symbol table file and populate the global <code>lc3_sym_tab</code>
 *  No error checking is performed. It is assumbed the file is in the correct
 *  format.
 *  @param sym_file, the file containing the symbol table
 */
void lc3_read_sym_table (FILE* sym_file);

/** Write an LC3 word to a file in binary or hex
 *  @param f - the file to write to
 *  @param value - the value to write
 */
void lc3_write_LC3_word (FILE* f, int value);

/** Write the symbol table to a file
 *  @param sym_file - the file in which the symbol table is written
 */
void lc3_write_sym_table (FILE* sym_file);

/** Convert a string to an integer using either the LC3 syntax, or the
 *  standard C syntax for a number. The LC3 format assumes hex unless
 *  the string is preceeded by a # sign. The initial x/X for hex values
 *  is optional. The C syntac is [+/-] digit [x/X] digit*
 *  @param token - the string to be converted
 *  @param value - pointer to where value will be stored
 *  @return 1 on success, 0 on failure
 */
int lc3_get_int (char* token, int* value);

/** Convert a string to an address. The sting is assumed to be a symbol. If
 *  it is found in the symbol table, the address is returned via the pointer.
 *  Otherwise call lc3_get_int() to convert the "number" to an address.
 *  @param token - the string to be converted
 *  @param value - pointer to where the address will be stored
 *  @return 1 on success, 0 on failure
 */
int lc3_get_address (char* token, int* value);

#endif /* __LC3_H__ */

