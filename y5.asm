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
                    la          $a0         L1                             
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _p                             
L10:                                                                     # ProcWhile
                    lw          $t1         _p                             
                    li          $t2         1                              
                    ble         $t1         $t2         L2                 
                    li          $v0         4                              
                    la          $a0         L3                             
                    syscall                                                
                    lw          $t1         _p                             
                    li          $v0         1                              
                    move        $a0         $t1                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L4                             
                    syscall                                                
                    li          $t1         2                              
                    sw          $t1         _d                             
L8:                                                                      # ProcWhile
                    lw          $t2         _d                             
                    lw          $t3         _p                             
                    li          $t4         2                              
                    div         $t5         $t3         $t4                
                    bgt         $t2         $t5         L5                 
                    lw          $t2         _p                             
                    lw          $t3         _d                             
                    div         $t4         $t2         $t3                
                    sw          $t4         _q                             
                    lw          $t2         _q                             
                    lw          $t3         _d                             
                    mul         $t5         $t2         $t3                
                    lw          $t2         _p                             
                    bne         $t5         $t2         L6                 
                    lw          $t2         _d                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L7                             
                    syscall                                                
L6:                                                                      # ProcIf
                    lw          $t2         _d                             
                    li          $t3         1                              
                    add         $t5         $t2         $t3                
                    sw          $t5         _d                             
                    b           L8                                       # ProcWhile
L5:                                                                      # ProcWhile
                    li          $v0         4                              
                    la          $a0         L9                             
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L1                             
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t2         $v0                            
                    sw          $t2         _p                             
                    b           L10                                      # ProcWhile
L2:                                                                      # ProcWhile
main_ret:                                                                  
                    lw          $ra         0($sp)                       # ProcFuncBody
                    addu        $sp         $sp         4                # ProcFuncBody
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_d:                 .word       0                                          
_p:                 .word       0                                          
_q:                 .word       0                                          
L7:                 .asciiz     " "                                        
L4:                 .asciiz     " are "                                    
L1:                 .asciiz     "> "                                       
L9:                 .asciiz     "\n"                                       
L3:                 .asciiz     "factors of "                               
