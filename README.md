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




