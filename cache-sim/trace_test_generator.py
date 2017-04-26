import random
import sys
import string
if (len(sys.argv) < 3):
	y = input("Enter the number of accesses desired- ")
	z = str(input("Enter the file name- "))
else:
	z = sys.argv[1]
	try:
		y = int(sys.argv[2])
	except:
		print("Not a Number")
		exit()
a = open(z,"w")
def ret_hex_str ( x ):
	return ''.join(random.choice('abcdef' + string.digits) for _ in range(x))
for _ in range(int(y)):
	c = random.choice('012')
	if (c=='2'):
		a.write(c+" ")
		a.write(ret_hex_str(6))
		a.write('\n')
	else:
		a.write(c+" ")
		a.write(ret_hex_str(8))
		a.write('\n')
a.close()