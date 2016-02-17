;; File:        instruction.asm
;; Description: Contains all instructions for PA5
;; Author:      Chris Wilcox
;; Date:        11/1/2010

          .ORIG   x3000

; list of simple instructions, none dealt with labels

           ADD    R3,R2,R1           ; ADD instruction
           ADD    R3,R2,#-5          ; ADD instruction
           ADD    R3,R2,x0f          ; ADD instruction
           AND    R4,R5,R6           ; AND instruction
           AND    R4,R5,#6           ; AND instruction
           AND    R4,R5,#15          ; AND instruction
           NOT    R7,R4              ; NOT instruction
           HALT                      ; HALT instruction
           JMP    R4                 ; JMP instruction
           JSRR   R5                 ; JSRR instruction
           RET                       ; RET instruction
           RTI                       ; RTI instruction
           TRAP   x21                ; TRAP instruction
           LDR    R2,R7,#10          ; LDR instruction
           STR    R2,R7,#-8          ; STR instruction

; now try instructions that contain labels (e.g. PCoffsets)

           BR     LABEL0             ; BR instructions
           BRn    LABEL0
           BRz    LABEL0
           BRp    LABEL0
           BRnz   LABEL0
           BRnp   LABEL0
           BRzp   LABEL0
           BRnzp  LABEL0
           JSR    NEXT               ; JSR instruction

LABEL0     LD     R2,LABEL1          ; LD instruction
           LDI    R3,LABEL2          ; LDI instruction
           LEA    R4,LABEL3          ; LEA instruction
           ST     R5,LABEL4          ; ST instruction
           STI    R6,LABEL5          ; STI instruction

; now try in pseudo-ops
NEXT      GETC
          OUT
          IN
          PUTS
          GETS
          .BLKW 3

LABEL1    .FILL   x1111
LABEL2    .FILL   x1111
LABEL3    .FILL   x1111
LABEL4    .FILL   x1111
LABEL5    .FILL   x1111

          .END
