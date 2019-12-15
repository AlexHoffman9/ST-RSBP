import numpy as np
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt

plt.figure()
files = ['','_mod_notuning_3','_mod_tuning_4','_mod_tuning_5']
for file in files:
    acc = []
    next = False
    with open('Result/MNIST_1kTrain'+file+'.txt','r') as f:
        for line in f:
            if '===================test Result================' in line:
                next = True
                continue

            if next:
                next = False
                acc.append(float(line[5:10]))

    acc = np.array(acc)
    plt.plot(acc[:26],'.-',linewidth=1.2)

plt.title('MNIST 1k Training')
plt.grid(True)
plt.legend(['original params','new eij','new eij + thresh','new eij + thresh + time'])
plt.savefig('acc_MNIST_1kTrain_mod_tuning_all.png')