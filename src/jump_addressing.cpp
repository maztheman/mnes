/*
     JMP

        #  address R/W description
       --- ------- --- -------------------------------------------------
        1    PC     R  fetch opcode, increment PC
        2    PC     R  fetch low address byte, increment PC
        3    PC     R  copy low address byte to PCL, fetch high address
                       byte to PCH
*/

void memory_jmp_absolute()
{
    //2
    uint pcl = ext_memory_read(g_Registers.pc++);
    //3
    g_Registers.pc = pcl | (ext_memory_read(g_Registers.pc) << 8);
}

/*
  Absolute indirect addressing (JMP)

        #   address  R/W description
       --- --------- --- ------------------------------------------
        1     PC      R  fetch opcode, increment PC
        2     PC      R  fetch pointer address low, increment PC
        3     PC      R  fetch pointer address high, increment PC
        4   pointer   R  fetch low address to latch
        5  pointer+1* R  fetch PCH, copy latch to PCL

       Note: * The PCH will always be fetched from the same page
               than PCL, i.e. page boundary crossing is not handled.
*/

void memory_jmp_absolute_indirect()
{
    //2
    uint pointer_l = ext_memory_read(g_Registers.pc++);
    //3
    uint pointer_h = ext_memory_read(g_Registers.pc++) << 8;
    //4
    uint pcl = ext_memory_read(pointer_l | pointer_h);
    //5
    g_Registers.pc = pcl | (ext_memory_read(((pointer_l + 1) & 0xFF) | pointer_h) << 8);
}