'''
Created on May 23, 2018

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
        
nbVertices=[25,49,100,144,196,289,529]
nbAtt=[2,5,8,10,13,15,20,25]
minVertices=[70,50,30,20,15,10,5,3]
#nbVertices=[25,49]
#nbAtt=[2,5]
#minVertices=[70,50,30]
root_path = os.getcwd() + '/'
timeout=10000 # seconds
# add the rest of cases, add timeout (10000 s), add >> log file 
linkToPRNMiner="../../PNRMiner_Code"


def variateNbVertices():
    folderPath="pnrPerfLondon/varVertices"
    for nbV in nbVertices:
        curFolder=root_path + folderPath+"/V"+str(nbV)+"/3hop/"
        os.chdir(curFolder)
        ret=execCmdWithTimeout("\""+root_path+linkToPRNMiner+"/./P-N-RMiner\" config.txt > log.txt",timeout)
        if not ret:
            break

variateNbVertices()



def variateMinVertices():
    folderPath="pnrPerfLondon/default/289v10att/3hop"
    os.chdir(root_path + folderPath)
    #createFolder(folderPath)
    for minV in minVertices:
        configFile="config_r"+str(minV)+".txt"
        ret=execCmdWithTimeout("\""+root_path+linkToPRNMiner+"/./P-N-RMiner\" "+configFile+"> log"+configFile,timeout)
        if not ret:
            break
        
variateMinVertices()        

   
   
def variateNbAttributes():
    folderPath="pnrPerfLondon/varAttributes"
    for nbA in nbAtt:
        curFolder=root_path + folderPath+"/"+str(nbA)+"att/3hop/"
        os.chdir(curFolder)
        ret=execCmdWithTimeout("\""+root_path+linkToPRNMiner+"/./P-N-RMiner\" config.txt > log.txt",timeout)
        if not ret:
            break     
        
#variateNbAttributes()       
        
        
        
        
        
        
#variateMinVertices()        
        
        