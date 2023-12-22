// Provide the graphviz representation of the puzzle

// Observations:
// CONJ pk, CONJ hf, CONJ pm, CONJ mk are linked to CONJ vi
// CONJ vi only element connected to rx
// the parts of the graph connected to pk, hf, pm, mk are disjoint.


const fs = require('fs');

const lines = fs.readFileSync("../inputs/20_puzzle.txt", "utf-8");

const all_nodes = new Map();

for (const line of lines.split(/\r?\n/)) {
  const boop = line.split(" ");

  all_nodes.set(boop[0], {});
}

function find_real_name(name) {
  if (all_nodes.has(name)) return name;
  if (all_nodes.has("&" + name)) return "&" + name;
  if (all_nodes.has("%" + name)) return "%" + name;

  all_nodes.set(name, {});
  return name;
}

function trad(name) {
  if (name[0] == "&") return "CONJ " + name.slice(1);
  if (name[0] == "%") return "FLIP " + name.slice(1);
  return name;
}

for (const line of lines.split(/\r?\n/)) {
  const boop = line.split(" ");

  let tgt = boop.slice(2);
  for (const target of tgt) {
    if (target.includes(",")) {
      console.log(`"${trad(boop[0])}" -> "${trad(find_real_name(target.substring(0, target.length - 1)))}"`);
    } else {
      console.log(`"${trad(boop[0])}" -> "${trad(find_real_name(target.substring(0, target.length)))}"`);
    }
  }
}
