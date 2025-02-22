# UNFAIR DICE TEMPLATE: REPLACE THIS LINE WITH YOUR FILE HEADER
import random
def biased_rolls(prob_list, s, n): 
    """ Simulate n rolls of a biased m-sided die and return
    a list containing the results. 
    
    Arguments:
        prob_list: a list of the probabilities of rolling the 
                   number on each side of the m-sided die. The list  
                   will always have the length m (m >= 2), where m is 
                   the number of sides numbered 1 to m. Therefore,  
                   for example, the probability stored at index 0 in 
                   the list is the probability of rolling a 1 on
                   the m-sided die.
        s: the seed to use when initializing the PRNG
        n: the number of rolls to return

    Return:
        rolls: a list (of length n) containing each of the n rolls of the 
               biased die, in the order they were generated.
    """
    random.seed
    prob_list[0] = 1/2
    s = 1
    n = 100 
    
    # replace this line and solve the problem!
    rolls = None

    # return the resulting rolls
    return rolls


def draw_histogram(m, rolls, width):
    """ Draws a frequency histogram of the rolls of an m-sided die
    mapped to a fixed width.

    Arguments: 
        m (int): the number of sides on the die
        rolls (list): the list of rolls generated by the biased die
        width (int): the fixed width of the histogram, in characters
                     (this is the length of the longest bar in the 
                     histogram, to maximize space in the chart)

    Returns:
        None (but prints the histogram to standard output)
    """
    # solve the problem here by replacing the pass keyword
    pass

    # this function should print to standard output
    # you should not return anything!


if __name__ == "__main__":
    # Any code indented under this line will only be run
    # when the program is called directly from the terminal
    # using "python3 unfairDice.py". This can be useful for
    # testing your implementations.
    print("No output")
    pass
