An exhaustive unlexicalised PCFG parser. It does no Markovisation of
rules and no annotation of internal symbols.

## Learn Grammar
1. Read treebank trees (texpr)
2. Remove function tags (“-LOC” etc) and “-NONE” elements
3. Binarize trees
4. Read grammar off trees
5. Output 2NF [^1] rules with probability
6. Output inverse unary relations

## Parse Input
1. Read grammar
2. Read input sentence (tokens with POS tags)
3. Run CKY algorithm
4. Output parse tree from CKY table (texpr)

[^1]: For an explication of the CKY formulation followed by this parser see [here](http://www.informatica-didactica.de/cmsmadesimple/index.php?page=LangeLeiss2009)
