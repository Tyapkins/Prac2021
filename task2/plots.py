#import pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd
import os
import matplotlib.pyplot as plt


coherent_pred = "coherent_res/"
parallel_pred = "parallel_res/"
coherent_names = os.listdir(coherent_pred)
parallel_names = os.listdir(parallel_pred)

coherent_time = pd.DataFrame()
parallel_time = pd.DataFrame()

i = 0
for file_name in coherent_names:
    new_file = open(coherent_pred+file_name, 'r')
    lines = new_file.readlines()
    nums = np.array(lines[0].replace('\n', '').split(';')).astype(np.int)
    if (len(lines) > 1):
        times = np.array(lines[1][:-2].replace('\n', '').split(' ')).astype(np.float)
        for time in times:
            coherent_time.loc[i, "jobs"] = int(nums[0])
            coherent_time.loc[i, "processors"] = int(nums[1])
            coherent_time.loc[i, "time"] = time
            i += 1
    #i += 1

coh_proc = sns.lineplot(data=coherent_time, x="processors", y="time", hue="jobs")
plt.savefig('plots/time_procs.png', bbox_inches='tight')
plt.show()

coh_jobs = sns.lineplot(data=coherent_time, x="jobs", y="time", hue="processors")
plt.savefig('plots/coh_time_jobs.png', bbox_inches='tight')
plt.show()

i = 0
for file_name in parallel_names:
    new_file = open(parallel_pred+file_name, 'r')
    lines = new_file.readlines()
    nums = np.array(lines[0].replace('\n', '').split(';')).astype(np.int)
    if (len(lines) > 1):
        times = np.array(lines[1][:-2].replace('\n', '').split(' ')).astype(np.float)
        for time in times:
            parallel_time.loc[i, "jobs"] = int(nums[0])
            parallel_time.loc[i, "threads"] = int(nums[1])
            parallel_time.loc[i, "time"] = time
            i += 1
    #i += 1
par_proc = sns.lineplot(data=parallel_time, x="threads", y="time", hue="jobs")
plt.savefig('plots/time_threads.png', bbox_inches='tight')
plt.show()

par_jobs = sns.lineplot(data=parallel_time, x="jobs", y="time", hue="threads")
plt.savefig('plots/par_time_jobs.png', bbox_inches='tight')
plt.show()