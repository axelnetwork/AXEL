//
// Usage:
//   node ./send.js --rpcuser=<your_rpcuser> --rpcpassword=<your_rpcpassword> --msg=<your_msg>
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
            name: 'gas',
            type: 'float',
            description: 'Gas used to pay the transaction',
        },
        {
            name: 'msg',
            type: 'string',
            description: 'Message to be sent, no more than 80 byte',
        },
    ])
    let defaultOptions = {
        rpcport: '42325',   // testnet, 32325 - mainnet
        gas: 0.1
    }
    let args = argv.run()
    let options = Object.assign(defaultOptions, args.options)
    if (!options.msg) {
        console.log('Message must be set')
        return
    }
    options.msgHexStr = strToHexStr(options.msg)
    if ((options.msgHexStr.length / 2) > 80) {
        console.log('Message must be no more than 80 byte')
        return
    }
    return options
}

function connect(options) {
    let url = 'http://' + options.rpcuser + ':' + options.rpcpassword + '@127.0.0.1:' + options.rpcport
    return new RPC.RPC(url)
}

function strToHexStr(str) {
    return Buffer.from(str).toString('hex')
}

async function getUtxo(rpc, gas) {
    let list = await rpc.rawCall('listunspent')
    for (let i = 0; i < list.length; i++) {
        let utxo = list[i];
        if (utxo.amount > gas) {
            console.log('UTXO:\n%o', utxo)
            return utxo;
        }
    }

    console.log('No UTXO avaliable')
    return null
}

function getChange(amount, gas) {
    return (amount * 1e8 - gas * 1e8) / 1e8
}

async function createTransaction(rpc, msg, gas, utxo, changeAddress) {
    let data = [
        [{ "txid": utxo.txid, "vout": utxo.vout }],
        { "data": msg, [changeAddress]: getChange(utxo.amount, gas) }
    ]
    return await rpc.rawCall('createrawtransaction', data)
}

async function send(rpc, options) {
    let utxo = await getUtxo(rpc, options.gas)
    let changeAddress = await rpc.rawCall('getrawchangeaddress')
    let rawTx = await createTransaction(rpc, options.msgHexStr, options.gas, utxo, changeAddress)
    console.log("raw tx:\n", rawTx)
    rawTx = await rpc.rawCall('signrawtransaction', [rawTx])
    await rpc.rawCall('sendrawtransaction', [rawTx.hex])
    return await rpc.rawCall('decoderawtransaction', [rawTx.hex])
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
    const transaction = await send(rpc, options)
    console.log('transaction:\n%o', transaction.txid)
}

run().then()
