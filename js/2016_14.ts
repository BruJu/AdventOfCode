import md5 from 'js-md5';


type Candidate = {
  id: number;
  repeatedKeys: Set<string>;
};

class KeySearcher {
  readonly salt: string;
  nextId: number = 0;
  keys: number[] = [];
  candidates: Candidate[] = [];

  constructor(salt: string) {
    this.salt = salt;
  }

  has64Keys(): boolean {
    return this.keys.length >= 64;
  }

  considerNext() {
    const n = this.nextId++;
    const hash = md5(this.salt + n);

    if (this.candidates.length > 0 && this.candidates[0].id + 1000 < n) {
      this.candidates.shift();
    }

    const repeated3Times = new Set<string>();
    const repeated5Times = new Set<string>();

    let current: string | null = null;
    let cnt = 0;
    for (let i = 0; i != hash.length; ++i) {
      if (current === hash[i]) {
        ++cnt;
        if (cnt === 3) repeated3Times.add(current);
        if (cnt === 5) repeated5Times.add(current);
      } else {
        current = hash[i];
        cnt = 1;
      }
    }

    if (n === 39 || n === 816) {
    console.log(repeated3Times);
    console.log(repeated5Times);
    }

    for (const validator of repeated5Times) {
      let sort = false;
      let iCandidate = 0;
      while (iCandidate !== this.candidates.length) {
        if (this.candidates[iCandidate].repeatedKeys.has(validator)) {
          let c = this.candidates[iCandidate].id;
          this.candidates.splice(iCandidate, 1);
          this.keys.push(c);
          console.log(c);
          sort = true;
        } else {
          ++iCandidate;
        }
      }
      
      if (sort) this.keys.sort((lhs, rhs) => lhs - rhs);
    }

    if (repeated3Times.size > 0) {
      this.candidates.push({ id: n, repeatedKeys: repeated3Times });
    }
  }
}


let ks = new KeySearcher("abc");

while (!ks.has64Keys()) {
  ks.considerNext();
}

for (let i = 0; i != 5001; ++i) {
  ks.considerNext();
}

console.log(ks.keys);
console.log(ks.keys.length);
console.log(ks.keys[65]);

