### CLI:

```
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ ls -lat
total 36
drwxrwxr-x  3 vit vit 4096 Apr 16 05:22 .
drwxrwxr-x  4 vit vit 4096 Apr 16 05:21 ..
-rw-rw-r--  1 vit vit  321 Apr 16 05:20 package.json
-rw-rw-r--  1 vit vit 3223 Apr 16 05:19 send.js
-rw-rw-r--  1 vit vit 2550 Apr 16 05:18 rpc.js
-rw-rw-r--  1 vit vit 3005 Apr 16 05:09 show.js
-rw-rw-r--  1 vit vit   60 Apr 16 04:44 axel-blockchain-enhancement.code-workspace
-rw-rw-r--  1 vit vit 2511 Apr 16 04:43 package-lock.json
drwxrwxr-x 10 vit vit 4096 Apr 16 04:43 node_modules
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ date && node ./send.js --rpcuser=<my_rpcuser> --rpcpassword=<my_rpcpassword> --msg="Hello, AXEL, from Vi+"
Thu Apr 16 05:24:18 EEST 2020
UTXO:
{
  txid: '0a3852528e82529ef903afcf79e5bc04d1c00db6744775a91b8f155ce772b809',
  vout: 5,
  address: 'aeVKY9f13KnKm9uDqUBqu4gvYo9E5TeQTr',
  account: 'first-batch-testnet-wallet2',
  scriptPubKey: '2102607de98c75639e45d6645bef03dc68c71fe3903c84f4287a83316e13adae7692ac',
  amount: 2500002.63,
  confirmations: 21,
  spendable: true
}
raw tx:
 010000000109b872e75c158f1ba9754774b60dc0d104bce579cfaf03f99e52828e5252380a0500000000ffffffff020000000000000000176a1548656c6c6f2c204158454c2c2066726f6d2056692b401946b85fe300001976a9142cbfc2a9a2404cd1c2223468d76dbe0a0f76513188ac0000000000
transaction:
'87ba9d6121714bee93dc3723377840c55679a80917be04c0b4b5b27a0ae1bb7c'
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ # wait a minute or two
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ date && node ./show.js --rpcuser=<my_rpcuser> --rpcpassword=<my_rpcpassword>
Thu Apr 16 05:26:23 EEST 2020
{
  txid: '87ba9d6121714bee93dc3723377840c55679a80917be04c0b4b5b27a0ae1bb7c',
  vout: 0,
  script: 'OP_RETURN 48656c6c6f2c204158454c2c2066726f6d2056692b',
  text: 'Hello, AXEL, from Vi+'
}
{
  txid: '9438cd76242a68738f15fb38fd916277358d992aebb322340c13076a85dc2991',
  vout: 0,
  script: 'OP_RETURN 48656c6c6f2c204158454c',
  text: 'Hello, AXEL'
}
{
  txid: 'b020c0ca3f54064ef635046e13a3729c6b9a4f00a7f27e8374d38cc08458113d',
  vout: 0,
  script: 'OP_RETURN 48656c6c6f2c204158454c',
  text: 'Hello, AXEL'
}
{
  txid: '8b15a0d3f977202c3de5c5b7d96ab9c45721f8f1050b53d09df27096a4a3cd5a',
  vout: 0,
  script: 'OP_RETURN 56692b203c33204158454c',
  text: 'Vi+ <3 AXEL'
}
➜  axel-blockchain-enhancement git:(v1.3.1) ✗
```


### Blockchain log:

```
~...~
2020-04-16 02:24:18 AddToWallet 87ba9d6121714bee93dc3723377840c55679a80917be04c0b4b5b27a0ae1bb7c  new
~...~
2020-04-16 02:25:30 UpdateTip: new best=ff4de54ce4b1dae919d9b0568c245e1d51cf708ef3537ba937206f92145bdb3a  height=232442  log2_work=71.680426  tx=553917  date=2020-04-16 02:26:04 progress=1.000000  cache=91
2020-04-16 02:25:30 AddToWallet 87ba9d6121714bee93dc3723377840c55679a80917be04c0b4b5b27a0ae1bb7c  update
2020-04-16 02:25:30 ProcessNewBlock : ACCEPTED in 15 milliseconds with size=690
~...~
```


### File signing/verifying based on *signmessage* and *verifymessage* AXEL blockchain API calls:

```
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ date && node ./sign-file.js --rpcuser=<my_rpcuser> --rpcpassword=<my_rpcpassword> --address=aQMzgHk9PCp2ZD1QPBT7o6gGKKAKQ5Wm6r --file=./demo-testnet.png
Mon Apr 20 20:01:30 EEST 2020
signature for further verifying of './demo-testnet.png' file:
'ICnFJFJLTk8ea5JulrwSU0wrd8RSvm79EKwlQTebVAZIWMb1fq5/X8biQ/GZyVwh+MluXTRMv0+z0Qn912mTNJM='
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ date && node ./verify-file.js --rpcuser=<my_rpcuser> --rpcpassword=<my_rpcpassword> --address=aQMzgHk9PCp2ZD1QPBT7o6gGKKAKQ5Wm6r --sig=ICnFJFJLTk8ea5JulrwSU0wrd8RSvm79EKwlQTebVAZIWMb1fq5/X8biQ/GZyVwh+MluXTRMv0+z0Qn912mTNJM= --file=./demo-testnet.png
Mon Apr 20 20:02:25 EEST 2020
'./demo-testnet.png' file is verified:
true
➜  axel-blockchain-enhancement git:(v1.3.1) ✗ ls -la ./demo-testnet.png
-rw-rw-r-- 1 vit vit 89538 Apr 16 13:08 ./demo-testnet.png
➜  axel-blockchain-enhancement git:(v1.3.1) ✗
```
