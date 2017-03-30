# coding:utf-8

'''
xls文件导出工具

将会把指定的excel目录下的所有xls文件转成需要的文件(这里直接转成了代码文件方便使用)

对xls文件的要求:
  第一行:键名(英文)[-数据类型,-index,-none]（不填则丢弃该列，每次缩进时的第一列不可丢弃）
  第二行:格式：列名（说明使用备注方式）
  之后的行都是数据

数据类型的支持:int float string list map bool table
-k_num 表示缩进时使用索引作为key值, 默认
-k_cur 表示缩进时使用当前列作为key值
-t_none 表示缩进时不使用上一列作为表名，而是直接放在父表, 默认使用上一列作为表名
table格式应为table(table_name, real_type, cols)， real_type当前列（此列作为table的key）的类型 只能为 int or string， cols为该table包含几列,table的列不能嵌套在普通缩进的列之中
list:对应python元组[1,2,3,4]，填写时无需填写[]
map对应python元表{1:1, 2:2, 3:3},填写时无需填写{}，注意字符串需要引号
每个sheet创建一个文件，sheet命名：XXX_文件名（表名）,当sheet中含unUsed字段时将不解析该sheet
填写表格时最好不要镂空，数据镂空请填$NONE$；
可丢弃某一列（属性栏为空），可缩进，但每次缩进时的第一列不可丢弃
'''

#在这里填需要导表的文件名
FILELIST = [
'1',
'2',
]


import sys, os, traceback
import  json
import  xdrlib ,sys
import xlrd
reload(sys)
sys.setdefaultencoding('utf-8')

EXCEL_PATH = "./"
OUTPUT_PYTHON_PATH = "./data/"
OUTPUT_JS_PATH = "./"
OUTPUT_AS_PATH = "./"
OUTPUT_LUA_PATH = "./data/"

# 开关
IS_OUTPUT_PYTHON = False
IS_OUTPUT_JS = False
IS_OUTPUT_AS = False
IS_OUTPUT_LUA = True
FIRST_FULL = False

# 数据类型
T_NONE      = -1
T_INT       = 0
T_FLOAT     = 1
T_STRING    = 2
T_MAP       = 3    # 哈希表
T_BOOLEAN   = 4    # 布尔类型,
T_TABLE     = 5    # 表2,
T_LIST      = 6



def _cell_to_string(cell):
    if type(cell.value) == type(u''):    # unicode
        return str(cell.value.encode('utf-8'))
    else:
        if type(cell.value) == type(0.0):   # float转string
            if int(cell.value) == cell.value:   # 如果不带小数
                #print "[warning] float->string", cell.value, '->', int(cell.value)
                return str(int(cell.value))
        return str(cell.value)

#解析类型表头
def _get_head(cell): 
    # print 'cell value:%s' % cell.value
    type_str0 = _cell_to_string(cell).strip()
    split_str = type_str0.split('-')
    real_type , table_name, table_attr_no = T_STRING, 'NONE', 0
    attr_name = split_str[0]
    attr_type = T_STRING
    key_use_type = 2#0：k_none, 1 :k_cur 2: k_num
    bIgnoreName = False
    try:
        type_str = split_str[1]
        if type_str == 'int':
            attr_type =  T_INT
        elif type_str == 'float': 
            attr_type =  T_FLOAT
        elif type_str == 'string':
            attr_type =  T_STRING
        elif type_str == 'map': 
            attr_type =  T_MAP
        elif type_str == 'list': 
            attr_type =  T_LIST
        elif type_str == 'bool':
            attr_type = T_BOOLEAN
        elif type_str.find('table') >= 0:
            attr_type = T_TABLE
            # print 'type_str:%s' % type_str
            table_name, real_type, table_attr_no = _get_table_param(type_str)
            if table_name == '':
                table_name = '$NONE'
        else:
            attr_type = T_STRING
    except:
        attr_type = T_STRING
    if type_str0 == '' :
        attr_type = T_NONE
    if len(split_str) > 1:
        if type_str0.find('k_cur') >= 0:
            key_use_type = 1
        elif type_str0.find('k_num') >= 0:
            key_use_type = 2

        if type_str0.find('t_none') >= 0:#是否忽略table名（缩进产生的table）
            bIgnoreName = True

    # print 'attr_type:%d, attr_name:%s, real_type:%s, table_name:%s, table_attr_no:%s' % (attr_type, attr_name, real_type, table_name, table_attr_no)
    return attr_type, attr_name, key_use_type, bIgnoreName, table_name, real_type, int(table_attr_no)

