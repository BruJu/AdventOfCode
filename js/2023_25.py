import networkx

file = open("../2023/inputs/25_puzzle.txt", 'r')
#file = open("../2023/inputs/25_example.txt", 'r')
lines = file.readlines()

graph = networkx.Graph()

for line in lines:
    from_, tos = line.split(":")

    for to in tos.strip().split(" "):
        graph.add_edge(from_, to)

cut = networkx.minimum_edge_cut(graph)
print(str(cut))
graph.remove_edges_from(cut)

g1, g2 = networkx.connected_components(graph)

print(len(g1) * len(g2))
