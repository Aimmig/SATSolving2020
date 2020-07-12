def encodeVar(i,a):
    return 25*(i-1)+a


houses = range(1,6)
attrib = range(1,26)
num_clauses = 25 + 25*5*(5-1)/2 + 25


# each attribute appears
line =''
for a in attrib:
   for i in houses:
      line += str(encodeVar(i,a))+' '
   line+= str(0)
   print(line)
   line =''

#each attribute appers only once
for a in attrib:
    for i in houses:
        for j in range(i+1,6):
           line = '-'+str(encodeVar(i,a)) + ' '+ '-'+str(encodeVar(j,a))
           print(line+' 0')

#each house has each attribute
for i in houses:
   for k in range(1,6):
       x = attrib[5*(k-1):5*k]
       line=''
       for a in x:
          line += str(encodeVar(i,a))+ ' '
       line+= str(0)
       print(line)
       line=''
