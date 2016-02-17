          .ORIG   x3000

; list of simple instructions, none with labels

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

LABEL1    .FILL   x1234

          .END