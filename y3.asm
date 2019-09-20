                    .text                                                # FinishSemantics
                    .globl      __start                                  # FinishSemantics
__start:                                                                 # FinishSemantics
                    jal         _main                                    # FinishSemantics
                    li          $v0         10                           # FinishSemantics
                    syscall                                              # FinishSemantics
_main:                                                                   # func entry
                    subu        $sp         $sp         4                # ProcFuncBody
                    sw          $ra         0($sp)                       # ProcFuncBody
                    li          $t0         62                             
                    li          $v0         11                             
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _x                             
                    li          $t1         62                             
                    li          $v0         11                             
                    move        $a0         $t1                            
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t1         $v0                            
                    sw          $t1         _y                             
                    li          $t2         120                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _x                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         121                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _y                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
L4:                                                                      # ProcWhile
                    lw          $t2         _x                             
                    lw          $t3         _y                             
                    beq         $t2         $t3         L1                 
                    lw          $t2         _x                             
                    lw          $t3         _y                             
                    bge         $t2         $t3         L2                 
                    lw          $t2         _y                             
                    lw          $t3         _x                             
                    sub         $t4         $t2         $t3                
                    sw          $t4         _y                             
                    b           L3                                       # ProcIf
L2:                                                                      # ProcIf
                    lw          $t2         _x                             
                    lw          $t3         _y                             
                    sub         $t5         $t2         $t3                
                    sw          $t5         _x                             
L3:                                                                      # ProcIf
                    b           L4                                       # ProcWhile
L1:                                                                      # ProcWhile
                    li          $t2         103                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _x                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
main_ret:                                                                  
                    lw          $ra         0($sp)                       # ProcFuncBody
                    addu        $sp         $sp         4                # ProcFuncBody
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_x:                 .word       0                                          
_y:                 .word       0                                          
