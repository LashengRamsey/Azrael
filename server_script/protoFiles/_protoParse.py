#-*-coding:utf-8-*-
#作者:叶伟龙@龙川县赤光镇
import re
import codecs
import copy
import os.path
import platform

SS_PROTO=1 #上行
CS_PRPTO=2 #下行
LINE_SEP="\x0d\x0a"
NEW_LINE_CHAR="\x0d"

class cTxtParser(object):
	def __init__(self):
		self.lServerServerNo=[]		#上行协议编号
		self.lServerServerFunc=[]	#上行协议函数名
		self.lClientSrverNo=[]		#下行协议编号
		self.lClientSrverFunc=[]	#下行协议函数名

	#sFileName文件名字
	#sSSProto:sFileName文件中的上行协议
	#sCSProto:sFileName文件中的下行协议
	def setParesProtoFileAttr(self,sFileName, sSSProto, sCSProto,sSNoMapFunc='gdSSNoMapFunc', sSFuncMapNo='gdSSFuncMapNo', sCNoMapFunc='gdCSNoMapFunc', sCFuncMapNo='gdCSFuncMapNo'):
		if not sFileName or not sSSProto or not sCSProto:
			raise Exception, trans('请传入文件名,文件的上行协议下行协议')
		self.sSrcPath=sFileName
		self.sSSProto=sSSProto
		self.sCSProto=sCSProto

		self.sSNoMapFunc=sSNoMapFunc
		self.sSFuncMapNo=sSFuncMapNo

		self.sCNoMapFunc=sCNoMapFunc
		self.sCFuncMapNo=sCFuncMapNo

		self.parseTxtTo2dGroup()

	#返回一个从txt生成的2维list
	def parseTxtTo2dGroup(self):
		if not self.sSrcPath or not self.sSSProto or not self.sCSProto:
			raise Exception, trans('请先调用self.setParesProtoFileAttr函数')
		try:
			fSrc=open(self.sSrcPath,'r')#codecs.open(self.sSrcPath,'r','utf-8')
		except Exception:
			raise Exception,trans('找不到 {}').format(self.sSrcPath)
		try:
			iProtoType=-1
			for iRow,sLineTxt in enumerate(fSrc):
				if iRow==0 and sLineTxt[:3]==codecs.BOM_UTF8:#去掉utf8的文件头
					sLineTxt=sLineTxt[3:]			
				sLineTxt=sLineTxt.strip('\n')
				sLineTxt=sLineTxt.strip('\r')
				sLineTxt=sLineTxt.strip()
				if sLineTxt.count('\t')==len(sLineTxt):#一行全是\t
					continue
				if not sLineTxt:
					continue
				if sLineTxt.startswith('//'):#该行是注释	不考虑/**/块注释的情况
					continue
				if sLineTxt.startswith(self.sSSProto):	#接下来的都是上行协议
					iProtoType=SS_PROTO
				if sLineTxt.startswith(self.sCSProto):	#下行协议
					iProtoType=CS_PRPTO
				if iProtoType != SS_PROTO and iProtoType != CS_PRPTO:
					continue
				if not re.match('^/\*\d+\*/rpc rpc.*(.*\..*)returns(.*\..*)', sLineTxt):
					continue
				lNo = re.findall(r'\d+', sLineTxt)	#提取数字
				if not lNo:		#没提取出数字
					continue
				iNo = int(lNo[0])	#/*100231*/rpc rpc****(123)   iNo=100231,第一个出现的数字
				lRpcFuncName=re.findall(r'rpc(.+?)\(', sLineTxt)	#提取函数名
				if not lRpcFuncName:	#没找到函数名
					continue
				sFuncName='\"%s\"'%(lRpcFuncName[0].strip())
				lProtoNo, lProtoFunc=[], []
				if iProtoType==SS_PROTO:
					lProtoNo, lProtoFunc = self.lServerServerNo, self.lServerServerFunc
				elif iProtoType==CS_PRPTO:
					lProtoNo, lProtoFunc = self.lClientSrverNo, self.lClientSrverFunc
				if iNo in lProtoNo:
					raise Exception, trans('{}在{}行发生错误:协议编号 {}也被使用,协议名{}'.format(self.sSrcPath, iRow, iNo, sFuncName))
				if sFuncName in lProtoFunc:
					raise Exception, trans('{}在{}行发生错误:协议编号{},协议名 {}也被使用'.format(self.sSrcPath, iRow, iNo, sFuncName))
				lProtoNo.append(iNo)
				lProtoFunc.append(sFuncName)	
		finally:
			fSrc.close()

	#将生成的数据拷贝到py文件
	def makeToPyFile(self, sDstPath):
		list=[]
		list.append('#上行协议数据表{编号:协议名}'+NEW_LINE_CHAR+self.sSNoMapFunc+'='+self.makeDict(self.lServerServerNo, self.lServerServerFunc, True))
		list.append('#上行协议数据表{协议名:编号}'+NEW_LINE_CHAR+self.sSFuncMapNo+'='+self.makeDict(self.lServerServerFunc, self.lServerServerNo, True))
		
		list.append('#下行协议数据表{编号:协议名}'+NEW_LINE_CHAR+self.sCNoMapFunc+'='+self.makeDict(self.lClientSrverNo, self.lClientSrverFunc, True))
		list.append('#下行协议数据表{协议名:编号}'+NEW_LINE_CHAR+self.sCFuncMapNo+'='+self.makeDict(self.lClientSrverFunc, self.lClientSrverNo, True))
		sTemp=LINE_SEP.join(list)

		sFlag1,sFlag2='#导表开始','#导表结束'
		if not os.path.exists(sDstPath):
			sTemp='''#-*-coding:utf-8-*-
#作者:叶伟龙@龙川县赤光镇
'''+sFlag1+LINE_SEP+sTemp+LINE_SEP+sFlag2
		else:
			fDst=open(sDstPath,'r')#读
			sOri=fDst.read()
			iBegin=sOri.find(sFlag1)
			iEnd=sOri.find(sFlag2)
			if iBegin==-1:
				fDst.close()
				raise Exception,trans('错误,{}没有导表开始标志 {}').format(sPath,sFlag1)
			if iEnd==-1:
				fDst.close()
				raise Exception,trans('错误,{}没有导表结束标志 {}').format(sPath,sFlag2)
			if iBegin>iEnd:
				fDst.close()
				raise Exception,trans('错误,导表开始,结束标志位置反了 {}').format(sPath)

			iBegin+=len(sFlag1)
			sTemp=sOri[:iBegin]+LINE_SEP+sTemp+LINE_SEP+sOri[iEnd:]
			fDst.close()
		fDst=open(sDstPath,'w')#写
		fDst.seek(0,0)
		fDst.write(sTemp)
		fDst.close()

	#生成一个字典
	def makeDict(self,lKeys,lValues,bIsNewLine=False,iElementIndent=1):#iElementIndent元素缩进tab个数
		if len(lKeys)==0 and len(lValues)==0:
			return '{}'

		if len(lKeys)!=len(lValues):
			raise Exception, trans('解析文件程序错误:编号个数和函数个数不相等')
		lTemp=[]
		for i,sKey in enumerate(lKeys):
			val=lValues[i]
			lTemp.append('%s:%s'%(sKey,val))
		if bIsNewLine:
			sEndTab='\t'*(iElementIndent-1) #字典结束符}缩进的tab串
			sElmTab=sEndTab+'\t' #元素缩进的tab串
			return '{\n%s'%sElmTab+(',\n%s'%sElmTab).join(lTemp)+',\n%s}'%sEndTab
		else:
			return '{'+','.join(lTemp)+'}'

	def makeTuple(self,lItems,bIsNewLine=False,iElementIndent=1):#iElementIndent元素缩进tab个数
		if bIsNewLine:
			sEndTab='\t'*(iElementIndent-1)	 #list结束符]缩进的tab串
			sElmTab=sEndTab+'\t' #元素缩进的tab串
			return '(\n%s'%sElmTab+(',\n%s'%sElmTab).join(lItems)+',\n%s)'%sEndTab
		else:
			#只有一个元素时,tuple的括号会被误解析为是运算符,所以要元素末尾要加逗号
			if len(lItems)==1:
				return '('+lItems[0]+',)'
			else:
				return '('+','.join(lItems)+')'

	def makeList(self,lItems,bIsNewLine=False,iElementIndent=1):#iElementIndent元素缩进tab个数
		if bIsNewLine:
			sEndTab='\t'*(iElementIndent-1) #元组结束符)缩进的tab串
			sElmTab=sEndTab+'\t' #元素缩进的tab串
			return '[\n%s'%sElmTab+(',\n%s'%sElmTab).join(lItems)+',\n%s]'%sEndTab
		else:
			return '['+','.join(lItems)+']'

	def isLastInGroup(self,lLines,iCurRow,iCurCol):#在当前组中,是否是最后一个key
		if iCurRow+1>=len(lLines):#已经是整个表最最后一行了,当然是最后一条key了
			return True

		if iCurCol>0 and self.isLastInGroup(lLines,iCurRow,iCurCol-1):#如果父组都结束了,就不需要当前key与下一个key比较了
			return True

		sCur=lLines[iCurRow][iCurCol]#当前行的某列key
		sNext=lLines[iCurRow+1][iCurCol]#下一行的某列key
		if sCur and not sNext:#上一行填了key,下一行没有填key,则说明当前行不是本组中最后一个key
			return False
		elif not sCur and sNext:#当前行策划没有填,但是下一行策划有填,说明当前行是本组中最后一个key了
			return True
		elif not sCur and not sNext:#当前行策划没有填,下一行策划也没有填,说明是当前行不是本组中最后一条key.
			return False
		else:# sCur and sNext:#当前行与下一行策划都填了,就看填的内容是不是相同的来决定了
			return sCur!=sNext

	def getVarName(self):#数据结构的变量名
		return self.sVarName

	def checkFormat(self,iRow,iCol,sText,iFormat):
		t= ('\"','\'')
		if sText and (sText[0] in t or sText[-1] in t):
			raise Exception,trans('{}行,{}列,数据不能单引号或双引号开头').format(iRow+1,iCol+1)
		elif sText.count('\"')%2 or sText.count('\'')%2:
			raise Exception,trans('{}行,{}列,数据单引号或双引号不成对').format(iRow+1,iCol+1)

		l=[]
		if iFormat&makeData.I:
			if isInt(iRow,iCol,sText):
				return makeData.I
			l.append('整形')
		if iFormat&makeData.D:
			if isDict(iRow,iCol,sText):
				return makeData.D
			l.append('用{}括起来的字典')
		if iFormat&makeData.L:
			if isList(iRow,iCol,sText):
				return makeData.L
			l.append('用[]括起来的列表')
		if iFormat&makeData.T:
			if isTuple(iRow,iCol,sText):
				return makeData.T
			l.append('用()括起来的元组')
		if iFormat&makeData.F:
			if isFloat(iRow,iCol,sText):
				return makeData.F
			l.append('用带小数点的浮点形')
		if iFormat&makeData.E:
			if isLambda(iRow,iCol,sText):
				return makeData.E
			l.append('以lambda开头的公式')
			
		if iFormat&makeData.S:
			return makeData.S

		raise Exception,trans('{}行{}列只能是{}').format(iRow+1,iCol+1,'or'.join(l))

