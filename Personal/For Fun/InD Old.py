import random
list = []
# put for coop or straem options
for  i in range (0,10):
    x = random.randint(1,4)
    if x == 1:
        list.append(x)
    elif x == 2:
        list.append(x)
    elif x == 3:
        list.append(x)
    elif x == 4:
        list.append(x)

    if list.count(1) == 3: 
        print("MC")
        flag = False
    if list.count(2) == 3:
        print("DL")
        flag = False
    if list.count(3) == 3:
        print("KS")
        flag = False
    if list.count(4) == 3:
        print("stream")
        flag = False

el1 = list.count(1)
el2 = list.count(2)
el3 = list.count(3)
el4 = list.count(4)



if el1 > el2 and el1 > el3 and el1 > el4:
    print("MC")
if el2 > el1 and el2 > el3 and el2 > el4:
    print("DL")
if el3 > el1 and el3 > el2 and el3 > el4:
    print("KS")
if el4 > el1 and el4 > el2 and el4 > el3:  
    print("Minecraft")
print(list)
