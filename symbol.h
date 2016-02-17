#ifndef __SYMBOL_H__
#define __SYMBOL_H__

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

/** @file symbol.h
 *  @brief Defines the interface to symbol.c functions (do not modify)
 *  @details This file defines the interface to a C file symbol.c that
 *  you will complete. The underlying data structure(s) used will be
 *  defined by the actual assignment. The assignment will define whether symbols
 *  are case sensitive or case in-sensitive.
 *  <p>
 *  In this implementation, you will learn about dynamic memory management using
 *  malloc/free. You will also learn about function pointers (callback functions).
 */

#define SYMBOL_SIZE 997  /**<  997 prime number, the size of hash table  */

/** This defines an opaque type. The actual contents of the structure are hidden
 *  in the implementation and only a pointer to this structure is used
 *  externally to this file. A pointer to an opaque structure is sometimes
 *  referred to as a handle.
 */

typedef struct sym_table sym_table_t;

/** The symbol_find methods return a pointer to this data structure. It is up
 *  to the implementor to decide how to use this stucture in the implementation.
 */

typedef struct symbol {
    char* name; /**< the name of the symbol */
    int   addr; /**< symbol's address in the LC3 memory */
} symbol_t;

/** Create a new symbol table and return a pointer to it.
 *  @param lookup_by_addr - if this value is non-zero, allocate and initialize
 *  an array of <code>char * (addr_table)</code> that is indexed by an LC3
 *  address to find the label (if any) associated with that address. 
 *  @return a pointer to the symbol table.
 */ 
sym_table_t* symbol_init (int lookup_by_addr);

/** Remove all symbols from the symbol table, and free all allocated memory.
 *  There must not be any memory leaks. After executing this function, the
 *  opaque pointer to the symbol table is no longer valid.
 */
void symbol_term(sym_table_t* symTab);

/** Remove all the symbols from the symbol table. After this call the opaque
 *  symbol table pointer is still valid and new symbols may be added to it. 
 *  If needed, clear the <code>addr_table</code>.
 *  @param symTab - pointer to the symbol table
 */
void symbol_reset (sym_table_t* symTab);

/** Add a symbol to the symbol table.
 *  @param symTab - pointer to the symbol table
 *  @param name - the name of the symbol
 *  @param addr - the address of the symbol
 *  @return 1 if the symbol is not currently in the symbol table, 0 if the
 *  symbol already exists. If appropriate, add the name to the
 *  <code>addr_table</code>. The <b>first</b>symbol with a given address
 *  should be stored in the <code>addr_table</code>.
 */

int symbol_add (sym_table_t* symTab, const char* name, int addr);

/** This function is only used internally and should be declared static. It is
 *  a useful support function for the <code>add()/find()</code> functions.
 *  It is declared here for documentation purposes.
 *  @param symTab - pointer to the symbol table
 *  @param name - the name of the symbol
 *  @param hash - pointer to location where hash value will be stored 
 *  @param index - pointer to location where index will be stored 
 *  @return  the nodes information or NULL if no symbol is associated with
 *   the name.
 */
struct node* symbol_search (sym_table_t* symTab, const char* name, int* hash, int* index);

/** Find a symbol by its name
 *  @param symTab - pointer to the symbol table
 *  @param name - the symbols name
 *  @return  the symbols information or NULL if no symbol is associated with
 *   the name.
 */
symbol_t* symbol_find_by_name (sym_table_t* symTab, const char* name);

/** Find a name by its LC3 address
 *  @param symTab - pointer to the symbol table
 *  @param addr - an LC3 address
 *  @return the <b>label</b> at that address or NULL if no symbol is
 *  associated with the adddress. 
 */
char* symbol_find_by_addr (sym_table_t* symTab, int addr);

/** Defines the signature of a callback function (also known as a function
 *  pointer). This is how languages such as Java and C++ do <b>dynamic</b>
 *  binding (i.e. figure out which function to call). Recall that in Java the
 *  code <code>obj.equals(object)</code> will call one of possibly many
 *  different methods depending on the actual type of <code>obj</code>. This
 *  is because the method <b>.equals()</b> may be <b>overridden</b>.
 *  <p>
 *  In the LC3, dynamic binding is based on the <b>JSRR</b> opcode. With this
 *  opcode, the address of the routine to call is stored in a register and can
 *  be changed at runtime. Compare this to a <b>JSR nameOfRoutine</b> opcode which
 *  specifies what routine to call from the label that follows it. Thus, the
 *  address is fixed at <b>assembly</b> time. 
 *  <p>
 *  This is used in the symbol_iterate() function. An interesting variation
 *  would be to have the callback function return an integer which determines
 *  whether the iteration should contibue or terminate.
 *  @param symTab - pointer to the symbol table
 */
typedef void (*iterate_fnc_t)(symbol_t* sym, void* data);

/** This function calls the function for every entry in the symbol table.
 *  The assigment will define the order in which the entries should be visited.
 *  @param symTab - pointer to the symbol table
 *  @param fnc - the function to be called on every element
 *  @param data - any additional information to be passed on to fnc. The called
 *  function will cast this to whatever type was actually passed.
 */

void symbol_iterate (sym_table_t* symTab, iterate_fnc_t fnc, void* data);

#endif /* __SYMBOL_H__ */