def isInt(iRow,iCol,sText):
	try:
		int(sText)
	except Exception:
		return False
	return True

def isFloat(iRow,iCol,sText):
	try:
		float(sText)
	except Exception:
		return False
	return True
	
def isLambda(iRow,iCol,sText):
	if not sText.startswith('lambda'):
		return False
	if sText.find(':')==-1:
		raise Exception,trans('lambda公式缺少一个冒号.')
	if sText.count('(')!=sText.count(')'):
		raise Exception,trans('lambda公式括弧不匹配.')
	try:
		eval(sText)
	except Exception:
		return False
	return True	
	

def isDict(iRow,iCol,sText):
	if sText[0]!='{' and sText[-1]!='}':
		return False
	checkTupleListDict(iRow,iCol,sText)
	return True

def isList(iRow,iCol,sText):
	if sText[0]!='[' and sText[-1]!=']':
		return False
	checkTupleListDict(iRow,iCol,sText)
	return True

def isTuple(iRow,iCol,sText):
	if sText[0]!='(' and sText[-1]!=')':
		return False
	checkTupleListDict(iRow,iCol,sText)
	return True

def checkTupleListDict(iRow,iCol,sText):#非法则返回错误原因
	if sText.count('{')!=sText.count('}'):
		raise Exception,trans('{}行{}列,{}花括号不匹配').format(iRow+1,iCol+1)
	elif sText.count('[')!=sText.count(']'):
		raise Exception,trans('{}行{}列,[]方括号不匹配').format(iRow+1,iCol+1)
	elif sText.count('(')!=sText.count(')'):
		raise Exception,trans('{}行{}列,()括弧不匹配').format(iRow+1,iCol+1)
	try:
		eval(sText)
	except Exception:
		raise Exception,trans('{}行{}列,数据错误').format(iRow+1,iCol+1)

def trans(sUTF8Text):
	if platform.system().upper()=='WINDOWS':
		try:
			return sUTF8Text.decode('utf-8').encode('gbk')
		except Exception:
			#logException()
			raise
	return sUTF8Text


if __name__ == "__main__":
	ps=cTxtParser()
	ps.setParesProtoFileAttr('gameService.proto', 'service gameServerService', 'service gameClientService')
	ps.makeToPyFile('../data/py/gameService.py')

	print trans('生成{}文件成功'.format('gameService.py'))

	ps=cTxtParser()
	ps.setParesProtoFileAttr('serviceMisc.proto', 'service gameServerMiscService', 'service gameClientMiscService')
	ps.makeToPyFile('../data/py/serviceMisc.py')
	print trans('生成{}文件成功'.format('serviceMisc.py'))












