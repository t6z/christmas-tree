
clk_freq = 3333333 # 3.333_333 MHz = 20MHz/6
clk_div = 4

notes = {
    "C4":261.63,
    "Cs4":277.18,
    "D4":293.66,
    "Ds4":311.13,
    "E4":329.63,
    "F4":349.23,
    "Fs4":369.99,
    "G4":392,
    "Gs4":415.3,
    "A4":440,
    "As4":466.16,
    "B4":493.88
}

per = [(clk_freq/clk_div)/(2*x) for x in notes.values()]
cmp0 = [x/2 for x in per]

print("\t4\t5\t6\t7")
for i,n in enumerate(notes):
    print(n,"\t",hex(int(per[i])),"\t",hex(int(per[i]/2)),
          "\t",hex(int(per[i]/4)),"\t",hex(int(per[i]/8)))
    
print("\t4\t5\t6\t7")
for i,n in enumerate(notes):
    print(n,"\t",int(notes[n]),"\t",int(notes[n]*2),"\t",int(notes[n]*4),"\t",int(notes[n]*8))

# Write to C Header File
with open("src/notes.h","w+") as file:

    file.write("#ifndef __NOTES_H__\n#define __NOTES_H__\n\n")
    
    file.write("#include <avr/io.h>\n\n")
    file.write("#define NUM_NOTES ("+str(len(per))+"U)\n\n")

    file.write("typedef enum {\n")
    for i in enumerate(notes.keys()):
        if i[0] == 0:
            file.write("   P"+i[1]+" = 0,\n")
        elif i[0] == (len(notes)-1):
            file.write("   P"+i[1]+"\n")
        else:
            file.write("   P"+i[1]+",\n")
    file.write("} Notes_e;\n\n")

    file.write("extern uint16_t tca0_per[NUM_NOTES];\n")
    file.write("extern uint16_t tca0_cmp0[NUM_NOTES];\n")

    file.write("\n#endif // __NOTES_H__\n")

# Write to C Source File
with open("src/notes.c", "w+") as file:
    file.write("#include \"notes.h\"\n\n")

    file.write("uint16_t tca0_per[NUM_NOTES] = {\n")
    for p in enumerate(per):
        p_hex = hex(int(p[1])).upper().replace('X','x')
        if p[0] == (len(per)-1):
            file.write(p_hex+"\n")
        elif p[0]%2 == 0:
            file.write("   "+p_hex+",")
        else:
            file.write(p_hex+",\n")
    file.write("};\n\n")

    file.write("uint16_t tca0_cmp0[NUM_NOTES] = {\n")
    for p in enumerate(cmp0):
        p_hex = hex(int(p[1])).upper().replace('X','x')
        if p[0] == (len(per)-1):
            file.write(p_hex+"\n")
        elif p[0]%2 == 0:
            file.write("   "+p_hex+",")
        else:
            file.write(p_hex+",\n")
    file.write("};\n")


