from z3 import *
import re

file = open("../2024/inputs/13_real.txt", 'r')

lines = file.readlines()

def solve_machine(button_a_line, button_b_line, target_line):
    s = Optimize()

    hit_a = Int('hit_a')
    hit_b = Int('hit_b')

    [a_x, a_y] = [int(z) for z in re.findall(r'-?\d+', button_a_line)]
    [b_x, b_y] = [int(z) for z in re.findall(r'-?\d+', button_b_line)]
    [target_x, target_y] = [int(z) for z in re.findall(r'-?\d+', target_line)]
    
    s.add(hit_a * a_x + hit_b * b_x == (10000000000000 + target_x) )
    s.add(hit_a * a_y + hit_b * b_y == (10000000000000 + target_y) )
    s.add(hit_a >= 0)
    s.add(hit_b >= 0)
    s.minimize(hit_a * 3 + hit_b)

    if s.check() == sat:
        model = s.model()
        return model[hit_a].as_long() * 3 + model[hit_b].as_long()
    else:
        return 0

output = 0

lines = [line.strip() for line in lines if len(line.strip()) != 0]

for i in range(0, len(lines), 3):
    output += solve_machine(lines[i], lines[i + 1], lines[i + 2])

print(output)
