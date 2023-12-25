from z3 import *
import re

s = Solver()

file = open("../2023/inputs/24_puzzle.txt", 'r')
#file = open("../2023/inputs/24_example.txt", 'r')

lines = file.readlines()

Variable = Real

x = Variable('x')
y = Variable('y')
z = Variable('z')
dx = Variable('dx')
dy = Variable('dy')
dz = Variable('dz')

i = 0
for line in lines:
    # 20, 19, 15 @  1, -5, -3
    t = Variable('t' + str(i))
    i = i + 1
    
    vals = [int(z) for z in re.findall(r'-?\d+', line)]

    s.add(x + t * dx == vals[0] + t * vals[0 + 3])
    s.add(y + t * dy == vals[1] + t * vals[1 + 3])
    s.add(z + t * dz == vals[2] + t * vals[2 + 3])
    s.add(t >= 0)

print(s.check())

mod = s.model()

print("x= ", str(mod[x]))
print("y= ", str(mod[y]))
print("z= ", str(mod[z]))

print(str(mod[x].as_long() + mod[y].as_long() + mod[z].as_long()))
