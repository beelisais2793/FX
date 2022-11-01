import numpy as np
import pandas as pd
import os
import time
#from pythonosc import udp_client
#from pythonosc.udp_client import SimpleUDPClient

eeg_data = pd.read_excel('dataset.xlsx', delimiter=";", decimal=",")
eeg_data = eeg_data.astype(float)

#ip = "127.0.0.1"
#port = 5510 #'faust default'
#client = SimpleUDPClient(ip, port)  # Create client

for i in range(eeg_data.shape[0]):
    col1 = eeg_data.iloc[:,0].values # prints column as array
    col2 = eeg_data.iloc[:,1].values
    col3 = eeg_data.iloc[:,2].values
    col4 = eeg_data.iloc[:,3].values
    time.sleep(1) # time delay  between itterations
    eeg_value1 = col1[i]* 0.25 + 0.75 # get value of column & normalize
# data in excel sheet is normalized between (-1,1) so I scaled too (0.5 - 1) for Faust
    eeg_value2 = col2[i]* 0.25 + 0.75
    eeg_value3 = col3[i]* 0.25 + 0.75
    eeg_value4 = col4[i]* 0.25 + 0.75
    osc_target1 = "/BrainwaveVirtualInstrument/Alpha/GainL" # osc target too UI element
    osc_target2 = "/BrainwaveVirtualInstrument/Alpha/GainR"
    osc_target3 = "/BrainwaveVirtualInstrument/Beta/GainL"
    osc_target4 = "/BrainwaveVirtualInstrument/Beta/GainR"
    #client.send_message(osc_target1, eeg_value1)
    #client.send_message(osc_target2, eeg_value2)
    #client.send_message(osc_target3, eeg_value3)
    #client.send_message(osc_target4, eeg_value4)
    print (osc_target1, eeg_value1)
    print (osc_target2, eeg_value2)
    print (osc_target3, eeg_value3)
    print (osc_target4, eeg_value4)


