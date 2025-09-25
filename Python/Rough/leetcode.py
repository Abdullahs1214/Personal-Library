
list1 = []
for x in range(len(nums)):
    for y in range(x+1,len(nums)):
        #print(index1 + index2)
            if (x + y) == target:
                print(x)
                print(y)
                list1.append(x)
                list1.append(y)
print(list1)

