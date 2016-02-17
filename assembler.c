#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>

#include "assembler.h"
#include "field.h"
#include "lc3.h"
#include "symbol.h"
#include "tokens.h"
#include "util.h"

/** Global variable containing the head of the linked list of structures */
static line_info_t* infoHead;

/** Global variable containing the tail of the linked list of structures */
static line_info_t* infoTail;

/** Global variable containing information about the current line */
static line_info_t* currInfo;

void asm_init_line_info (line_info_t* info) {
  if (info) {
    info->next        = NULL;
    info->lineNum     = srcLineNum;
    info->address     = currAddr;
    info->machineCode = 0;
    info->opcode      = OP_INVALID;
    info->form        = 0;
    info->reg1        = -1;
    info->reg2        = -1;
    info->reg3        = -1;
    info->immediate   = 0;
    info->reference   = NULL;
  }
}

void asm_print_line_info (line_info_t* info) {
  if (info) {
    printf("%3d: address: x%04x machineCode:%04x op: %s form: %d reg1:%3d, reg2:%3d reg3:%3d imm: %d ref: %s\n",
     info->lineNum, info->address, info->machineCode,
     lc3_get_opcode_name(info->opcode), info->form, info->reg1,
     info->reg2, info->reg3, info->immediate, info->reference);
  }
}

/* based on code from http://www.eskimo.com/~scs/cclass/int/sx11c.html */
void asm_error (char* msg, ...) {
 numErrors++;
 va_list argp;
 fprintf(stderr, "ERROR %3d: ", srcLineNum);
 va_start(argp, msg);
 vfprintf(stderr, msg, argp);
 va_end(argp);
 fprintf(stderr, "\n");
}

void asm_init (void) {
  infoHead = infoTail = currInfo = NULL; 
  tokens_init();
  lc3_sym_tab = symbol_init(0); 
}

/** @todo implement this function */
//done
void asm_pass_one (char* asm_file_name, char* sym_file_name) {
	char line[MAX_LINE_LENGTH];
	char* token = NULL;	
	//open the souce file
	FILE* fp = open_read_or_error(asm_file_name);
	//while there are still lines to read
	//call fgets to store the fp into line	
	while(fgets(line,MAX_LINE_LENGTH,fp)){	
    printf("%s",line);
		//convert to a list of tokens
		token = tokenize_line (line);
    //while my token is not null
    if(token != NULL){
      //if(util_get_opcode(token) != -1){
      //allocate memory currInfo
      currInfo = malloc(sizeof(struct line_info));
      //idk what this does but i'm doing it
      asm_init_line_info(currInfo);

      //check line syntax
      check_line_syntax(token);
      update_address();
      
      //set up link list
      
        if(infoHead == NULL){
          infoHead = currInfo;
          infoTail = currInfo;
        } else{
          //if the head is not null;
          //temp to inc count
          //get the next of the info tail
          infoTail -> next= currInfo;
          //set it equal to currinfo
          infoTail = currInfo;
          //inc that address
        }
      //} 
    }
  }
  //write the symbol talble file 
  if(numErrors == 0){
    lc3_write_sym_table(fp);
  }
  fclose(fp);
}


/** @todo implement this function */
void asm_pass_two (char* obj_file_name) {
	//do?
  FILE* fw = open_write_or_error(obj_file_name);
  for(currInfo = infoHead; currInfo->opcode != 17; currInfo = currInfo->next){
    
    //asm_print_line_info(currInfo);
    LC3_inst_t* inst = lc3_get_inst_info(currInfo -> opcode);
    printf("WHY IS ADD WRONG %p\n", inst);
    operands_t operands = inst->forms[currInfo->form].operands;
   printf("form is: %d\n", currInfo->form);
    currInfo->machineCode = inst->forms[currInfo->form].prototype;
    for (operand_t op = FMT_R1; op <= FMT_STR; op <<= 1) {
        if(op & operands){
          printf("op is %d\n", op );
          if(currInfo->opcode == OP_BR){
            currInfo->machineCode = setField(currInfo->machineCode,11,9,currInfo->reg1);
            encode_PC_offset_or_error(9);
          }

          encode_operand(op);
         // printf("making it?\n");
        }
    }
    asm_print_line_info(currInfo);
    if(currInfo->opcode == OP_BLKW){
      for(int i = 0; i < currInfo->immediate;i++){
        lc3_write_LC3_word(fw,0000);
      }
    }else{
     lc3_write_LC3_word(fw,currInfo->machineCode);
      }

  }
}

/** @todo implement this function */
void asm_term (void) {

}

