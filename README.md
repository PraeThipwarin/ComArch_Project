# ComArch_Project

วิธีคอมไพล์และรันไฟล์ assembler.cpp
คอมไพล์: g++ -std=c++17 -O2 assembler.cpp -o assembler.exe
รัน: ./assembler.exe .\test\test.txt program_rs.txt

วิธีคอมไพล์และรันไฟล์ simulator.cpp
คอมไพล์: g++ -std=c++17 -O2 simulator.cpp -o simulator
รัน: 
  1) โหมดปกติ: ./simulator machine-code/test-class.txt
  2) โหมดคูณ (--task=multiply): ./simulator --task=multiply machine-code/test-mul.txt
  3) โหมด combination (--task=comb): ./simulator --task=comb machine-code/test-combi.txt
