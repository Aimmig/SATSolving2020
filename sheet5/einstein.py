def encodeVar(i,a):
    return 25*(i-1)+a

NUM_FACTS = 2
NUM_IMPL = 8
NUM_NEIG = 4
NUM_NEIG_ONESIDE = 1
additional_constraints = 5*NUM_IMPL + NUM_FACTS + 5*NUM_NEIG + 4*NUM_NEIG_ONESIDE

houses = range(1,6)
attrib = range(1,26)
num_clauses = 25 + 25*5*(5-1) + 25
num_clauses += additional_constraints
num_vars = 125

# 1-5   nationality
ENG = 1
GER  = 2
NOR  = 3
DEN  = 4
SWE  = 5
# 6-10  color of house
RED = 6
BLU = 7
GRE = 8
WHT = 9
YEL = 10
#11-15  pet
BIR = 11
CAT = 12
DOG = 13
FIS = 14
HOR = 15
#16-20  cigaret
PAL = 16
DUN = 17
PRI = 18
MAS = 19
BLE = 20
#21-25  drink
TEE = 21
BEE = 22
WAT = 23
MIL = 24
COF = 25

# enforces S_i^a
def enforceFact(i,a):
    print(encodeVar(i,a),0)

# attrib a1 implies a2 for neighbour
def enforceNeighbour(a1, a2, onlyLeft=False):
    for i in [2,3,4]:
        if onlyLeft:
            print(str(-encodeVar(i,a1)),str(encodeVar(i+1,a2)),0)
        else:
        # i with a1 implies a2 at i-1 or i+1
            print(str(-encodeVar(i,a1)),str(encodeVar(i-1,a2)),str(encodeVar(i+1,a2)),0)
    # left & right border
    if onlyLeft:
        print(str(-encodeVar(1,a1)),str(encodeVar(2,a2)),0)
    else:
        print(str(-encodeVar(1,a1)),str(encodeVar(2,a2)),0)
        print(str(-encodeVar(5,a1)),str(encodeVar(4,a2)),0)

# attrib a1 implies a2
def enforceImplication(a1,a2):
    for i in houses:
       cond = str(-encodeVar(i,a1)) + ' '+str(encodeVar(i,a2))
       print(cond,0)

print("p cnf",num_vars, num_clauses)

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
           line = str(-encodeVar(i,a)) + ' '+ str(-encodeVar(j,a))
           print(line, 0)

#each house has each attribute
for i in houses:
   for k in range(1,6):
       x = attrib[5*(k-1):5*k]
       line=''
       for a in x:
          line += str(encodeVar(i,a))+ ' '
       print(line, 0)
       line=''

# each house has only one attribute of each type
for i in houses:
   for k in range(1,6):
       x = attrib[5*(k-1):5*k]
       for a1 in x:
          for a2 in range(a1+1,1+5*k):
              print(str(-encodeVar(i,a1))+' '+str(-encodeVar(i,a2))+' '+str(0))





enforceImplication(ENG,RED)
enforceImplication(SWE,DOG)
enforceImplication(DEN,TEE)
enforceNeighbour(GRE,WHT,True)
enforceImplication(GRE,COF)
enforceImplication(PAL,BIR)
enforceImplication(YEL,DUN)
enforceFact(3,MIL)
enforceFact(1,NOR)
enforceNeighbour(BLE,CAT)
enforceImplication(MAS,BEE)
enforceNeighbour(HOR,DUN)
enforceImplication(GER,PRI)
enforceNeighbour(NOR,BLU)
enforceNeighbour(BLE,WAT)
