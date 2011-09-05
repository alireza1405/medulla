#! /bin/python
# Reads in a .csv file and removes any redundent entries and re orders the parts.
#  Input file must be tab delimited with " text delmiters.

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

ref_dict = {}
type_dict = {}
manu_dict = {}
desc_dict= {}
digi_dict = {}
#tek_dict = {}
pack_dict = {}
count_dict = {}

nums = []

for i in range(9) :
	line = f.readline()

while line:

	s = line.split("\t")
	print "\t" + s[0]
	print "\t" + s[1]

	if s[0] == '"Subtotal"' :
		print "\tGAHHH"
		line = f.readline()

	else :
		
		print s

		num = s[2]

		if not ref_dict.has_key(num) :
			type_dict[num] = s[0].strip('"1234567890,')
			ref_dict[num] = s[0].strip('"')[len(type_dict[num]):]
			manu_dict[num] = s[1]
			desc_dict[num] = s[3]
			digi_dict[num] = s[6]
#			tek_dict[num] = s[7]
		else :
			ref_dict[num] = ref_dict[num] + ',' + s[0].strip('"')[1:]


		if (num not in nums) and (type_dict[num] != '') :
			nums.append(num)

	line = f.readline()					# Read the next line in the file

f.close								# Close the file

print "\n\n\n\n\n\n\n"
print type_dict
print "\n\n\n\n\n\n\n"
print ref_dict
print "\n\n\n\n\n\n\n"
print manu_dict
print "\n\n\n\n\n\n\n"
print desc_dict
print "\n\n\n\n\n\n\n"
print digi_dict
print "\n\n\n\n\n\n\n"


nums.sort()

f = open(sys.argv[1]+".list.csv", 'w')

f.write("Ref. Des.,Manufacture,Manufacture #,Description,Digikey #,Quantity\n")

for num in nums :

	count_dict[num] = 0
	temp = ref_dict[num].strip('"').split(',')
	count_dict[num] = len(temp)

	print temp
	temp.sort(lambda a,b: cmp(int(a), int(b)))

	ref_dict[num] = temp[0]
	for ref in temp[1:] :
		ref_dict[num] = ref_dict[num]+','+ref

#	print num + "\t" + type_dict[num] + "\t" + ref_dict[num] + "\t" + str(count_dict[num])
	f.write( '"'+type_dict[num]+ref_dict[num]+'",'+manu_dict[num]+','+num+','+desc_dict[num]+','+digi_dict[num]+','+str(count_dict[num])+'\n' )
f.close

print "\n\ndone!\n\n"

