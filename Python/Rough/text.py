def decode(message_file):
    with open(message_file, 'r') as file:
        lines = file.readlines()
    message_words = []
    max_number = 0
    for line in lines:
        parts = line.split()
        number = int(parts[0])
        word = ' '.join(parts[1:])
        if number > max_number:
            max_number = number
        message_words.append((number, word))
    pyramid = [''] * (max_number + 1)
    for number, word in message_words:
        pyramid[number] = word
    decoded_message = ' '.join(pyramid[1:])
 
    return decoded_message
message_file = r'C:\Users\srskh\Downloads\text.txt'  # Replace with the actual file path
decoded_message = decode(message_file)
print(decoded_message)