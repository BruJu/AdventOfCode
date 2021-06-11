const md5 = require('md5');
const fs = require('fs');

               // js 2015 src
let c = fs.readFileSync('../../../inputs/2015/04_tosolve.txt', 'utf-8');

console.log(c);

function find(leadingWith) {
    let i = 0;
    while (true) {
        let content = c + i;
    
        const hash = md5(content);
    
        if (hash.startsWith(leadingWith)) {
            return i;
        }
    
        ++i;
    }
}



console.log("The answer of part A is " + find("00000"));
console.log("The answer of part B is " + find("000000"));
