import math
import numpy as np

#Set input voltage and potential divider resistance
V0 = 3.3
R0 = 10000

#enter temp and voltage pairs
T1 = float(input("T1"))
T2 = float(input("T2"))
T3 = float(input("T3"))

V1 = float(input("V1"))
V2 = float(input("V2"))
V3 = float(input("V3"))


#calculate resistance of thermistor
R1 = V1 * R0/(V0-V1)
R2 = V2 * R0/(V0-V2)
R3 = V3 * R0/(V0-V3)

print(R1, R2, R3)

#Store resistance and temp values in matrices
arr1 = np.array([[1,math.log(R1),(math.log(R1)**3)],
                 [1,math.log(R2),(math.log(R2)**3)],
                 [1,math.log(R3),(math.log(R3)**3)]])
arr2 = np.array([1/(T1+273.15), 1/(T2+273.15), 1/(T3+273.15)])
print(arr1)
print(arr2)
#Solve matrix equation to get A, B and C
z=np.linalg.solve(arr1,arr2)
A = z[0]
B = z[1]
C = z[2]
print("A=", A)
print("B=", B)
print("C=", C)

#Write A B and C to a text file
f = open("tenpCoefficients.txt", "wt")
coefficients = [str(A), str(B), str(C)]
for l in coefficients:
    f.write("%s \n" % l)
         
f.close()
