                    .text                                                # FinishSemantics
                    .globl      __start                                  # FinishSemantics
__start:                                                                 # FinishSemantics
                    jal         _main                                    # FinishSemantics
                    li          $v0         10                           # FinishSemantics
                    syscall                                              # FinishSemantics
_main:                                                                   # func entry
                    subu        $sp         $sp         4                # ProcFuncBody
                    sw          $ra         0($sp)                       # ProcFuncBody
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _x                             
L3:                                                                      # ProcForLoop
                    lw          $t1         _x                             
                    li          $t2         5                              
                    bge         $t1         $t2         L1                 
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
                    lw          $t1         _x                             
                    li          $v0         1                              
                    move        $a0         $t1                            
                    syscall                                                
                    li          $t1         10                             
                    li          $v0         11                             
                    move        $a0         $t1                            
                    syscall                                                
                    lw          $t1         _x                             
                    li          $t2         1                              
                    add         $t3         $t1         $t2                
                    sw          $t3         _x                             
                    b           L3                                       # ProcForLoop
L1:                                                                      # ProcForLoop
                    li          $v0         4                              
                    la          $a0         L4                             
                    syscall                                                
main_ret:                                                                  
                    lw          $ra         0($sp)                       # ProcFuncBody
                    addu        $sp         $sp         4                # ProcFuncBody
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_x:                 .word       0                                          
L2:                 .asciiz     "loop "                                    
L4:                 .asciiz     "done\n"                                   
