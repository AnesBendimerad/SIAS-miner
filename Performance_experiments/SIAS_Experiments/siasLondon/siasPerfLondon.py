'''
Created on May 22, 2018

@author: anesbendimerad
'''
import os
def createFolder(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)
def createDesignPoint(resultFolderPath,inputFilePath):
    designPoint={}
    designPoint["maxDistance"]=3
    designPoint["binningType"]="percentileBins"
    designPoint["minVertices"]=5
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

nbVertices=[25,49,100,144,196,289,529]
nbAtt=[2,5,8,10,13,15,20,25]
minVertices=[70,50,30,20,15,10,5,3]
linkToSias="../../../Siasminer/Executable/SiasMiner.jar"
#nbVertices=[25,49,100]
#nbAtt=[2,5,8,10]
#minVertices=[70,50]

# nbVertices experiments
def variateVertices():
    folderPath="siasPerfLondon/output/varVertices"
    createFolder(folderPath)
    for nbV in nbVertices:
        curFolder=folderPath+"/V"+str(nbV)
        createFolder(curFolder)
        designPoint=createDesignPoint(curFolder,"siasPerfLondon/input/varVertices/V"+str(nbV)+"/FoursquareGraph.json")
        createParamFile(curFolder+"/parameters.txt",designPoint)
        os.system("java -Xss2000m -Xmx15g -jar "+linkToSias+" "+curFolder+"/parameters.txt")
#variateVertices()


    
def variateMinVertices():
    folderPath="siasPerfLondon/output/varMinVertices"
    createFolder(folderPath)
    for minV in minVertices:
        curFolder=folderPath+"/minV"+str(minV)
        createFolder(curFolder)
        designPoint=createDesignPoint(curFolder,"siasPerfLondon/input/default/289v10att/FoursquareGraph.json")
        designPoint["minVertices"]=minV
        createParamFile(curFolder+"/parameters.txt",designPoint)        
        os.system("java -Xss2000m -Xmx15g -jar "+linkToSias+" "+curFolder+"/parameters.txt")
    
variateMinVertices()

def variateAttributes():
    folderPath="siasPerfLondon/output/varAttributes"
    createFolder(folderPath)
    for nbA in nbAtt:
        curFolder=folderPath+"/"+str(nbA)+"att"
        createFolder(curFolder)
        designPoint=createDesignPoint(curFolder,"siasPerfLondon/input/varAttributes/"+str(nbA)+"att/FoursquareGraph.json")
        createParamFile(curFolder+"/parameters.txt",designPoint)
        os.system("java -Xss2000m -Xmx15g -jar "+linkToSias+" "+curFolder+"/parameters.txt")
#variateAttributes()
    
    
    
    
    
    

