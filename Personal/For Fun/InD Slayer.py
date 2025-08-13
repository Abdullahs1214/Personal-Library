def InD_Slayer():
    import random
    import InDSlayerNames
    listcoop = []
    liststream = []
    c = 0
    s = 0
    flag1 = True
    flag2 = True
    snames = InDSlayerNames.stream
    cnames = InDSlayerNames.coop

    while flag1 == True:
        a = random.randint(0,1)
        if a == 0:
            print("Stream")
            s = 1
            flag1 = False
        elif a == 1:
            print("Coop")
            c = 1
            flag1 = False
            
    if s == 1:
        while flag2 == True:
            x = random.randint(0,3)
            liststream.append(x)
            for i in range(4):
                if liststream.count(i) == 3:
                    flag2 = False
                    print(snames[i])
        
    if c == 1:
        while flag2 == True:
            x = random.randint(0,3)
            listcoop.append(x)
            for i in range(3):
                if listcoop.count(i) == 3:
                    flag2 = False
                    print(cnames[i])
InD_Slayer()
               


