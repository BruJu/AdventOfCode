"use strict";

const fs = require('fs');
const input = fs.readFileSync('../../../inputs/2015/12_tosolve.txt', 'utf-8');
const json = JSON.parse(input);

function sum(json, ignoreRed = false) {
    if (typeof json == 'string') {
        return 0;
    } else if (typeof json == 'number') {
        return json;
    } else if (Array.isArray(json)) {
        return json.map(x => sum(x, ignoreRed)).reduce((a, b) => a + b, 0);
    } else {
        if (ignoreRed) {
            if (Object.values(json).indexOf("red") !== -1) {
                return 0;
            }
        }

        return Object.values(json).map(x => sum(x, ignoreRed)).reduce((a, b) => a + b, 0);;
    }
}

console.log(`The answer of part A is ${sum(json)}`);
console.log(`The answer of part B is ${sum(json, true)}`);