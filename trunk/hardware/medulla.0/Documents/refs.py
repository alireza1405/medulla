#!/bin/python
import sys
import os



try :
	f = open(sys.argv[1], "r")
except IOError :
	print 'No such file: ' + sys.argv[1]
	sys.exit(2)
except IndexError :
	print 'Please specify a file'
	sys.exit(2)

fo = open(sys.argv[1]+".csv", 'w')
fo.write("Part,Refrence\n")


refs = []
part = []
type = ""


for i in range(8) :
	line = f.readline()

r = 0;
while line:

	s = line.split("\t")

	if (s[0] == 'Subtotal') or (s[0] == ''):
		line = f.readline()
	else :

		temp = s[0].strip('"')
		for i in temp.split(",") :
			if i[0] not in ['1','2','3','4','5','6','7','8','9','0'] :
				type = i[0]
				refs.append(i)
			else :
				refs.append(type + i)
				
			if s[7] == '' :
				part.append(s[6])
			else :
				part.append(s[7])

			fo.write(part[r]+","+refs[r]+"\n")
			r += 1;



	line = f.readline()					# Read the next line in the file

f.close								# Close the file
fo.close

print "\n\ndone!\n\n"