#解析tale表头
def _get_table_param(type_str):
    type_str.strip()
    if type_str.find('table') >= 0:
        type_str = type_str[6:-1]
        return (type_str.split(','))

#bHead检查是否是子表的开始字段（列）,bNew是否为下一个缩进表
def _check_head(sheet, nrows, record_start_row, index_c,attr_type,head_data):
    if index_c == 0:
        return True, False
    bHead = False
    bNew = False

    bFrontEmpty = False
    bBackEmpty = False
    bFirstEmpty = False
    #找到前一个可用列
    preColumIndex = 0
    for c in xrange(index_c-1, -1, -1):
        preColumNone, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no = _get_head(sheet.row(0)[c])
        if preColumNone != T_NONE:
            preColumIndex = c
            break
    #print '++++++++++++++ preColumIndex index_c record_start_row nrows',preColumIndex, index_c, record_start_row, nrows
    (attr_type, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no)  = _get_head(sheet.col(index_c)[0])
    (pre_attr_type, pre_attr_name, pre_key_use_type, pre_bIgnoreName, pre_table_name, pre_real_type, pre_table_attr_no, pre_bHead, pre_bNew)  = head_data[preColumIndex]
    if attr_type != T_NONE:
        for r in xrange(record_start_row, record_start_row+nrows):           
            if  sheet.row(r)[0].ctype == 0 and sheet.row(r)[preColumIndex].ctype == 0 and sheet.row(r)[index_c].ctype != 0:
                bFrontEmpty = True
            if sheet.row(r)[0].ctype == 0 and sheet.row(r)[preColumIndex].ctype != 0 and sheet.row(r)[index_c].ctype == 0:
                bBackEmpty = True
                # print '++++++++++++++ bBackEmpty',r, preColumIndex,bBackEmpty
            # if  sheet.row(r)[0].ctype == 0 and sheet.row(r)[index_c].ctype != 0:
            #     bFirstEmpty = True
            #     print '++++++++++++++ bFirstEmpty',r, index_c, bFirstEmpty
        # print  index_c, bFrontEmpty , bBackEmpty , bFirstEmpty

        # preColumNone, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no = _get_head(sheet.row(0)[index_c - 1])

        # if preColumNone == T_NONE:
        #     bFrontEmpty = False
        # if afterColumNone = 
        # if bFrontEmpty or bBackEmpty and bFirstEmpty:
        #     bHead = True
        #当前列为忽略表名或非该条数据的第一行的前空后实，该列为字表开始字段
        if (pre_bNew and not pre_bIgnoreName) or bIgnoreName or bFrontEmpty:#or (bFrontEmpty )and index_c == 1
            bHead = True

        if bBackEmpty :
            bNew = True
    else:
        bHead=False
        bNew=False
    # print  '++++++++++++++++++++++++++++++++ ',index_c, pre_bNew, pre_bIgnoreName, bIgnoreName ,bFrontEmpty, bBackEmpty , bHead, bNew
    return bHead, bNew

def _get_file_name(sheet_name):
    split_str = sheet_name.split('_')
    unUsed = False
    if sheet_name.find('unUsed') >= 0:
        unUsed = True
    if len(split_str) > 1:
        return split_str[1]+"Data", unUsed
    else:
        return 'NONE', True

def _get_attr_data(cell, attr_type, attr_name, row, col):
    res = None

    try:

        # 特殊处理 字符串返回"",其他返回None
        if cell.value == "" or cell.value == "$NONE$":
            if attr_type == T_STRING:
                return ""
            else:
                return None
        # if attr_type == T_INT:
        #     res = int(cell.value)
        # elif attr_type == T_FLOAT:
        #     res = float(cell.value)
        # elif attr_type == T_STRING:
        #     res = _cell_to_string(cell)
        if attr_type == T_MAP:
            res = '{'+_cell_to_string(cell)+'}'
        elif attr_type == T_LIST:
            res = '['+_cell_to_string(cell)+']'
        elif attr_type == T_BOOLEAN:
            res = bool(cell.value)
        else:
            res = _cell_to_string(cell)
    except:
        traceback.print_stack()
        print "cell ERROR row:%d col:%d value:%s attr_type:%d attr_name:%s" % \
            (row, col, repr(cell.value), attr_type, attr_name)
    return res



def export_file(excel_file):
    insheets = []
    bk = xlrd.open_workbook(excel_file)
    count = len(bk.sheets()) #sheet数量
    for c in xrange(0, count):
        export_sheet(bk.sheets()[c])

