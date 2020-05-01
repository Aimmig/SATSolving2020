#!/usr/bin/env python

# Calculate number of variables and clauses for
# checking wether W(2,k) > n

# Using the encoding of the lecture we have:
# - for each integer 1,...n a variable x_i
# - some clauses to exclude all possibility of k equally spaced

# So trivally we have just n variables
def printResult(n, c):
    print("The encoding uses " + str(n) + " variables")
    print("The encoding uees " + str(c) + " clauses")

# Calculating the clauses is a bit tricky
# Dividing the clauses in subset S_r, such that each subset
# exludes just the possibility spaced r appart

# We get n-(k-1)*r clauses for exclusion the r appart 
def spaced_appart(n, k, r):
    return n-(k-1)*r

# The upper for equally spacing k from n
def getMaxSpaced(n,k):
    return (n-1)//(k-1)

# Just some over all partial results for r=1,...
# At the end multiply by 2 because each case needs
# to exclude all var 0 as well as all var 1
def getClauses(n,k):
    r = getMaxSpaced(n,k)
    clauses = 0
    for i in range(1, r+1):
        clauses += spaced_appart(n,k,i)
    return 2*clauses

if __name__ == "__main__":
    n = int(input("Enter n "))
    k = int(input("Enter k "))
    printResult(n,getClauses(n,k))
