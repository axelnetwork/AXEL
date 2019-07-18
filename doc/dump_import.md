## Short introduction into dumping to and importing from crypto wallet

### General info
By default physically the wallet exists as a standalone _/home/&lt;user&gt;/.&lt;coin&gt;/wallet.dat_ in the form of [LevelDB](https://github.com/PIVX-Project/PIVX/tree/master/src/leveldb) file, where _&lt;coin&gt;_ is a coin symbol under _&lt;user&gt;_ account, i. e. for example _~/.bltg/wallet.dat_ is a wallet file for _BLTG_ coin under your current user.

It's possible to "export" every single private key with using "_dumpprivkey &lt;axeladdress&gt;_" command and "import" it back with "_importprivkey &lt;axeladdress&gt;_" command.

Also, it's possible to "export" the whole wallet (i. e. all private keys) into plain-text file by "_dumpwallet &lt;filename&gt;_" command, and "import" them back with "_importwallet &lt;filename&gt;_" command.

Useful commands ([full list](https://github.com/PIVX-Project/PIVX/wiki/API-Calls-List#full-list)):

| Command       | Parameters       | Description  |
| ------------- | ---------------- | ------------ |
|_&lt;getwalletinfo&gt;_  |                    |Returns an object containing various wallet state info|
|_&lt;importwallet&gt;_   |_&lt;filename&gt;_  |Imports keys from a wallet dump file (see dumpwallet)|
|_&lt;dumpwallet&gt;_     |_&lt;filename&gt;_  |Dumps all wallet keys in a human-readable format|
|_&lt;importprivkey&gt;_  |_&lt;pivxprivkey&gt; [label] [rescan=true]_|Adds a private key (as returned by dumpprivkey) to your wallet|
|_&lt;dumpprivkey&gt;_    |_&lt;pivxaddress&gt;_|Reveals the private key corresponding to 'pivxaddress'|


### Examples of usage
#### Backup wallet
```shell
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ./src/bltg-cli backupwallet my_bltg_backup
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ls -la my_bltg_backup
-rw------- 1 vit vit 671744 Feb 27 18:49 my_bltg_backup
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ # original wallet file
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ls -la ~/.bltg/wallet.dat
-rw------- 1 vit vit 671744 Feb 27 19:48 /home/vit/.bltg/wallet.dat
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ file ~/.bltg/wallet.dat
/home/vit/.bltg/wallet.dat: Berkeley DB (Btree, version 9, native byte-order)
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $
```

#### Getting wallet info
```shell
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ./src/bltg-cli getwalletinfo
{
  "walletversion": 61000,
  "balance": 10.00300000,
  "txcount": 4,
  "keypoololdest": 1551286349,
  "keypoolsize": 1001
}
```


#### Dumping wallet into plain-text file
```shell
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ./src/bltg-cli dumpwallet my_temp_dump && cat my_temp_dump
# Wallet dump created by BLTG v2.0.0.0-9d950ff-dirty (2018-12-15 00:34:02 +0000)
# * Created on 2019-02-27T16:53:38Z
# * Best block at time of backup was 186028 (986a423cafdd0a5c93109ef6144d1cf154e15d00be6a2d54279e7a4ff4cab4e4),
#   mined on 2019-02-27T16:53:31Z

~...~
NZiZWaRu9j3EZVij6Bd4JJ3ys7ezQekGTxEUAYYrzYhjmRajTSBx 2018-11-08T21:04:56Z change=1 # addr=BMMHWVtzNTHNrJ4yZVBa1wdwD6AWVXjkRv
NWcPXpgBRAAxPPSpiJSme9JiBgFZeLSjur18ncq9Bf5JX7HC5vmK 2018-11-08T21:04:56Z change=1 # addr=BNAoFP8gEBLjzrCCTYCptdb4PayHngGQyw
NWagXtMKHmuBbNPp5Zomkx1SsGf3eASNoHapTkFkSjwH5ZqPGmfh 2018-11-08T21:04:56Z change=1 # addr=BHCbUGYtSKv5j8kRgkAnqbtycCo2M3ufFn
NW79X2P8gSSfBv2asiw2kvd1YyyXZXs2LN1VgUKosqLdQ8BTiPhg 2018-11-08T21:04:56Z change=1 # addr=BAn4w3LJPMbGKt6uApiED3rtr8JLeKApT8
~...~
```


#### Importing wallet from previous wallet dump
```shell
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ./src/bltg-cli importwallet my_temp_dump 
```
In log file (_~/.bltg/debug.log_ for _BLTG_) we can see something like:
```shell
2019-02-27 16:50:18 Skipping import of B4g3ZZAMfExxrt5pATfLvZwwzBSSXe6BTi (key already present)
2019-02-27 16:50:18 Skipping import of BAR7LpkpWn3BmRWxqvNXVKt6w77GNWFhTi (key already present)
2019-02-27 16:50:18 Skipping import of B8rSn8ivker2Kp2UxxBJePdZmiQQSyux3c (key already present)
2019-02-27 16:50:18 Skipping import of BJeQEeURDiH9hhEXHgcmvaQpHPYQchHqPY (key already present)
```
Or, as below:
```shell
2019-02-27 16:52:43 Importing BMznuNUxfrrZ6gCWChJrgCGnrZr9DydpAx...
2019-02-27 16:52:43 Importing BR4tZRcXbzRtiaiXptamCRPo2HYfAr7LgD...
2019-02-27 16:52:43 Importing BAYxH6Hq1C1EA5DvUJCGvek5fyfE7YF1GV...
2019-02-27 16:52:43 Importing BKDwrjpsTLkgjdQ3nzQjS2y3JSUHLx7VPD...
```

#### (Dirty) private keys importing after previous wallet dump
```shell
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ cat my_temp_dump | awk '{ split($0, a, " "); system("./src/bltg-cli importprivkey "a[1]) }'
```
In log file we can see:
```shell
2019-02-27 17:17:46 AddToWallet 3e79b838f80b0a14d92aa5e6c73e4c00a4073199449775b263f88d696eebf595  new
2019-02-27 17:17:46 AddToWallet 2ca3ed1d400ad997d76e913de103cb09f30c5796014f37388d77238e1b273dfb  new
2019-02-27 17:17:46 AddToWallet f1cd4c5cf708ddf796360f443d16e00109e6710e214e3db71639c8e616e5d945  new
2019-02-27 17:17:46 AddToWallet 2b31bfc5fbad30b585cb4afdc9793f73a0425e90b6bdabffe083c93906360a90  new
```


#### Dumping some particular private key
```shell
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ ./src/bltg-cli dumpprivkey BHArW7eiUsBytV4n8D9t4d1EQr3kSzNWek
NU58ZfMzogxEK1XRHrko6SrBCs4v7usR6J69kLjomEpRvGFRyWsw
vit@evergreen ~/prjs/axel/gitlab.stoamigo.com/axel-as-bltg $ 
```