def export_sheet(sheet):
    sh = sheet
    lines = []
    (file_name, unUsed) = _get_file_name(sh.name)
    if file_name != 'NONE':
        print ">>data: ", sh.nrows, "x", sh.ncols, sh.name, ' file_name:', file_name
    if sh.nrows <= 0 or unUsed:
        return
    data_start_row = 2
    # 读取表头
    head_data = []
    for c in xrange(0, sh.ncols):
        attr_type, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no = _get_head(sh.col(c)[0])
        #if attr_type != T_NONE:
        (bHead,bBreak) = _check_head(sh, sh.nrows- data_start_row, data_start_row,c,attr_type,head_data)
        head_data.append((attr_type, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no, bHead, bBreak))
    (excel_str, c1) =_parse_data(sheet, head_data, 0, data_start_row, 0, sh.nrows - data_start_row, len(head_data), True)
    excel_data = eval(excel_str)


    # file_name = _get_file_name(sh.name)
    # 输出python
    if IS_OUTPUT_PYTHON:
        python_path = file_name+'.py'
        output_python(head_data, excel_data, os.path.join(OUTPUT_PYTHON_PATH, python_path))

    # 输出JS
    if IS_OUTPUT_JS:
        js_path = file_name+'js'
        output_js(head_data, excel_data, os.path.join(OUTPUT_JS_PATH, js_path))

    # 输出AS
    if IS_OUTPUT_AS:
        as_path = file_name+'.as'
        output_as(head_data, excel_data, os.path.join(OUTPUT_AS_PATH, as_path))

    # output LUA
    if IS_OUTPUT_LUA:
        lua_path = file_name+'.lua'
        output_lua(head_data, excel_data, os.path.join(OUTPUT_LUA_PATH, lua_path))

PYTHON_HEAD = '# coding:utf-8\n'
def output_python(head_data, excel_data, outfile):
    #print "output_python", outfile
    f = open(outfile, 'w')
    if not f:
        print "无法创建文件:%s" % outfile
        return

    f.write(PYTHON_HEAD)
    f.write('# head: ' + ' | '.join( [d[1] for d in head_data] ) + '\n')
    f.write('data = ' + repr(excel_data) + '\n')
    f.close()


def output_js(head_data, excel_data, outfile):
    #print "output_js", outfile
    f = open(outfile, 'w')
    if not f:
        print "无法创建文件:%s" % outfile
        return

    f.write('// head: ' + ' | '.join( [d[1] for d in head_data] ) + '\n')
    file_name = os.path.split(outfile)[-1]
    var_name = file_name.split('.')[0]
    f.write(var_name + ' = ' + json.dumps(excel_data) + '\n')
    f.close()

AS_PACKAGE = 'TB'
def output_as(head_data, excel_data, outfile):
    #print "output_as", outfile
    f = open(outfile, 'w')
    if not f:
        print "无法创建文件:%s" % outfile
        return

    f.write('// head: ' + ' | '.join( [d[1] for d in head_data] ) + '\n')
    f.write('package '+ AS_PACKAGE + '{\n')
    file_name = os.path.split(outfile)[-1]
    class_name = file_name.split('.')[0]
    f.write('public class '+ class_name + '{\n')
    f.write('public static var data = ' + json.dumps(excel_data) + '\n')
    f.write('}\n')
    f.write('}\n')
    f.close()


def output_lua(head_data, excel_data, outfile):
    #print "output_lua", outfile
    f = open(outfile, 'w')
    if not f:
        print "无法创建文件:%s" % outfile
        return

    f.write('-- head: ' + ' | '.join( [d[1] for d in head_data] ) + '\n')
    file_name = os.path.split(outfile)[-1]
    var_name = file_name.split('.')[0]
    f.write(var_name + ' = {}\n')
    f.writelines(_data_tolua(var_name, excel_data))
    f.close()

    glAllMakeFile.append('require("%s")'%(var_name))

def _data_tolua(var_name, excel_data):
    lines = []
    for id, record in excel_data.iteritems():
        lines.append(_record_tolua(var_name, id, record))
    return lines

def _record_tolua(var_name, id, record):
    id_name = str(id)
    if type(id) == type(''): id_name = "'"+id+"'"   # id is string

    res = var_name + '[' + id_name + ']=' + str(_tolua(record)) + '\n'
    # print "_record_tolua var_name:%s id_name:%s res:%s" % (var_name, id_name, res)
    return res
    

