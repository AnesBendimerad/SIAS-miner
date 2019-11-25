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
def createDesignPoint(resultFolderPath,inputFilePath):
    designPoint={}
    designPoint["maxDistance"]=1
    designPoint["binningType"]="percentileBins"
    designPoint["minVertices"]=20
    designPoint["resultFolderPath"]=resultFolderPath
    designPoint["inputFilePath"]=inputFilePath
    designPoint["topK"]=100000
    return designPoint
def createParamFile(filePath,designPoint):
    with open(filePath,"w") as f:
        f.write("maxDistance="+str(designPoint["maxDistance"])+"\n")
        f.write("resultFolderPath="+designPoint["resultFolderPath"]+"\n")
        f.write("inputFilePath="+designPoint["inputFilePath"]+"\n")
        f.write("binningType="+designPoint["binningType"]+"\n")
        f.write("minVertices="+str(designPoint["minVertices"])+"\n")
        f.write("topK="+str(designPoint["topK"])+"\n")
        f.write("applyUpdating=false\n")

nbVertices=[50,100,200,500,1000,1500,2400]
nbAtt=[2,3,5,10,15,20]
minVertices=[80,60,40,20,10,5]
timeout=10000

#nbVertices=[50,100,200]
#nbAtt=[2,3,5,10]
#minVertices=[80,60,40]

mainFolder="siasPerfIngredients"
linkToSias="../../../Siasminer/Executable/SiasMiner.jar"

# nbVertices experiments
def variateVertices():
    
    folderPath=mainFolder+"/output/varVertices"
    createFolder(folderPath)
    for nbV in nbVertices:
        curFolder=folderPath+"/V"+str(nbV)
        createFolder(curFolder)
        designPoint=createDesignPoint(curFolder,mainFolder+"/input/varVertices/V"+str(nbV)+"/graph.json")
        createParamFile(curFolder+"/parameters.txt",designPoint)
        if (not execCmdWithTimeout("java -Xss2000m -Xmx15g -jar "+linkToSias+" "+curFolder+"/parameters.txt",timeout)):
            break
variateVertices()


    
def variateMinVertices():
    folderPath=mainFolder+"/output/varMinVertices"
    createFolder(folderPath)
    for minV in minVertices:
        curFolder=folderPath+"/minV"+str(minV)
        createFolder(curFolder)
        designPoint=createDesignPoint(curFolder,mainFolder+"/input/default/2400V20att/graph.json")
        designPoint["minVertices"]=minV
        createParamFile(curFolder+"/parameters.txt",designPoint)
        if (not execCmdWithTimeout("java -Xss2000m -Xmx15g -jar "+linkToSias+" "+curFolder+"/parameters.txt",timeout)):
            break
    
variateMinVertices()

def variateAttributes():
    folderPath=mainFolder+"/output/varAttributes"
    createFolder(folderPath)
    for nbA in nbAtt:
        curFolder=folderPath+"/"+str(nbA)+"att"
        createFolder(curFolder)
        designPoint=createDesignPoint(curFolder,mainFolder+"/input/varAttributes/"+str(nbA)+"att/graph.json")
        createParamFile(curFolder+"/parameters.txt",designPoint)
        if (not execCmdWithTimeout("java -Xss2000m -Xmx15g -jar "+linkToSias+" "+curFolder+"/parameters.txt",timeout)):
            break
variateAttributes()
    
    
    
    
    
    