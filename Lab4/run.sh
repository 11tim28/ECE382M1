make
./assembler.linux ./add.asm ./obj/add.obj
./assembler.linux ./data.asm ./obj/data.obj
./assembler.linux ./int.asm ./obj/int.obj
./assembler.linux ./vector_table.asm ./obj/vector_table.obj
./assembler.linux ./except_prot.asm ./obj/except_prot.obj
./assembler.linux ./except_unaligned.asm ./obj/except_unaligned.obj
./assembler.linux ./except_unknown.asm ./obj/except_unknown.obj
./simulate ucode4 ./obj/add.obj ./obj/data.obj ./obj/except_prot.obj ./obj/except_unaligned.obj ./obj/except_unknown.obj ./obj/int.obj ./obj/vector_table.obj
