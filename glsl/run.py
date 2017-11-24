f = open("input.txt", "r")
ft = f.read()
f.close()

final = ""

for line in ft.split("\n"):
	if(not line.strip() == ''):
		parts = line.split('"')
		final += parts[1] + "\\\n"

of = open("output.txt", "w")
of.write(final)
of.close()