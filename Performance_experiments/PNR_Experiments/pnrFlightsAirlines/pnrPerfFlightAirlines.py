'''
Created on May 24, 2018

@author: anesbendimerad
'''


import os
import subprocess

def execCmdWithTimeout(cmdTxt,timeout):
    try:
        subprocess.check_call(args= cmdTxt , shell=True, timeout=timeout)
        return True
    except subprocess.TimeoutExpired:
        print('time out')
        return False
    except subprocess.CalledProcessError as e:
        print ('CalledProcessError')
        return False
    except Exception as e:
        print ('Exception')
        return False

def createFolder(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

nbVertices=[40,60,100,150,200,322]
nbAtt=[2,3,5,10,14]
minVertices=[100,80,60,40,30,20,10]
maxD=1
#nbVertices=[40]
#nbAtt=[2]
#minVertices=[100]
root_path = os.getcwd() + '/'
timeout=10000 # seconds
# add the rest of cases, add timeout (10000 s), add >> log file
mainFolder="pnrPerfFlights"

linkToPRNMiner="../../PNRMiner_Code"

def variateNbVertices():
    folderPath=mainFolder+"/varVertices"
    print("Different number of Vertices", flush=True)
    for nbV in nbVertices:
        print(str(nbV),flush=True)
        curFolder=root_path + folderPath+"/V"+str(nbV)+"/"+str(maxD)+"hop/"
        os.chdir(curFolder)
        ret=execCmdWithTimeout("\""+root_path+linkToPRNMiner+"/./P-N-RMiner\" config.txt > log.txt",timeout)
        if not ret:
            break

variateNbVertices()



def variateMinVertices():
    folderPath=mainFolder+"/default/322V14att/"+str(maxD)+"hop"
    os.chdir(root_path + folderPath)
    print("Different minimum of Vertices", flush=True)
    #createFolder(folderPath)
    for minV in minVertices:
        print(str(minV),flush=True)
        configFile="config_r"+str(minV)+".txt"
        ret=execCmdWithTimeout("\""+root_path+linkToPRNMiner+"/./P-N-RMiner\" "+configFile+"> log"+configFile,timeout)
        if not ret:
            break

variateMinVertices()



def variateNbAttributes():
    folderPath=mainFolder+"/varAttributes"
    print("Different number of Attributes", flush=True)
    for nbA in nbAtt:
        print(str(nbA),flush=True)
        curFolder=root_path + folderPath+"/"+str(nbA)+"att/"+str(maxD)+"hop/"
        os.chdir(curFolder)
        ret=execCmdWithTimeout("\""+root_path+linkToPRNMiner+"/./P-N-RMiner\" config.txt > log.txt",timeout)
        if not ret:
            break

variateNbAttributes()






#variateMinVertices()