/** @todo implement this function */ //bob
//done
char* check_for_label (char* token) {
  if(util_get_opcode (token) == -1){
		//if it is then is it a vaild label?		
    if(util_is_valid_label(token)){
			//if it is a vaild label add that shit
			//return the next token broski
      //fprintf(stderr, "adding symbol: %s\n",token);
      if(symbol_add(lc3_sym_tab,token,currAddr) == 0){
        asm_error(ERR_DUPLICATE_LABEL,token);
      }
      return next_token();
    }	else{
      //damn that shit anint wokring right
    asm_error(ERR_BAD_LABEL,token);
    }		
  }

  return token;
}

/** @todo implement this function */
//done
void check_line_syntax (char* token) {
  printf("check_line_syntax('%s')\n", token);
  //check if its a label
  token = check_for_label(token);
  printf(" my token is %s \n ", token);
  //store the op in an int
  if(token == NULL)
    return;
  int myop = util_get_opcode(token);
  printf("opcode is: %d\n", myop );
  //store it itno my data structure..
  currInfo -> opcode = myop;

  if(myop == OP_BR){
    currInfo->reg1 = util_parse_cond(token+2);
    token = next_token();
    currInfo->reference = strdup(token);
    //get_operand(op,token);
  }

  LC3_inst_t* inst = lc3_get_inst_info(myop);
  printf("inst is:  %p \n" , inst);

  int position = 0;
  if(strcasecmp(inst ->forms[0].name,token) != 0){
    position = 1;
  }
  
  /*if(inst ->forms[1].name != NULL &&strcasecmp(inst ->forms[1].name,token) == 0){
    position = 1;
    if(strcasecmp(inst ->forms[0].name,token) != 0)
    position = 1;
  }*/
 if(myop != OP_BR){
  currInfo->form=position;}

printf("%d\n whats my position", position);
  operands_t format =  inst -> forms[position].operands;
  scan_operands(format);

  
}

/** @todo implement this function */
//done
void encode_operand (operand_t operand) {
  switch(operand){
  case FMT_R1:
    currInfo->machineCode = setField(currInfo->machineCode,11,9,currInfo->reg1);
    break;
  case FMT_R2:
    currInfo -> machineCode = setField(currInfo->machineCode,8,6,currInfo->reg2);
    break;
  case FMT_R3:
    currInfo -> machineCode = setField(currInfo->machineCode,2,0,currInfo->reg3);
    break;
  case FMT_IMM5:
    currInfo -> machineCode = setField(currInfo->machineCode,4,0,currInfo->immediate);
    currInfo->form=1;
    break;
  case FMT_IMM6:
    currInfo -> machineCode = setField(currInfo->machineCode,5,0,currInfo->immediate);
    break;
  case FMT_VEC8:
     currInfo -> machineCode = setField(currInfo->machineCode,7,0,currInfo->immediate);
     currInfo->form=1;
    break;
  case FMT_ASC8:
    currInfo -> machineCode = setField(currInfo->machineCode,7,0,currInfo->immediate);
    break;
  case FMT_PCO9:
    encode_PC_offset_or_error(9);
    break;
  case FMT_PCO11:
    encode_PC_offset_or_error(11);
    break;
  case FMT_IMM16:
    currInfo -> machineCode = setField(currInfo->machineCode,15,0,currInfo->immediate);
    break;
  case FMT_CC:
    printf("enter\n");
    //currInfo->machineCode = setField(currInfo->machineCode,11,9,currInfo->reg1);
    break;
  case FMT_STR:
    //do somehting
  default:
    break;
  }
}

/** @todo implement this function */
void encode_PC_offset_or_error (int width) {
  printf("REFERENCE: %s\n",currInfo->reference);
  symbol_t* symbol = symbol_find_by_name (lc3_sym_tab, currInfo->reference);
  printf("SYMBOL: %s,%d\n",symbol->name,symbol->addr);
  if(symbol != NULL){
      int offset = symbol->addr-currInfo->address-1;
      int whatever = fieldFits(offset,width,1);
      if(whatever == 1){
        currInfo -> machineCode = setField(currInfo->machineCode,width-1,0,offset);
      }
      else{
        asm_error(ERR_BAD_PCOFFSET,currInfo->reference);
      }

  }
}

/** @todo implement this function */
//done
void get_comma_or_error (void) {
  char* token = next_token();
  if(strcasecmp(token,",") != 0){
    asm_error(ERR_EXPECTED_COMMA);
  }
}

