An exhaustive unlexicalised PCFG parser for the Penn Treebank. It does
no Markovisation of rules and no annotation of internal symbols.

## Learn Grammar
1. Read Treebank Trees (texpr)
2. Binarize Trees
3. Read Grammar Off Trees
4. Output 2NF Rules With Probability
5. Calculate and output nullable symbols and inverse unary relations with probabilities

## Parse Input
1. Read Grammar
2. Read Input Sentence (tokens with POS tags)
3. Run CKY Algorithm
4. Output Parse Tree From CKY Table (texpr)
