import subprocess

tunes = {
    "Simpsons:d=4,o=5,b=160:32p,c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g",
    "Greensleaves:d=4,o=5,b=140:g,2a#,c6,d.6,8d#6,d6,2c6,a,f.,8g,a,2a#,g,g.,8f,g,2a,f,2d,g,2a#,c6,d.6,8e6,d6,2c6,a,f.,8g,a,a#.,8a,g,f#.,8e,f#,2g",
    "blah:d=4,o=4,b=160:c,d,e,f,g,a,b",
    ":d=4,o=4,b=144:C,D,E,F,G,A,B,c,d,e,f,g,a,b",
    "G:d=4,o=5,b=140:C,d,E,f,G,a,B,c,D,e,F,g,A,b"
}

passed = 0

for tune in tunes:
    result = subprocess.run(["./test_rtttl",tune],
                        capture_output=True,
                        text=True)
    if (result.returncode == 0):
        passed = passed + 1
    print("Input String:\n",result.args[1],"\n",sep="")
    print("Program Output:\n",result.stdout,sep="")

print("Finished running all tests.")
print(passed,"/",len(tunes)," tests passed",sep="")