def _tolua(data):
    res = ""
    if type(data) == type({}):  # dict
        res += '{'
        for k, v in data.iteritems():

            if type(k) is type(''):
                if k.isdigit():
                    res += '[\'' + str(k) + '\']=' + _tolua(v) + ','   # if k is a string of Number
                else:
                    res += str(k) + '=' + _tolua(v) + ','   # k must be string
            else:
                 res += '['+ str(k) + ']=' + _tolua(v) + ','
        res += '}'
        return res

    elif type(data) == type(()) or type(data) == type([]):   # list or tuple
        res += '{'
        i = 1
        for v in data:
            res += '['+str(i)+']='+ _tolua(v) + ','
            i += 1
        res += '}'
        return res

    elif type(data) == type(""):
        if len(data) == 0:
            return 'nil'
        else:
            return "'" + str(data) + "'"
    elif isinstance(data, bool):
        s = str(data)
        s = s.lower()
        return s
    else:
        return str(data)

#由第一列计算第一条记录包含多少行
def _count_record_row(sheet, record_start_row, parent_c, nrows):
    count = 1
    for c in xrange(record_start_row+1, record_start_row+nrows):
        if sheet.row(c)[parent_c].ctype == 0:
            count += 1
        else:
            # print '_count_record_row count1:record_start_row:%d, parent_c:%d, nrows:%d' % (record_start_row, parent_c, nrows)
            return count
    # print '_count_record_row count2:record_start_row:%d, parent_c:%d, nrows:%d' % (record_start_row, parent_c, nrows)
    return count

