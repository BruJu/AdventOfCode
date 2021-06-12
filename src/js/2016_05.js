const md5 = require('js-md5');
const fs = require('fs');

let c = fs.readFileSync('../../inputs/2016/05_tosolve.txt', 'utf-8');

console.log(c);

function find() {
    const leadingWith = "00000";
    let partA = "";
    let partB = "$$$$$$$$";

    let i = 0;
    while (true) {
        let content = c + i;
    
        const hash = md5(content);
    
        if (hash.startsWith(leadingWith)) {
            const c = hash.charAt(leadingWith.length)
            if (partA.length < 8) partA += c;

            if (c >= '0' && c <= '7') {
                const pos = c - '0';

                if (partB.charAt(pos) === '$') {
                    partB = partB.substr(0, pos) + hash.charAt(leadingWith.length + 1) + partB.substr(pos + 1);
                    console.log(c);
                    console.log(partB);
                    if (partB.indexOf("$") == -1)
                        return [partA, partB];
                }
            }
        }
    
        ++i;
    }
}


const [partA, partB] = find();

console.log(`PartA=${partA} ; PartB=${partB}`);
