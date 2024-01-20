import os

#STR = "BR_RESIDENT_ENTRY "
#STR = "BR_ASM_CALL "
STR= "BR_PUBLIC_ENTRY "

fpaa = open("x.def", "w")

for root, dirs, files in os.walk("inc"):
	for name in files:
		path = os.path.join(root, name)
		
		with open(path, "r") as fp:
			m = fp.readlines()
			for x in m:
				e = x.find(STR)
				if e == -1:
					continue
				
				ww = x[e + len(STR):]

				ee = ww.find("(")
				if ee != -1:
					ww = ww[:ee]
				
				fpaa.write("\t")
				fpaa.write(ww)
				fpaa.write("\n")
				