#思路：由第一列计算第一条记录包含多少行，之后每一列里，在该记录的行数内有多个数据，则该列为子表的开始列，且默认拿该列数据作为子表的key，仅当类型后添加-index时使用序号作key
#若类型不指定子表名，则使用前一列的值，作为table名
#bFirst是否为第一次解析表
def _parse_data(sheet, head_data, parent_c, r, c, parse_rows, parse_colums , bFirst):
    # print '_parse_data:>>>>parent_c:%d, r:%d, c:%d, parse_rows:%d, parse_colums:%d bFirst:%d' % (parent_c, r, c, parse_rows, parse_colums,bFirst)
    rowModify = 1
    record_start_row = r
    record_nrows = 0
    index_r = r
    index_c = c
    index_count = 1#索引计算
    key = None
    key_type = T_INT
    table_str = ''
    while index_r < r + parse_rows:
        # print 'parse the row :%d' % (index_r)
        index_c = c
        if index_r == record_start_row + record_nrows:#这是一条新记录
            record_nrows = _count_record_row(sheet, index_r, index_c, r + parse_rows - index_r)
            record_start_row = index_r
            row_str = None
            key = None
            if bFirst:
                first_record_nrows = record_nrows
            # print '>>>>>>>>>>>>>>新纪录：record_nrows:%d record_start_row:%d<<<<<<<<<<<<' % (record_nrows, record_start_row)
        
        while index_c < c + parse_colums:
            (attr_type, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no, bHead, bBreak)  = head_data[index_c]

            # print '\nparse the colum :%d \nattr_type :%d , attr_name :%s, key_use_type :%d , bIgnoreName :%d , table_name :%s, real_type :%s , table_attr_no :%d , bHead :%d , bBreak :%d \n' \
            #         % (index_c, attr_type, attr_name, key_use_type, bIgnoreName, table_name, real_type, table_attr_no, bHead, bBreak)
            # print index_c, c, bFirst,bHead
            if index_c != c and bHead and attr_type != T_TABLE:#若为子表的开始列,不支持缩进表嵌套
                if not bFirst and bBreak:
                    break
                str_data, index_c = _parse_data(sheet, head_data, index_r, index_r, index_c, record_nrows,c + parse_colums - index_c, False)
                row_str +=str_data
                FIRST_FULL = False
                # print '\n\nafter parse_suojin, the colums is ', index_c, row_str
            else:
                if index_c != c and not bFirst and bBreak:# 
                    # print '\n\nbreak: ', index_c, c
                    break
                cell = sheet.row(index_r)[index_c]
                attr_data = _get_attr_data(cell, attr_type, attr_name, index_r, index_c)
                # print '\n\n index_c, attr_data: ', index_c, attr_data
                if attr_type == T_NONE:#忽略该列
                    # print '\n\n 忽略该列: ', index_c, attr_data
                    index_c += 1
                    continue
                elif attr_type != T_TABLE:
                    # print '\n\n index_c, attr_data,key_use_type: ', index_c, c, attr_data,key_use_type, attr_name
                    if index_c == c: #默认拿该列数据作为表的key，仅当类型后添加-index时使用序号作key
                        if key_use_type == 1 or index_c == 0:
                            key = attr_data
                            key_type = attr_type
                        elif key_use_type == 2:
                            key = index_count
                            index_count += 1
                            key_type = T_INT
                        # print 'key_use_type : %d row:%d, colum:%d, key:%s' % (key_use_type, index_r, index_c, key)
                    index_c += 1
                    if attr_data != None:
                        if row_str == None:
                            row_str = ''
                        if attr_type == T_STRING:
                            row_str += '\'' + str(attr_name)+'\':r\''+str(attr_data)+'\','
                        else:
                            row_str += '\'' + str(attr_name)+'\':'+str(attr_data)+','
                    # print 'key_use_type : %d row:%d, colum:%d, key:%s row_str:%s' % (key_use_type, index_r, index_c, key, row_str)                    
                else:
                    if bFirst:#只有第一次解析才能解析Table
                        attr_data, rowsNO2, colNO2 = _parse_table_data(sheet, head_data, 0, table_name, real_type, table_attr_no, index_r, index_c)                
                        index_c += colNO2
                        if attr_data != None and table_name != None and table_name != '':
                            row_str += '\'' + str(table_name)+'\':'+str(attr_data)+','
                    else:
                        break

        index_r += record_nrows
        if index_r == record_start_row + record_nrows or index_r >= r + parse_rows:#下一条是新记录
            
            if key != None:
                row_str = '{' + row_str + '},'
                if key_type == T_INT:
                    table_str += str(key) + ':' + str(row_str)
                else:
                    table_str += '\'' + str(key) + '\':' + str(row_str)
            #print '===============record:%d key:%s, row_str:%s table_str:%s================' % (record_start_row ,str(key), row_str, table_str)
    #若类型不指定子表名，则使用前一列的值，作为table名
    # print 'table_str1' ,table_str
    if c != 0:#第一列仅作为key，不需添加表名
        (attr_type, attr_name, key_use_type, bIgnoreName, table_name0, real_type, table_attr_no, bHead, bBreak)  = head_data[c]
        if bIgnoreName == False:
            #使用前一列的值，作为table名 
            (attr_type1, attr_name1, key_use_type1, bIgnoreName1, table_name1, real_type1, table_attr_no1,  bHead, bBreak)  = head_data[c - 1]
            #table_name = sheet.row(r)[c-1].value 
            table_name = _cell_to_string(sheet.row(r)[c-1])
            # print 'table_name', table_name, 'attr_type1', attr_type1, 'c', c, 'r', r
            if table_name != None and table_name != '' and table_str != None and table_str != '':
                if attr_type1 == T_STRING:
                    table_str = '\'' + str(table_name)+'\':{'+str(table_str)+'},'
                elif attr_type1 == T_INT or attr_type1 == T_FLOAT:
                    table_str = str(int(table_name))+':{'+str(table_str)+'},'
                else:
                    table_str = '\'' + str(table_name)+'\':{'+str(table_str)+'},'
    else:
        table_str = '{'+str(table_str)+'}'
    
    # print 'table_str2' ,table_str
    return table_str, index_c
                    

