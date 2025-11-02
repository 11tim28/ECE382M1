make
./assembler.linux ./add.asm ./add.obj
./assembler.linux ./data.asm ./data.obj
./assembler.linux ./int.asm ./int.obj
./assembler.linux ./vector_table.asm ./vector_table.obj
./assembler.linux ./except_prot.asm ./except_prot.obj
./assembler.linux ./except_unaligned.asm ./except_unaligned.obj
./assembler.linux ./except_unknown.asm ./except_unknown.obj
./assembler.linux ./except_page_fault.asm ./except_page_fault.obj
./assembler.linux ./pagetable.asm ./pagetable.obj
./simulate ucode5 ./pagetable.obj ./add.obj ./data.obj ./except_prot.obj ./except_unaligned.obj ./except_unknown.obj ./int.obj ./vector_table.obj ./except_page_fault.obj 
