# Script for plotting 
# extract data from command: nvidia-smi dmon -s puct -o DT -c 60 -f data.txt

import matplotlib.pyplot as plt

def extractData():
    startDate = []
    util = []
    mclk = []
    pclk = []
    rxpci = []
    txpci = []

    with open('data.txt', 'r') as f:
        lines = f.readlines()
    
    for line in lines:
        cur = line.split(' ')
        # skip header lines
        if cur[0].startswith('#'):
            continue
        # remove spaces and get clear data
        else:
            cur = removeSpace(cur)
            startDate.append(cur[0])
            util.append(int(cur[6]))
            mclk.append(int(cur[10]))
            pclk.append(int(cur[11]))
            rxpci.append(int(cur[12]))
            txpci.append(int(cur[13]))

    return (startDate, util, mclk, pclk, rxpci, txpci)
        
def removeSpace(charlist):
    while '' in charlist:
        charlist.remove('')
    return charlist
    
def multiPlots(meta):
    startDate = meta[0][0]
    util = meta[1]
    mclk = meta[2]
    pclk = meta[3]
    rxpci = meta[4]
    txpci = meta[5]

    # Add subplot for data
    _, axis = plt.subplots(2, 3, constrained_layout = False)
    plt.suptitle(f'Date recorded: {startDate}')

    axis[0, 0].plot(range(len(util)), util)
    axis[0, 0].set_title('GPU Util (%)')

    axis[0, 1].plot(range(len(mclk)), mclk)
    axis[0, 1].set_title('mclk (MHz)')

    axis[0, 2].plot(range(len(pclk)), pclk)
    axis[0, 2].set_title('pclk (MHz)')

    axis[1, 0].plot(range(len(rxpci)), rxpci, color='g', label='read')
    axis[1, 0].set_title('rxpci (MB/s)')

    axis[1, 1].plot(range(len(txpci)), rxpci, color='r', label='write')
    axis[1, 1].set_title('txpci (MB/s)')

    plt.savefig('stats.png')
    plt.show()
    
if __name__ == '__main__':
    data = extractData()
    multiPlots(data)
