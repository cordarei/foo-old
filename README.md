An exhaustive unlexicalised PCFG parser for the Penn Treebank. It does
no Markovisation of rules and no annotation of internal symbols.

## Learn Grammar
1. Read Treebank Trees (texpr)
2. Remove function tags (“-LOC” etc) and “-NONE” elements
3. Binarize Trees
4. Read Grammar Off Trees
5. Output 2NF Rules With Probability
6. Calculate and output nullable symbols and inverse unary relations with probabilities

## Parse Input
1. Read Grammar
2. Read Input Sentence (tokens with POS tags)
3. Run CKY Algorithm
4. Output Parse Tree From CKY Table (texpr)
