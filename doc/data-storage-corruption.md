This document describes how Axel Core stores blockchain data, how unexpected database corruption happen when killing the application process(AGN-2908) and what to do with it.

# Background
## There are basically four pieces of data that are maintained:

* blocks/blk*.dat: the actual blocks, in network format, dumped in raw on disk. They are only needed for rescanning missing transactions in a wallet, reorganizing to a different part of the chain, and serving the block data to other nodes that are synchronizing.
* blocks/index/*: this is a LevelDB database that contains metadata about all known blocks, and where to find them on disk. Without this, finding a block would be very slow.
* chainstate/*: this is a LevelDB database with a compact representation of all currently unspent transaction outputs and some metadata about the transactions they are from. The data here is necessary for validating new incoming blocks and transactions. It can theoretically be rebuilt from the block data (see the -reindex command line option), but this takes a rather long time. Without it, you could still theoretically do validation indeed, but it would mean a full scan through the blocks for every output being spent.
* blocks/rev*.dat: these contain "undo" data. You can see blocks as 'patches' to the chain state (they consume some unspent outputs, and produce new ones), and see the undo data as reverse patches. They are necessary for rolling back the chainstate, which is necessary in case of reorganisations.


## How data is read and wrote:

The entire database is loaded into memory on startup. See LoadBlockIndexDB in main.cpp. This only takes a few seconds.

Blocks are written to disk as soon as they are received, in AcceptBlock, see WriteBlockToDisk. And Info about the block files is stored in the block index (the LevelDB).

Blocks stored in the LevelDB database are represented in memory as CBlockIndex objects. When headers are received over the network, they are streamed into a vector of CBlockHeaders, which are then checked. Each header that checks out causes a new CBlockIndex to be created, which is stored to the database. But before flushing, the data is still in the cache.

Flush() is called from FlushStateToDisk. FlushStateToDisk is invoked at a few different points, with a given mode (IF_NEEDED, ALWAYS, PERIODIC). The idea is to flush the block cache frequently (to avoid having to download a large number of blocks if the program crashes), but the coins cache infrequently (in order to maximize the benefit from the coins cache.)

Specifically, the block cache is guaranteed to be flushed once an hour, whereas the coins cache once per day.

# Problems

## Corrupted block database detected after Axel application killed
In this case, the shutdown can be detected then flush the database before exit. The problem is the wrong thread group manipulation. The thread group should be interrupted and joined after everything flushed and shutdown instead of before that. Adjust the place of thread group manipulation could fix this. The fix involves refactor of thread group and shutdown detect.

## Corrupted block database detected after Axel process killed
In this case, the shutdown can not be detected, and the database cache lost inevitably. At the next startup, the block data and the database is out-of-sync of course. When loading the database (in LoadBlockIndexDB), it try to repair this kind of inconsistence with a comlicated logic. But the strategy is too aggressive. It does not repair the problem, but even cleaned the existed block index db to height 1 under this situation, which casued the later block database detected error due to inconsistency between block index db and undo data. Adjust the repair strategy could fix this.



# Further Improvements
The current solution gave up some block data already got in case 2. Those blocks need to be downloaded again, this is somewhat waste. A better solution could be make best use of the block data on disk and go back as near as possible.

