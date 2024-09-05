const readline = require('readline');
const {VM} = require("vm2");
const vm = new VM();

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});
 
rl.question('Plesse encode your js_code with base64:   ', (answer) => {
    const buffer = Buffer.from(answer, 'base64');
    const code = buffer.toString('utf8');
    console.log(`\n\ninput js code is: \n\n${code}`);
    console.log(vm.run(code));
    rl.close();
});
