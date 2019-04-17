# XtsIScript
XtsIScript
  
  - early stage project

  - vars can be :
     32b int
     32b float
     String

     array[] of int, float, String

     statements are typed objects w/ an address

     tokenization produces a file on storage system (to free RAM)

     Heap can be paginated then sent to storage (to free RAM)
       - ex. paginated per current script that been called......
       - can be defragmented

    ac$    => [TYPE_String][String zero terminated]
    ab$    => [TYPE_ARRAY][LENGTH]
    ab$[1] => [TYPE_ARRAY][INDEX][TYPE_String][String zero terminated]
                   1B        1B     xB
                              \  so max array length is 255

    dim ab$[255];
    ac$ = "Coucou";
 
    register = {
        [ "ab$00", arrayIndex, addr1, addr0 ], 64KB page addressable
        5 long 0 padding / 1byte for array cell index / 8 bytes per entry
    }

    VAR         |  REGISTER   | HEAP
    ------------+-------------+----------+
    ac$    =>   | ac$000YX    | 2Coucou les gens0
    ab$[1] =>   | ab$001YX    | 1xxxx    | ==> YX heap addr // xxxx 32b integer
    ab$    =>   | ab$00?YX    | 3000x    | ==> ?(0xFF) arrayVar descriptor // x uint8_t array length ==> ????
     \__ Cf need to pass array descriptor as fct arg. : 
            need a specific pointer so array limit of 256 elems goes to 255 ( [0..254] )
            then [255] becomes the array itself

    /!\\ ok = testAssignOneInt("ab", 2, 65000); => returns -792

    Memory fragmentation : 
    ac$ = "Coucou";            |   Coucou0                                    | initial Str
    ac$ = "Couc";              |   Couc000                                    | truncate Str (padd)
    ac$ = "Coucou";            |   0000000Coucou0                             | more than previous size (0padd + reallocate)
    ac$ = "Coucou les gens";   |   0000000Coucou les gens0                    | as is last value in heap -> grows
    new = 12                   |   0000000Coucou les gens0X12                 | new value in heap -> add it
    ac$ = "Coucou les gens 2"; |   00000000000000000000000X12Coucou les gens0 | bigger than previous size -> (0padd + reallocate)
    
    later : may find a way to preallocate String length
            could use a 'break-padding-char'
            or store String length somewhere
              - @ this time : a String can virtually be 64KB long (+0)
              - if store length -> we have to delimit max String precision
                - need to keep some memory space for that length storage
                - each time even if not needed for all values
                - stored in a separated space
              - BOTH cases consumes more RAM than now ....

      - if there's no value after : 
        - slow to run 'till end of Heap
        - find if there is a value in the HeapRegister after ? (faster)
          - could code a findVar(name, idx) : 
            - findNextVar(name, idx)
            - findPrevVar(name, idx)

      - if there's a value after
        - it CAN'T start by a zero (Cf typed value) so last zero is the String maxLen

            








