# -*- coding: utf-8 -*-
import re
import sys
import os

EXT_SRC = ".proto" # 原文件扩展名
EXT_DEST = ".lua" # 目标文件扩展名

def trans(src, dest):
	f = open(src, "r")
	txt = f.read()
	f.close()
	
	serviceList = []
	flagList = []
	for flag,lines in re.findall("\nservice\s(\w+)\s?\{([\s\S]+?)\}", txt):
		lst = []
		for line in lines.split("\n"):
			s = split(line)
			if s:
				lst.append(s)
		service = """%s = {
	%s
}""" % (flag, ",\n\t".join(lst))
		serviceList.append(service)
		flagList.append(flag)
		
	if not serviceList:
		return []

	code = """module(..., package.seeall)
	
%s
""" % "\n\n".join(serviceList)
	f = open(dest, "w")
	f.write(code)
	f.close()
	return flagList

def split(line):
	line = line.strip()
	m = re.match("rpc (\S+)\((\S+)\)\s?returns\((\S+)\);", line)
	if m:
		name = m.group(1)
		sendFunc = m.group(2).replace(".", "_pb.")
		revFunc = m.group(3).replace(".", "_pb.")
		return "%s = {%s, %s}" % (name, sendFunc, revFunc)
	return ""

def indent(txt, n):
	'''缩进
	'''
	lines = []
	for line in txt.split("\n"):
		if line.strip():
			line = "\t"*n + line
		lines.append(line)
	return "\n".join(lines)

def unpackArgs(args):
	data = {}
	for arg in args:
		k,v = arg.split("=")
		data[k] = v
	return data


def makeRpc(fileName, nameList):
	upList = []
	downList = []
	for name, flagList in nameList:
		upList.append("%s_rpc.%s" % (name, flagList[0]))
		downList.append("%s_rpc.%s" % (name, flagList[1]))
		
	code = """module(..., package.seeall)

--上行的
tUp={
%s
}


--下行的
tDown={
%s
}""" % (indent(",\n".join(upList), 1), indent(",\n".join(downList), 1))
		
	f = open(fileName, "w")
	f.write(code)
	f.close()


def makeRequire(fileName, nameList):
	txtList = []
	for name in nameList:
		txtList.append('require("%s")' % name)

	code = """module(..., package.seeall)

%s
""" % "\n".join(txtList)
		
	f = open(fileName, "w")
	f.write(code)
	f.close()


if __name__ == "__main__":
	args = unpackArgs(sys.argv[1:])
	srcPath = args.get("src", "../") # 源目录
	destPath = args.get("dest", "lua/") # 目标目录
	rpcFile = args.get("rpc", "_rpc.lua")
	requireFile = args.get("require", "_init.lua")
		
	rpcList = []
	requireList = []
	for path, dirs, files in os.walk(srcPath):
		#if path != srcPath: # 只取根目录下的protobuff文件
			#continue
		for srcName in files:
			if not srcName.endswith(EXT_SRC):
				continue
			name, _ = srcName.split(".")
			srcFile = os.path.join(path, srcName)
			destFile = destPath + name + "_rpc" + EXT_DEST
			name_pb = name + "_pb"
			name_rpc = name + "_rpc"
			if name_pb not in requireList:
				requireList.append(name_pb)
			flagList = trans(srcFile, destFile)
			if flagList:
				rpcList.append((name, flagList))
				requireList.append(name_rpc)

	makeRpc(destPath + rpcFile, rpcList)
	requireList.append(rpcFile.split(".")[0])
	makeRequire(destPath + requireFile, requireList)
