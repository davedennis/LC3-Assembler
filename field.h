#ifndef __FIELD_H__
#define __FIELD_H__

/*
 * field.h - simple field manipulation functions to get students working
 *           with masking. This will be used in the simulator to extract
 *           information when executing an LC3 instruction. It may also be
 *           used in the assembler to "create" the 16 bit LC3 instructions.
 *           If an assignment involving floating point manipulation (e.g.
 *           16 bit floating point addition), this file can be used to
 *           provide support routines for extracting/setting the sign,
 *           exponent, and mantissa fields.
 *
 * "Copyright (c) 2013 by Fritz Sieker."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written
 * agreement is hereby granted, provided that the above copyright notice
 * and the following two paragraphs appear in all copies of this software,
 * that the files COPYING and NO_WARRANTY are included verbatim with
 * any distribution, and that the contents of the file README are included
 * verbatim as part of a file named README with any distribution.
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

/** @file field.h
 *  @brief Defines interface of field.c functions (do not modify)
 *  @details This file defines the interface to a C file field.c that
 *  you will complete.  You will learn how to
 *  use the C language operators for binary <b>and</b> (<b>&amp;</b>),
 *  binary <b>or</b> (<b>|</b>), and binary <b>not</b> (<b>~</b>). You will also use the
 *  C language <b>bit shift</b> operators (<b>&lt;&lt;</b> and <b>&gt;&gt;</b>).
 *  <p>
 *  Binary <b>and</b> (<b>&amp;</b>) will be used to extract the value of a 
 *  bit and to set a bit to 0. This relies on the fact that the binary 
 *  <b>and</b> (<b>&amp;</b>) of a value and 1 results in the original value. 
 *  Binary <b>and</b> (<b>&amp;</b>) of a value and 0 results in
 *  0. Binary <b>or</b> (<b>|</b>) is use to set a bit to 1. This relies 
 *  on the the fact that binary <b>or</b> (<b>|</b>) of 1 and anything results 
 *  in a 1.
 *  <p>
 *  You will create masks. A mask is a bit pattern that contains 0's and 1's in
 *  appropriate places so when the binary <b>and/or</b> operation is performed,
 *  the result has extracted/modified the bits of interest. In the
 *  following examples <b>B</b> stands for bits of interest while <b>x</b>
 *  stands for a bit that is not of interest. Note that, in general,
 *  the bits of interest need not be consecutive. In this code, we will
 *  be dealing with consecutive sets of bits.
 *  <p><pre><code>
 *    value:    xxxBBBBxxxxx  value: xxxBBBBxxxxx   value: xxxBBBBxxxxx
 *    mask:     000111100000  mask:  111000011111   mask:  000111100000
 *    -------   ------------         ------------          ------------
 *    and(&amp;)    000BBBB0000   and(&amp;) xxx0000xxxxx   or(|)  xxx1111xxxxx
 *    result:   isolate field        clear field           set field
 *  </code></pre>
 *  <p>
 *  You may create masks on the fly using C code, or you may pre compute
 *  masks and store them in an array and just use the one you need. Note
 *  the mask for clearing bits is the binary <b>not</b> (<b>~</b>) of the
 *  mask for extracting bits.
 *  <p>
 *  Bit positions are numbered from 31 to 0 with 0 being the least
 *  significant bit. The bit position corresponds to the power of 2 in the
 *  binary representation.
 *  <p>
 *  As an example of how the fields interface is useful consider the
 *  IEEE half precision floating point representation.
 *  In the IEEE half precision floating point representation, 16 bits are
 *  used to represent a floating point number. This is shown in the following
 *  table where <b>S</b> represents the sign, <b>E</b> represents bits for the
 *  exponent, and <b>F</b> represents bits for the fraction.
 *
 *  <table border=1 rules="all">
 *  <tr>
 *  <td>bit position</td><td>31..16</td><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td>
 *  <td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td>
 *  </tr>
 *  <tr>
 *  <td>meaning</td><td>not used</td><td>S</td><td>E</td><td>E</td><td>E</td><td>E</td><td>E</td><td>F</td><td>F</td>
 *  <td>F</td><td>F</td><td>F</td><td>F</td><td>F</td><td>F</td><td>F</td><td>F</td>
 *  </tr>
 *  </table>
 *  To extract fields from the value, you would make calls like:
 *  <pre><code>
 *     int sign = getField(value, 15, 15, 0);
 *     int exp  = getField(value, 14, 10, 0);
 *     int frac = getField(value,  9,  0, 0);
 *  </code></pre>
 *
 *  In the following methods, hi and lo are NOT guaranteed to be ordered!
 *  So getField(value, 5, 10, 0) and getField(value, 10, 5, 0) must both produce
 *  identical results.
 *  <p>
 *  @author Fritz Sieker
 */

/** Extract the field (possibly signed) between bits hi and lo (inclusive).
 *  @param value  the source value or bit pattern
 *  @param hi the bit position of one end of the field
 *  @param lo the bit position of the other end of the field
 *  @param isSigned zero means the field is unsigned, non-zero means the field is signed
 *  @return The value of the field.  Sanity check example: 
 *   if the field is three bits wide and unsigned, the result
 *   will be a value between 0 and 7, regardless of the actual position of the
 *   bits in value. If the value is signed, the result will be between -4 and 3.
 */
int getField (int value, int hi, int lo, int isSigned);

/** Change the bits of oldValue between hi and lo to the newValue,
 * leaving the other bits unchanged.
 * @param oldValue the original value
 * @param hi the bit position of one end of the field
 * @param lo the bit position of the other end of the field
 * @param newValue the new value to put in the field (use lower bits)
 * @return the value after replacing <b>only</b> the hi to low bits 
 *         inclusive by newValue
 */
int setField (int oldValue, int hi, int lo, int newValue);

/** Determine if a value will fit in a specified field
 *  @param value  the source value
 *  @param width the number of bits holding the value
 *  @param isSigned zero means the field is unsigned, non-zer means the field is signed
 *  @return zero if the field does NOT fit. Return 1 if the value fits.
 */
int fieldFits (int value, int width, int isSigned);

#endif
