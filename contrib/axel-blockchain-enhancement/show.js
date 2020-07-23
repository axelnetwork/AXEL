//
// Usage:
//   node ./show.js --rpcuser=<your_rpcuser> --rpcpassword=<your_rpcpassword>
//

const RPC = require('./rpc')
const argv = require('argv')

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
            name: 'number',
            type: 'int',
            description: 'Number of blocks go through',
        }
    ])
    let defaultOptions = {
        rpcport: '42325',   // testnet, 32325 - mainnet
        number: 1000
    }
    let args = argv.run()
    let options = Object.assign(defaultOptions, args.options)
    return options
}

function connect(options) {
    let url = 'http://' + options.rpcuser + ':' + options.rpcpassword + '@127.0.0.1:' + options.rpcport
    return new RPC.RPC(url)
}

async function getTransaction(rpc, txid) {
    try {
        let transaction = await rpc.rawCall('getrawtransaction', [txid, 1])
        return transaction
    } catch (e) {
        console.log("'getrawtransaction' failed: ${method}");
    }

    try {
        let transaction = await rpc.rawCall('gettransaction', [txid])
        return await rpc.rawCall('decoderawtransaction', [transaction.hex])
    } catch (e) {
        console.log("'decoderawtransaction' failed: ${method}");
    }
}

function hexToText(hex) {
    return Buffer.from(hex, 'hex').toString('utf8')
}

async function show(rpc, options) {
    let blockhash = await rpc.rawCall('getbestblockhash')
    for (let i = 0; i < options.number; i++) {
        let block = await rpc.rawCall('getblock', [blockhash])
        blockhash = block.previousblockhash
        if (block.tx.length < 3) {
            continue
        }

        for (let j = 2; j < block.tx.length; j++) { // skip coinbase and coinstake
            let txid = block.tx[j]
            let transaction = await getTransaction(rpc, txid)
            if (transaction === undefined) {
                continue
            }

            let outs = transaction.vout
            for (let k = 0; k < outs.length; k++) {
                let out = outs[k]

                if (out.scriptPubKey.asm.substring(0, 9) === 'OP_RETURN') {
                    // console.log('output:\n%o', out)

                    let text = hexToText(out.scriptPubKey.asm.substring(10))
                    console.log({
                        txid: txid,
                        vout: k,
                        script: out.scriptPubKey.asm,
                        text: text
                    })
                }
            }
        }
    }
}

async function run() {
    const options = parseArgv()
    // console.log('options:\n%o', options)
    const rpc = connect(options)
    // console.log('AXEL RPC:\n%o', rpc)
    await show(rpc, options)
}

run().then()
