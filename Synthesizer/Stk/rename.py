import os,sys

d = os.listdir('.')
for f in d:
    if(len(f.split('.')) == 1): continue
    if(f.split('.')[1] == 'hpp' and f[0]=='T'):
        print(f)
        os.system('cp ' + f + ' ' + 'Stk' + f[1:])
