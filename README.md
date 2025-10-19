# ComArch_Project

====== วิธีคอมไพล์และรันไฟล์ assembler.cpp ======
คอมไพล์: g++ -O2 <ชื่อไฟล์ที่จะคอมไพล์> -o <output file.exe>
ex : g++ -std=c++17 -O2 assembler.cpp -o assembler.exe

รัน: ./<output file.exe> <input file(assembly)> <output file(machine code)>
ex: ./assembler.exe .\test\mul.asm program_rs.mc
    ./assembler.exe '..\assembly_lang\divide.asm' '..\output\div-test.mc'
วิธีคอมไพล์และรันไฟล์ simulator.cpp
คอมไพล์: g++ -std=c++17 -O2 simulator.cpp -o simulator
รัน: 
  1) โหมดปกติ: ./simulator machine-code/test-class.txt
  2) โหมดคูณ (--task=multiply): ./simulator --task=multiply machine-code/test-mul.txt
  3) โหมด combination (--task=comb): ./simulator --task=comb machine-code/test-combi.txt
