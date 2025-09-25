def wheelspinner():
    import random
    my_list = []
    flag = True
    n = int(input("How many options: "))
    name = (input("Enter the names of options: "))
    names = name.split(" ")

    while flag == True:
        x = random.randint(0,n-1)
        my_list.append(x)
        for i in range(n):
            if my_list.count(i) == 3:
                flag = False
                print(i+1)
                print(names[i]) 
                
wheelspinner()