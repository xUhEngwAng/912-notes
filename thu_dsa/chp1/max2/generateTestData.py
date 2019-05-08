from numpy import random

f = open(r'testData.txt', 'w')

f.write(str(random.randint(10000)))
for ix in range(10000):
	f.write('\n' + str(random.randint(10000))) 
	
f.close()

