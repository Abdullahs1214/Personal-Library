import re

# Sample input text (the entire question block)
text = """
Question 1 Correct Mark 1.00 out of 1.00 Y Flag question

Which one of the following statements is incorrect regarding cancer and combined hormonal

contraceptives (CHC) use?

Select one:

a. The use of CHC for more than 5 years is associated with a small increase in the risk of ovarian
cancer.

b. The use of CHC is acceptable for a woman whose mother had breast cancer.

c. Increased risk of breast cancer associated with CHC use declines to become non-significant after
10 years of non-use.

d. The use of CHC is associated with reduced risk of endometrial cancer and colorectal cancer.

Your answer is correct.

The correct answer is: The use of CHC for more than 5 years is associated with a small increase in the risk of
ovarian cancer.
"""

# Extracting the question
question_pattern = r"Question \d+ Correct Mark \d+\.\d+ out of \d+\.\d+ Y Flag question\n\n(.*?)\n\nSelect one:"
question_match = re.search(question_pattern, text, re.DOTALL)
question = question_match.group(1).strip() if question_match else ""

# Extracting the options
options_pattern = r"Select one:\n\n(.*?)\n\nYour answer is correct."
options_match = re.search(options_pattern, text, re.DOTALL)
options_block = options_match.group(1).strip() if options_match else ""

# Extracting options without letters
options = [line.split(". ", 1)[1].strip() for line in options_block.splitlines() if line]

# Extracting the correct answer
correct_answer_pattern = r"The correct answer is: (.*)"
correct_answer_match = re.search(correct_answer_pattern, text)
correct_answer = correct_answer_match.group(1).strip() if correct_answer_match else ""

# Output the results
print("Question:", question)
print("Options:", options)
print("Correct Answer:", correct_answer)
