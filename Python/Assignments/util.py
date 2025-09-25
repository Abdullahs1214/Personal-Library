import bitio
import huffman
import pickle

def read_tree(tree_stream): 
  tree = pickle.load(tree_stream)
  return tree

def decode_byte(tree, bitreader): 

  while not isinstance(tree, huffman.TreeLeaf): 
    #splits depending if the bit is 1 or 0, right or left
    if bitreader.readbit():
      tree = tree.getRight() 
    else:
      tree = tree.getLeft()
  #return the leaf value found
  tree_val = tree.getValue()
  return tree_val

def decompress(compressed, uncompressed):
  try:
    #creating a tree
    tree = read_tree(compressed)
  except Exception:
    #stop if it cannot create tree
    return None
  #open and read using bitio
  bitread = bitio.BitReader(compressed)
  bitwrite =  bitio.BitWriter(uncompressed)
  #decode file until an error is reached, commonly EOF
  try:
    decode = decode_byte(tree, bitread)
  except EOFError:
    decode = None
  while decode != None:
    try:
      bitwrite.writebits(decode, 8)
      decode = decode_byte(tree, bitread)
      
    #break the while loop if it is EOF error
    except EOFError:
      break

def write_tree(tree, tree_stream): 
  pickle.dump(tree, tree_stream)

def compress(tree, uncompressed, compressed):
  try:
    write_tree(tree, compressed)
  except Exception:
    return None
  encoding_table = huffman.make_encoding_table(tree)
  
  #open and read using bitio
  bitread = bitio.BitReader(uncompressed)
  bitwrite = bitio.BitWriter(compressed)

  #until EOF error, encoded code (8 bit) is written bitwise
  cont = True
  while cont: 
    try:
      code = bitread.readbits(8)
    except EOFError: 
      
      #no encoded code if EOF is reached
      code = None
      cont = False
    for bit in encoding_table[code]:
        bitwrite.writebit(bit)
  bitwrite.flush()
