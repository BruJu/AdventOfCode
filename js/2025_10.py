from z3 import *
import re

file = open("../2025/inputs/10_real.txt", 'r')

lines = file.readlines()

def solve_machine(line):
    components = line.split(" ")[1:]

    lights_target_value = [int(x) for x in components[-1][1:-1].split(",")]
    lights_target = [0 for x in components[-1].split(",")]

    constraints = []
    to_minimize = 0

    buttons_variables = []
    button_number = 0
    for button in components[0:-1]:
        button_var = Int('hit_' + str(button))
        buttons_variables.append(button_var)
        constraints.append(button_var >= 0)
        to_minimize = to_minimize + button_var
        for light_id in [int(id) for id in button[1:-1].split(",")]:
            lights_target[light_id] = lights_target[light_id] + button_var


    s = Optimize()

    for constraint in constraints:
        s.add(constraint)
    
    s.minimize(to_minimize)

    for i in range(len(lights_target_value)):
        s.add(lights_target_value[i] == lights_target[i])

    if s.check() == sat:
        model = s.model()
        ssum = 0
        for button_variable in buttons_variables:
            ssum += model[button_variable].as_long() 
        return ssum
    else:
        return 0

output = 0

lines = [line.strip() for line in lines if len(line.strip()) != 0]

for i in range(len(lines)):
    if lines[i][0] == '[': # to be able to check with inline file
        output += solve_machine(lines[i])

print(output)
