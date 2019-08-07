
# coding: utf-8

# In[13]:


import subprocess
import pandas
import matplotlib.pyplot as plt
get_ipython().run_line_magic('matplotlib', 'inline')


# In[14]:


# Calling the process with different number of process
# each for 10 times and storing the values in result.csv
subprocess.call(["gcc", "ass3_52.c", "-lm"])
for i in range(10):
    for j in [10, 50, 100]:
        # print ("hello i = " + str(i) + " j = "+ str(j);
        subprocess.call(["./a.out", str(j), str(i+j)]);


# In[62]:


fd = pandas.read_csv('result.csv');
print(fd);
result = pandas.DataFrame({'number':[10,50,100],
'FCFS':[0.0, 0.0,0.0],
'NP_SJF' : [0.0, 0.0,0.0],
'P_SJB' : [0.0, 0.0,0.0],
'RR' : [0.0, 0.0,0.0],
'HRRF' : [0.0, 0.0,0.0]});

# print(result);
# average_FCFS = [0.0, 0.0,0.0];
# average_NP_SJF = [0.0, 0.0,0.0];
# average_P_SJB = [0.0, 0.0,0.0];
# average_RR = [0.0, 0.0,0.0];
# average_HRRF = [0.0, 0.0,0.0];


# In[63]:


ch = 0;

for i in range(30):
    if fd['n'][i] == 10:
        ch = 0;
    elif fd['n'][i] == 50:
        ch = 1;
    elif fd['n'][i] == 100:
        ch = 2;
    result['FCFS'][ch] += fd['npFCFS_r'][i];
    result['NP_SJF'][ch] += fd['npSJF_r'][i];
    result['P_SJB'][ch] += fd['pSJF_r'][i];
    result['RR'][ch] += fd['roundRobin_r'][i];
    result['HRRF'][ch] += fd['hRRF_r'][i];

for i in range(3):
    result['FCFS'][i] /= 10;
    result['NP_SJF'][i] /= 10;
    result['P_SJB'][i] /= 10;
    result['RR'][i] /= 10;
    result['HRRF'][i] /= 10;


# In[64]:


print(result);


# In[65]:


result = result.append({'number':0, 'FCFS':0, 'NP_SJF':0, 'P_SJB':0, 'RR':0, 'HRRF':0}, ignore_index=True);


# In[66]:


result.set_index('number', inplace=True);
print(result);


# In[67]:


result = result.sort_index();
print(result);


# In[68]:


result_plot = result.plot(figsize=[10,10]);
fig = result_plot.get_figure();
fig.savefig("result_plot.pdf", bbox_inches='tight')

