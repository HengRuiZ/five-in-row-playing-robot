def readFile():
    f = open('./Chess/next.txt', 'r')
    content = f.readlines()
    f.close()
    if content[0][0] == 'c':
        x = int(content[1])
        y = int(content[2])
        return [False, x, y]
    else:
        return [True, 0, 0]