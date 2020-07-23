//
// Usage:
//   node ./verify-file.js --rpcuser=<your_rpcuser> --rpcpassword=<your_rpcpassword> --address=<your_address> --sig=<your_sig> --file=<your_file>
//

const RPC = require('./rpc')
const argv = require('argv')
const fs = require('fs')

function parseArgv() {
    argv.option([
        {
            name: 'rpcuser',
            type: 'string',
            description: 'Username for JSON-RPC connections of AXEL',
        },
        {
            name: 'rpcpassword',
            type: 'string',
            description: 'Password for JSON-RPC connections of AXEL',
        },
        {
            name: 'rpcport',
            type: 'string',
            description: 'Port for JSON-RPC connections of AXEL',
        },
        {
            name: 'address',
            type: 'string',
            description: 'AXEL address to use for signing',
        },
        {
            name: 'sig',
            type: 'string',
            description: "File's signature",
        },
        {
            name: 'file',
            type: 'string',
            description: 'File to sign',
        },
    ])
    let defaultOptions = {
        rpcport: '42325'   // testnet, 32325 - mainnet
    }
    let args = argv.run()
    let options = Object.assign(defaultOptions, args.options)
    if (!options.address) {
        console.log('AXEL address must be present')
        return
    }
    if (!options.sig) {
        console.log("File's signature must be present")
        return
    }
    if (!options.file) {
        console.log('File must be present')
        return
    }
    return options
}

function connect(options) {
    let url = 'http://' + options.rpcuser + ':' + options.rpcpassword + '@127.0.0.1:' + options.rpcport
    return new RPC.RPC(url)
}

function getFileContent(file) {
    return fs.readFileSync(file, { encoding: 'base64' })
}

async function verify(rpc, options) {
    content = getFileContent(options.file)
    // console.log("content:\n", content)
    return await rpc.rawCall('verifymessage', [options.address, options.sig, content])
}

async function run() {
    const options = parseArgv()
    if (options === undefined) {
        console.log('Use -h to get a help about needed args')
        return
    }
    // console.log('options:\n%o', options)
    const rpc = connect(options)
    // console.log('AXEL RPC:\n%o', rpc)
    const verified = await verify(rpc, options)
    console.log("'%s' file is verified:\n%o", options.file, verified)
}

run().then()