/** @todo implement this function */
//done
void get_immediate_or_error (char* token, int width, int isSigned) {
  //field fits
  //value widith issighned
 int value = 0;
 int bob = lc3_get_int(token,&value);

 if(!bob)
  asm_error(ERR_BAD_IMM,token);

  if(fieldFits (value,width,isSigned) != 0){
      //set something equal to it 
      currInfo -> immediate = value; 
  }
  else{
    asm_error(ERR_EXPECT_REG_IMM);
  }
}
/** @todo implement this function */
//done
void get_PC_offset_or_error (char* token) {
  //PCoffset..
  printf("TOKEN %s\n",token);
  if(util_is_valid_label(token) != 0){
    currInfo -> reference = strdup(token);
  }
  else{
    asm_error(ERR_BAD_LABEL,token);
  }


}

/** @todo implement this function */
//done
int get_reg_or_error (char *token) {
  int get = util_get_reg(token);
  if(get == -1){
    asm_error(ERR_EXPECTED_REG,token);
  }

  return get;
}

/** @todo implement this function */
//done
FILE *open_read_or_error (char* file_name) {
	FILE* fp = fopen(file_name,"r");
	if(fp == NULL){
		asm_error(ERR_OPEN_READ, file_name);
	}  
	
	return fp;
}

/** @todo implement this function */
//done
FILE *open_write_or_error (char* file_name) {  
	FILE* fw = fopen(file_name,"w");
  if(fw == NULL){
    asm_error(ERR_OPEN_WRITE,file_name);
  }
  return fw;
}

/** @todo implement this function */
//done
void get_operand (operand_t operand, char* token) {
  switch (operand) {
    case FMT_R1:
    currInfo->reg1 = get_reg_or_error(token);
    break;
    case FMT_R2:
    currInfo->reg2 = get_reg_or_error(token);
    break;
    case FMT_R3:
    case FMT_IMM5:
      if(util_get_reg(token) != -1){
        currInfo->reg3 = util_get_reg(token);
      }else{
        get_immediate_or_error(token,5,1);
        currInfo->form=1;
      }
      break;
    case FMT_IMM6:
      get_immediate_or_error(token,6,1);
      break;
    case FMT_VEC8:
      get_immediate_or_error(token,8,0);
      //currInfo->form = 1;
      break;
    case FMT_ASC8:
      get_immediate_or_error(token,8,0);
      break;
    case FMT_PCO9:
      get_PC_offset_or_error(token);
      currInfo -> reference = strdup(token);
      break;
    case FMT_PCO11:
      get_PC_offset_or_error(token);
      currInfo -> reference = strdup(token);
      break;
    case FMT_IMM16:
      get_immediate_or_error(token,16,1);
      /*if(currInfo -> opcode == 16){
        currInfo -> address = currInfo -> immediate;
      }*/
      break;
    case FMT_STR:
      printf("this is the probelm");
      currInfo -> reference = strdup(token);
      break;
    default:
    break;
  }
}

// can use ints instead of unsigned ints as long as v is never negative. This
// is true for the use in LC3 assembler.
// see http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
int count_bits (int v) {
  //unsigned int v; // count the number of bits set in v
int c; // c accumulates the total bits set in v
for (c = 0; v; c++)
{
  v &= v - 1; // clear the least significant bit set
}
return c;
}


/** @todo implement this function */
//done
void scan_operands (operands_t operands) {
  printf("scan_operands() for %s\n", lc3_get_format_name(operands));
  int operandCount = 0;
  int numOperands  = count_bits(operands);
  int errorCount   = numErrors;
  for (operand_t op = FMT_R1; op <= FMT_STR; op <<= 1) {
    //if the bits are set
    //printf(" ");
    if(op & operands){
      //create a token
      char* token = next_token();
      //get the operand of that token
      if(token != NULL){
      get_operand(op,token);
      
        //if errorocunt is not equal return
       if (errorCount != numErrors)
        return; // error, so skip processing remainder of line
      //inc operand count
      operandCount++;
      //if the count is less then the operands 
      if(operandCount < numOperands){
        //get comma or error
        get_comma_or_error();
      }
      //check errorcount again
      if (errorCount != numErrors)
      return; // error, so skip processing remainder of line

    }
  }
  }
}

/** @todo implement this function */
//done..
void update_address (void) {
  int op = currInfo -> opcode;
  if(op == OP_ORIG){
    currAddr = currInfo -> immediate;
    currInfo->address = currInfo -> immediate;
  } 
  else if(op == OP_BLKW){
    currAddr += currInfo->immediate;
  }
  else if(op == OP_STRINGZ){
    currAddr += strlen(currInfo->reference)-1;
  }
  else{
    currAddr++;
  }
}

