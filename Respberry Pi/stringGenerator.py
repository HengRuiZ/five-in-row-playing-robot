def graspString():
    return '#@000000000000+000+000$'

def getString():
    return '#P000000000000+000+000$'

def putDownString():
    return '#a000000000000+000+000$'

def resetString():
	return '#p000000000000+000-000$'

def zChangeString(a): #Problem: the specific location in String seems wrong!
	tmp = '%03d' % a
	return '#4000000' + tmp + '000+000-000$'

def 

def turnToString(a,b): #Problem: a and b means the coordinate in board, starting from 0?
    GRIDLENGTH = 30
    tmp0 = '%03d' % a
    tmp1 = '%03d' % b
    return '#8' + tmp0 + tmp1 + '000000+000-000$'