#解析table类型
def _parse_table_data(sh, head_data, parent_c, table_name, real_type, table_attr_no, r, c):
    row = sh.row(r)
    table_str = '{'
    rowsNO = 0
    colsNO = 0
    index_r = r
    index_c = c
    # print "++++++++++++++_parse_table_data parent_c:%d table_name:%s real_type:%s table_attr_no:%d r:%d c:%d" % (parent_c, table_name, real_type, int(table_attr_no), r, c)
    bNone = False
    while index_r < sh.nrows:
        # if  sh.row(index_r)[c].value == '' or sh.row(index_r)[parent_c].value != '':
        #     print "<<<<<<<<<<table over table_str:%s" % table_str
        #     break
        row = sh.row(index_r)
        key = None
        bContinue = True
        rowsNO2 = 1
        colNO2 = 0
        index_c = c
        # print "+++++222+++_parse_table_data parent_c:%d table_name:%s real_type:%s table_attr_no:%d r:%d c:%d" % (parent_c, table_name, real_type, int(table_attr_no), r, c)
        while index_c < c+table_attr_no:
            cell = row[index_c]
            # print "+++++333+++_parse_table_data parent_c:%d table_name:%s real_type:%s table_attr_no:%d index_r:%d index_c:%d" % (parent_c, table_name, real_type, int(table_attr_no), index_r, index_c)

            if cell.value == '':
                index_c +=1
                colNO2  +=1                
                continue
            else:
                bContinue = False
            (attr_type, attr_name, key_use_type, bIgnoreName, table_name2, real_type2, table_attr_no2, bHead, bBreak)  = head_data[index_c]
            # (attr_type, attr_name, real_type2, table_name2, table_attr_no2)  = head_data[index_c]
            # (attr_type, attr_name, attr_type_value) = head_data[index_c]
            attr_data = _get_attr_data(cell, attr_type, attr_name, r, index_c)
            # print "---------------_parse_table_data index_r:%d index_c:%d attr_data:%s attr_type:%d attr_name:%s" % (index_r, index_c, attr_data, attr_type, attr_name)
            rowsNO3 = 1
            colNO3 = 1                
            if attr_type != T_TABLE:
                if attr_type != T_NONE:
                    attr_data = _get_attr_data(cell, attr_type, attr_name, r, index_c)
                    if attr_type == T_STRING:
                        if attr_data != "":
                            table_str += '\'' + str(attr_name)+'\':r\''+str(attr_data)+'\','
                    else:
                        if attr_data != None:
                            table_str += '\'' + str(attr_name)+'\':'+str(attr_data)+','
            else:
                table_str2 = ''
                if index_c == c:
                    # '\''+table_name+'\':{'
                    # print "\n\n real_type:%s\n\n" % real_type
                    # if real_type == 'string':
                    if attr_data == None or attr_data == "":
                        bNone = True
                    if real_type.find('string') >= 0:
                        # print "come in string\n"
                        table_str2 = '\''+str(attr_data)+'\':{\''+str(attr_name)+'\':r\''+str(attr_data)+'\','
                    else:
                        # print "come in else:real_type:%s\n\n" % real_type
                        table_str2 = str(attr_data)+':{\''+str(attr_name)+'\':'+str(attr_data)+','
                else:
                    # (table_name2, real_type2, table_attr_no2) = _get_table_param(attr_type_value)
                    table_str2,rowsNO3, colNO3= _parse_table_data(sh, head_data, c, table_name2, real_type2, int(table_attr_no2), index_r, index_c)
                    table_str2 = '\'' + str(table_name2) + '\':' + table_str2 + ','
                table_str += table_str2
            index_c += colNO3                
            rowsNO2 = max(rowsNO2, rowsNO3)
            colNO2  += colNO3
            # print ">>>>>>>_parse_table_data index_r:%d index_c:%d rowsNO2:%d colNO3:%d table_str:%s" % (index_r, index_c, rowsNO2, colNO3, table_str)
        table_str += '},'
        rowsNO += rowsNO2
        colsNO = max(colNO2, colsNO)
        index_r += rowsNO2
        # print "<<<<<<<<<<_parse_table_data index_r:%d parent_c:%d c:%d rowsNO:%d colsNO:%d" % (index_r, parent_c, c, rowsNO,colsNO)
        if  bContinue or index_r >= sh.nrows or sh.row(index_r)[c].value == '' or sh.row(index_r)[parent_c].value != '':
            # print "<<<<<<<<<<table over table_str:%s" % table_str
            break
    table_str += '}'
    if bNone:
        table_str ="{}"
    return table_str, rowsNO, colsNO


glAllMakeFile = []
def main():
    # 取得文件列表
    infiles = []
    TEMPLIST = []
	
    for i in FILELIST:
		TEMPLIST.append(i.encode("cp936"))
	
    for root, dirs, files in os.walk( EXCEL_PATH ):
        for fn in files:
            if fn.startswith('~'):
                continue
            if fn.endswith('.xls') or fn.endswith('.xlsx'):
				pointPos = fn.find(".")
				fileName = fn[:pointPos]
				if fileName in TEMPLIST:
					infiles.append(root+'\\'+fn)

    # 输出
    for p in infiles:
        print '\n'
        print 'parse excel:',p
        export_file(p)

    # output LUA
    if IS_OUTPUT_LUA:
        init_file = "initRequire.lua"
        f = open(os.path.join(OUTPUT_LUA_PATH, init_file), 'w')
        if not f:
            print "无法创建文件:%s" % init_file
            return
        f.write('module(..., package.seeall)\n--导入数据表\n\n')
        f.write("%s"%("\n".join(glAllMakeFile)))
        f.close()


if __name__ == "__main__":
    main()
    os.system("pause")
