                    .text                                                # FinishSemantics
                    .globl      __start                                  # FinishSemantics
__start:                                                                 # FinishSemantics
                    jal         _main                                    # FinishSemantics
                    li          $v0         10                           # FinishSemantics
                    syscall                                              # FinishSemantics
_main:                                                                   # func entry
                    subu        $sp         $sp         4                # ProcFuncBody
                    sw          $ra         0($sp)                       # ProcFuncBody
                    li          $v0         4                              
                    la          $a0         L6                             
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t9         $v0                            
                    sw          $t9         _nn                            
L15:                                                                     # ProcWhile
                    lw          $s0         _nn                            
                    li          $s1         0                              
                    beq         $s0         $s1         L7                 
                    jal         _isPrime                                 # CallFunc
                    li          $v0         4                              
                    la          $a0         L8                             
                    syscall                                                
                    lw          $s0         _nn                            
                    li          $v0         1                              
                    move        $a0         $s0                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L9                             
                    syscall                                                
                    lw          $s0         _ret                           
                    li          $s1         1                              
                    bne         $s0         $s1         L10                
                    li          $v0         4                              
                    la          $a0         L11                            
                    syscall                                                
                    b           L14                                      # ProcIf
L10:                                                                     # ProcIf
                    li          $v0         4                              
                    la          $a0         L12                            
                    syscall                                                
                    lw          $s0         _c                             
                    li          $v0         1                              
                    move        $a0         $s0                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L13                            
                    syscall                                                
L14:                                                                     # ProcIf
                    li          $v0         4                              
                    la          $a0         L6                             
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $s0         $v0                            
                    sw          $s0         _nn                            
                    b           L15                                      # ProcWhile
L7:                                                                      # ProcWhile
                    li          $v0         4                              
                    la          $a0         L16                            
                    syscall                                                
main_ret:                                                                  
                    lw          $ra         0($sp)                       # ProcFuncBody
                    addu        $sp         $sp         4                # ProcFuncBody
                    jr          $ra                                      # func return
_isPrime:                                                                # func entry
                    subu        $sp         $sp         4                # ProcFuncBody
                    sw          $ra         0($sp)                       # ProcFuncBody
                    li          $t0         -1                             
                    sw          $t0         _r                             
                    li          $t1         2                              
                    sw          $t1         _d                             
                    li          $t2         0                              
                    sw          $t2         _c                             
                    lw          $t3         _nn                            
                    li          $t4         2                              
                    div         $t5         $t3         $t4                
                    sw          $t5         _n                             
L3:                                                                      # ProcWhile
                    lw          $t3         _d                             
                    lw          $t4         _n                             
                    bgt         $t3         $t4         L1                 
                    lw          $t3         _nn                            
                    lw          $t4         _d                             
                    div         $t6         $t3         $t4                
                    lw          $t3         _d                             
                    mul         $t4         $t6         $t3                
                    sw          $t4         _a                             
                    lw          $t3         _a                             
                    lw          $t6         _nn                            
                    bne         $t3         $t6         L2                 
                    lw          $t3         _c                             
                    li          $t6         1                              
                    add         $t7         $t3         $t6                
                    sw          $t7         _c                             
L2:                                                                      # ProcIf
                    lw          $t3         _d                             
                    li          $t6         1                              
                    add         $t8         $t3         $t6                
                    sw          $t8         _d                             
                    b           L3                                       # ProcWhile
L1:                                                                      # ProcWhile
                    lw          $t3         _c                             
                    li          $t6         0                              
                    bne         $t3         $t6         L4                 
                    li          $t3         1                              
                    sw          $t3         _ret                           
                    b           L5                                       # ProcIf
L4:                                                                      # ProcIf
                    li          $t6         0                              
                    sw          $t6         _ret                           
L5:                                                                      # ProcIf
isPrime_ret:                                                               
                    lw          $ra         0($sp)                       # ProcFuncBody
                    addu        $sp         $sp         4                # ProcFuncBody
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_d:                 .word       0                                          
_n:                 .word       0                                          
_r:                 .word       0                                          
_nn:                .word       0                                          
_ret:               .word       0                                          
_a:                 .word       0                                          
_c:                 .word       0                                          
L12:                .asciiz     "not prime. It has "                               
L13:                .asciiz     " divisors.\n"                               
L8:                 .asciiz     "number "                                  
L9:                 .asciiz     " is "                                     
L11:                .asciiz     "prime.\n"                                 
L6:                 .asciiz     "> "                                       
L16:                .asciiz     "done\n"                                   
