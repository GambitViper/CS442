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
                    lw          $t2         _y                             
                    lw          $t3         _x                             
                    lw          $t4         _x                             
                    mul         $t5         $t3         $t4                
                    add         $t3         $t2         $t5                
                    sw          $t3         _r                             
                    li          $t2         114                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _r                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _y                             
                    lw          $t4         _x                             
                    add         $t5         $t2         $t4                
                    lw          $t2         _x                             
                    mul         $t4         $t5         $t2                
                    sw          $t4         _r                             
                    li          $t2         114                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _r                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _y                             
                    lw          $t5         _x                             
                    lw          $t6         _x                             
                    mul         $t7         $t5         $t6                
                    add         $t5         $t2         $t7                
                    sw          $t5         _r                             
                    li          $t2         114                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _r                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _x                             
                    lw          $t6         _x                             
                    mul         $t7         $t2         $t6                
                    lw          $t2         _y                             
                    add         $t6         $t7         $t2                
                    sw          $t6         _r                             
                    li          $t2         114                            
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         61                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    lw          $t2         _r                             
                    li          $v0         1                              
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         10                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         1                              
                    li          $v0         11                             
                    mul         $t2         $t2         14                 
                    add         $t2         $t2         70                 
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         32                             
                    li          $v0         11                             
                    move        $a0         $t2                            
                    syscall                                                
                    li          $t2         0                              
                    li          $v0         11                             
                    mul         $t2         $t2         14                 
                    add         $t2         $t2         70                 
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
_r:                 .word       0                                          
_x:                 .word       0                                          
_y:                 .word       0                                          
