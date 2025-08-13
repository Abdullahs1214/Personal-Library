import random
import InDSlayerNames

def userSelection():
    choice = int(input("Make your selection: 1 for Solo, 2 for Coop, 3 for Both: "))
    if choice == 1:
        print("solo")
    elif choice == 2:
        print("coop")
    else:
        print("both")
        return choice

def randomizer():
    return 0
def InDSlayer():
   mode = userSelection()

InDSlayer()