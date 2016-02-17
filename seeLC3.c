#include <stdio.h>
#include <stdlib.h>

#include "lc3.h"
#include "util.h"

int main (int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: seeLC3 opcode\n");
    printf("e.g. seeLC3 ADD\n");
  }
  else {
    opcode_t opcode = util_get_opcode(argv[1]);
    LC3_inst_t* info = lc3_get_inst_info(opcode); 

    if (! info) {
      printf("%s is no an opcode\n", argv[1]);
    }

    else {
      printf("form bit:%2d\n", info->formBit);

      for (int i = 0; i < 2; i++) {
        if (info->forms[i].name != NULL) {
          printf("form: %d name: %s operands: %s prototype x%04x\n", i,
	       info->forms[i].name,
	       lc3_get_format_name(info->forms[i].operands),
	       info->forms[i].prototype); 
	}
      }
    }
  }
}
